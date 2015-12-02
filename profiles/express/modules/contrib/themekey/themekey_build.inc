<?php

/**
 * @file
 * The functions in this file are the back end of ThemeKey which should be
 * used only if you configure something, but not when ThemeKey switches themes.
 *
 * @author Markus Kalkbrenner | Cocomore AG
 *   @see http://drupal.org/user/124705
 *
 * @author profix898
 *   @see http://drupal.org/user/35192
 */


/**
 * Creates options array for a theme select box.
 *
 * Example:
 *   $form['theme'] = array(
 *     '#type' => 'select',
 *     '#title' => t('Theme'),
 *     '#options' => themekey_theme_options(),
 *   );
 *
 * @param $default
 *   Boolean to indicate if options array should contain
 *   'System default' theme. Default is TRUE.
 * @param $admin
 *   Boolean to indicate if options array should contain
 *   'Administration theme'. Default is FALSE.
 *
 * @return
 *   options array for a theme select box
 */
function themekey_theme_options($default = TRUE, $admin = FALSE) {
  $themes = list_themes();
  ksort($themes);

  $options_themes = array();
  if ($default) {
    $options_themes['default'] = '=> ' . t('System default');
  }
  foreach ($themes as $theme) {
    if ($theme->status || variable_get('themekey_allthemes', 0)) {
      $options_themes[$theme->name] = $theme->info['name'];
    }
  }
  if ($admin) {
    $options_themes['ThemeKeyAdminTheme'] = '=> ' . t('Administration theme');
  }

  return $options_themes;
}


/**
 * Rebuilds all ThemeKey-related Drupal variables
 * by calling ThemeKey's hooks:
 * - hook_themekey_properties()
 * - hook_themekey_paths()
 */
function themekey_rebuild() {
  // includes all modules in the themekey/modules subfolder (internal modules)
  themekey_scan_modules();

  module_load_include('inc', 'themekey', 'themekey_base');

  // Get property definitions (from internal and other modules)
  $properties = array_merge_recursive(themekey_invoke_modules('themekey_properties'), module_invoke_all('themekey_properties'));

  // Attributes
  $attributes = isset($properties['attributes']) ? $properties['attributes'] : array();
  ksort($attributes);

  $property_names = array();
  foreach ($attributes as $property_name => $attribute) {
    if (empty($attribute['static'])) {
      if (preg_match("/^[\w-]+:[:\w-]+$/", $property_name)) {
        $property_names[$property_name] = $property_name;
      }
      else {
        drupal_set_message(t('%property is not a valid ThemeKey property name.', array('%property' => $property_name)), 'error');
      }
    }

    if (!isset($attribute['page cache'])) {
      $attributes[$property_name]['page cache'] = THEMEKEY_PAGECACHE_UNSUPPORTED;
    }
  }
  variable_set('themekey_attributes', $attributes);
  variable_set('themekey_properties', $property_names);

  // Property maps
  $maps = isset($properties['maps']) ? $properties['maps'] : array();
  variable_set('themekey_maps', $maps);

  // Get (and register) paths from themekey modules
  $paths = array_merge_recursive(themekey_invoke_modules('themekey_paths'), module_invoke_all('themekey_paths'));

  // assign fit factor and weight to this item
  array_walk($paths, 'themekey_path_set');

  $paths_sort = array();
  foreach ($paths as $item) {
    $paths_sort[$item['fit']][$item['weight']][] = $item;
  }
  ksort($paths_sort, SORT_NUMERIC);

  $paths = array();
  foreach (array_reverse($paths_sort) as $same_fit) {
    ksort($same_fit, SORT_NUMERIC);
    foreach (array_reverse($same_fit) as $same_weight) {
      foreach ($same_weight as $item) {
        $paths[] = $item;
      }
    }
  }

  variable_set('themekey_paths', $paths);
}


/**
 * Scans directory themekey/modules for suitable files
 * which provide ThemeKey properties mapping function and so on
 * and stores the file names, for later use, in a Drupal variable
 * called 'themekey_modules'.
 *
 * @see themekey_rebuild()
 * @see themekey_invoke_modules()
 */
function themekey_scan_modules() {
  $modules = array();
  $files = file_scan_directory(dirname(__FILE__) . '/modules', '/^themekey.[a-z]+.inc$/');
  foreach ($files as $file) {
    list( , $module) = explode('.', $file->name);
    if (module_exists($module)) {
      $modules[] = $file->name;
    }
  }

  variable_set('themekey_modules', $modules);
}


/**
 * Named wildcards in ThemeKey rules based on property
 * drupal:path are stored as serialized array in the database.
 *
 * This function deserializes those wildcards and injects them back
 * into the value of the rule. This format is needed by ThemeKey's
 * administration interface.
 *
 * It's the counterpart of these functions:
 * @see themekey_prepare_path()
 * @see themekey_prepare_custom_path()
 *
 * @see themekey_load_rules()
 *
 * @param $item
 *   reference to an inject
 *   containing a ThemeKey rule as returned
 *   directly from database
 */
function themekey_complete_path($item) {
  $item->wildcards = unserialize($item->wildcards);
  if (count($item->wildcards)) {
    $parts = explode('/', $item->value, MENU_MAX_PARTS);
    foreach ($item->wildcards as $index => $wildcard) {
      $parts[$index] .= $wildcard;
    }
    $item->value = implode('/', $parts);
  }
}


/**
 * Examines ThemeKey paths created by modules
 * via hook_themekey_paths() in database and
 * assigns a fit factor and a weight.
 *
 * @see themekey_rebuild()
 *
 * @param $item
 *   reference to an associative array
 *   containing a ThemeKey path structure
 */
function themekey_path_set(&$item) {
  $item['callbacks'] = (isset($item['callbacks']) && !empty($item['callbacks'])) ? $item['callbacks'] : array();

  list($item['fit'], $item['weight'], $item['wildcards']) = themekey_prepare_path($item['path']);
}


/**
 * Extracts named wildcards from ThemeKey paths returned
 * by modules via hook_themekey_paths() and associates a
 * weight and a fit factor to this path.
 *
 * @param $item
 *   reference to path as string
 *
 * @return
 *   array containing three elements:
 *   - fit as integer
 *   - weight as integer
 *   - named wildcards as array
 */
function themekey_prepare_path(&$path) {
  $fit = 0;
  $weight = 0;
  $wildcards = array();

  $parts = explode('/', $path, MENU_MAX_PARTS);
  $slashes = count($parts) - 1;
  foreach ($parts as $index => $part) {
    if (preg_match('/^(\%|\#)([a-z0-9_:]*)$/', $part, $matches)) {
      $parts[$index] = $matches[1];
      if (!empty($matches[2])) {
        $wildcards[$index] = $matches[2];
      }
      if ($matches[1] == '#') {
        $weight |=  1 << ($slashes - $index);
      }
    }
    else {
      $fit |=  1 << ($slashes - $index);
    }
  }
  $path = implode('/', $parts);

  return array($fit, $weight, $wildcards);
}


/**
 * Extracts named wildcards from paths entered as value
 * in a ThemeKey rule with property drupal:path.
 *
 * @param $path
 *   path as string
 *
 * @return
 *   array containing two elements:
 *   - path with unnamed wildcards
 *   - named wildcards as array
 */
function themekey_prepare_custom_path($path) {
  $wildcards = array();

  $parts = explode('/', $path, MENU_MAX_PARTS);
  foreach ($parts as $index => $part) {
    if (preg_match('/^(\%|\#)([a-z0-9_:]*)$/', $part, $matches)) {
      $parts[$index] = $matches[1];
      if (!empty($matches[2])) {
        $wildcards[$index] = $matches[2];
      }
    }
  }
  $path = implode('/', $parts);

  return array($path, $wildcards);
}


/**
 * Loads all ThemeKey Rules from the database.
 * Therefore, it uses recursion to build the rule chains.
 *
 * @param $parent
 *   id of the parent rule. Default is '0'.
 *   During the recursion this parameter changes.
 *
 * @param $depth
 *   Integer that represents the 'indentation'
 *   in current rule chain. Default is '0'.
 *   During the recursion this parameter changes.
 *
 * @return
 *   sorted array containing all ThemeKey rules
 */
function themekey_load_rules($parent = 0, $depth = 0) {
  static $properties = array();
  static $parent_lookups = array();

  if (isset($parent_lookups[$parent])) {
    // prevent endless recursion in case of malformated data in database
    return $properties;
  }

  $result = db_select('themekey_properties', 'tp')
    ->fields('tp')
    ->condition('parent', $parent)
    ->orderBy('weight', 'asc')
    ->execute();

  foreach ($result as $item) {
    if ('drupal:path' == $item->property) {
      themekey_complete_path($item);
    }
    $item->depth = $depth;
    $properties[$item->id] = get_object_vars($item);
    themekey_load_rules($item->id, $depth + 1);
    $parent_lookups[$item->id] = TRUE;
  }
  return $properties;
}


/**
 * Stores ThemeKey rules in database.
 * It creates a new dataset or updates an existing one.
 *
 * @param $item
 *   reference to an associative array
 *   containing a ThemeKey rule structure:
 *   - id
 *   - property
 *   - operator
 *   - value
 *   - weight
 *   - theme
 *   - enabled
 *   - wildcards
 *   - parent
 *
 */
function themekey_rule_set(&$item) {
  if ('drupal:path' == $item['property']) {
    list($item['value'], $item['wildcards']) = themekey_prepare_custom_path($item['value']);
  }
  else {
    $item['wildcards'] = array();
  }

  if (empty($item['id'])) {

    // TRANSACTIONS - SEE http://drupal.org/node/355875
    // The transaction opens here.
    $txn = db_transaction();

    // new entry should be added at the end of the chain
    $result = db_select('themekey_properties', 'tp');
    $result->addExpression('MAX(weight)', 'weight');
    $weight = $result->execute()->fetchField();

    // if query fails $weight will be FALSE which will cause $item['weight'] to be set to '1'
    $item['weight'] = 1 + $weight;
    drupal_write_record('themekey_properties', $item, array());

  }
  else {
    drupal_write_record('themekey_properties', $item, 'id');
  }

  // $txn goes out of scope here, and the entire transaction commits.
}


/**
 * Deletes a ThemeKey rule from database.
 *
 * @param $id
 *   id of the rule to be deleted from database
 */
function themekey_rule_del($id) {

  // TRANSACTIONS - SEE http://drupal.org/node/355875
  // The transaction opens here.
  $txn = db_transaction();

  $result = db_select('themekey_properties', 'tp');
  $result->condition('parent', $id);
  $result->addExpression('COUNT(*)', 'num_childs');
  $num_childs = $result->execute()->fetchField();

  if (FALSE !== $num_childs) {
    if ($num_childs > 0) {
      drupal_set_message(t('ThemeKey rule could not be deleted because it has children in the chain'), 'error');
    }
    else {
      $result = db_delete('themekey_properties')
      ->condition('id', $id)
      ->execute();
      if (!$result) {
        drupal_set_message(t('Error while deleting ThemeKey rule'), 'error');
      }
    }
  }
  else {
    drupal_set_message(t('Error while deleting ThemeKey rule'), 'error');
  }

  // $txn goes out of scope here, and the entire transaction commits.
}


/**
 * Loads ThemeKey rule from database.
 *
 * @param $id
 *   id of the rule to be loaded from database
 *
 * @return
 *   the rule as associative array or NULL
 */
function themekey_rule_get($id) {

  if ($result = db_select('themekey_properties', 'tp')
    ->fields('tp')
    ->condition('id', $id)
    ->execute()
  ) {

    foreach ($result as $item) {
      if ('drupal:path' == $item->property) {
        themekey_complete_path($item);
      }
      return $item;
    }

  }

  return NULL;
}


/**
 * Adds or modifies a so-called static rule in the
 * database. Static rules can be moved around in the chain
 * and enabled or disabled by the site administrator, but the values
 * are immutable. There's one static rule per static property.
 *
 * @param $property
 *   name of the static property as string
 *
 * @param $state
 *   boolean:
 *   - TRUE the rule should be created or updated
 *   - FALSE the rule should be deleted
 */
function themekey_update_static_rule($property, $state) {

  $id = db_select('themekey_properties', 'tp')
    ->fields('tp', array('id'))
    ->condition('property', $property)
    ->execute()
    ->fetchField();


  if ($state) {
    $item = array(
      'property' => $property,
      'operator' => '=',
      'value' => 'static',
      'theme' => 'default',
    );

    if ($id) {
      $item['id'] = $id;
      // leave 'enabled' as it is in database
    }
    else {
      // enable new rule
      $item['enabled'] = 1;
    }

    themekey_rule_set($item);
  }
  elseif ($id) {
    themekey_rule_del($id);
  }
}
