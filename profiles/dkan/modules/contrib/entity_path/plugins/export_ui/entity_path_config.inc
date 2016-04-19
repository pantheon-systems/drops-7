<?php

/**
 * @file
 *  UI configuration for entity_path_config exportable objects.
 */

$plugin = array(
  'schema' => 'entity_path_config',
  'access' => 'administer site configuration',

  'menu' => array(
    'menu prefix' => 'admin/config/content',
    'menu item' => 'entity_path',
    'menu title' => 'Entity Paths',
    'menu description' => 'Administer internal paths of entities.',
  ),

  'title singular' => t('entity path'),
  'title singular proper' => t('Entity path'),
  'title plural' => t('entity paths'),
  'title plural proper' => t('Entity paths'),

  'form' => array(
    'settings' => 'entity_path_ctools_export_ui_form',
    //'validate' => 'entity_path_ctools_export_ui_form_validate',
    'submit' => 'entity_path_ctools_export_ui_form_submit',
  ),
);

/**
 * UI form for the entity_path configuration.
 */
function entity_path_ctools_export_ui_form(&$form, &$form_state) {

  $item = $form_state['item'];

  // Get the instance from the form state (e.g. ajax) or the default object.
  $instance = (isset($form_state['values']['instance'])) ? $form_state['values']['instance'] : $item->instance;

  $form['info']['instance'] = array(
    '#type' => 'select',
    '#title' => t('Instance'),
    '#options' => _entity_path_instance_options(TRUE, $instance, FALSE),
    '#default_value' => $instance,
    '#required' => TRUE,
    '#description' => t('Select the entity type or bundle to build this configuration for.'),
    '#ajax' => array(
      'callback' => 'entity_path_ctools_export_ui_form_ajax_replacement',
      'wrapper' => 'replacement-patterns',
      'method' => 'replace',
    ),
  );
  $form['path_pattern'] = array(
    '#type' => 'textfield',
    '#title' => t('Path pattern'),
    '#default_value' => $item->path_pattern,
    '#required' => TRUE,
    '#description' => t('Specify the internal path for the selected instance (entity type, bundle): e.g. <code>genre/[term:tid]</code> for a taxonomy term.'),
  );
  $form['query_pattern'] = array(
    '#type' => 'textfield',
    '#title' => t('Query pattern'),
    '#default_value' => $item->query_pattern,
    '#field_prefix' => '?',
    '#required' => FALSE,
    '#description' => t('Attach a query string to the path defined above, like <code>tid=[term:tid]&name=[term:title]</code>. You can use token replacement only for values, but not for keys.'),
  );
  $form['fragment_pattern'] = array(
    '#type' => 'textfield',
    '#title' => t('Fragment pattern'),
    '#default_value' => $item->fragment_pattern,
    '#field_prefix' => '#',
    '#required' => FALSE,
    '#description' => t('Attach a fragment to the path defined above, like <code>nid-[node:nid]</code>.'),
  );

  // Replacement pattern help.
  $form['replacement'] = array(
    '#type' => 'fieldset',
    '#title' => t('Replacement patterns'),
    '#attributes' => array(
      'id' => 'replacement-patterns',
    ),
  );
  if (module_exists(('token'))) {
    if (empty($instance)) {
      $form['replacement']['token_empty'] = array(
        '#prefix' => '<span class="description warning">',
        '#markup' => t('Please select an instance first to view replacement patterns.'),
        '#suffix' => '</span>',
      );
    }
    $form['replacement']['token_help'] = array(
      '#theme' => 'token_tree',
      '#token_types' => array(_entity_path_get_token_type($instance)),
      '#global_types' => FALSE,
    );
  }
  else {
    $form['replacement']['token_missing'] = array(
      '#prefix' => '<span class="description warning">',
      '#markup' => t('The !token module is needed for a preview of replacement patterns. It also would provide some additional tokens.', array('!token' => l('Token', 'http://drupal.org/project/token'))),
      '#suffix' => '</span>',
    );
  }
  // Notice for entity_token module.
  if (module_exists(('entity')) && !module_exists('entity_token')) {
    $form['replacement']['token_entity'] = array(
      '#prefix' => '<span class="description notice">',
      '#markup' => t('As you have enabled the Entity API, you also might enable the Entity Token module for providing token support for additional properties.'),
      '#suffix' => '</span>',
    );
  }

  // Additional configuration.
  $form['config'] = array(
    '#type' => 'fieldset',
    '#title' => t('Additional configuration'),
    '#tree' => TRUE,
  );

  if (module_exists('pathauto')) {
    $form['config']['pathauto_cleanstring'] = array(
      '#type' => 'checkbox',
      '#title' => t('Pathauto cleanstring'),
      '#description' => t('Clean up the token replacements with <code>pathauto_cleanstring()</code>, using the !pathauto_settings.', array('!pathauto_settings' => l(t('Pathauto settings'), 'admin/config/search/path/settings'))),
      '#default_value' => !empty($item->config['pathauto_cleanstring']),
    );
  }

  ctools_include('dependent');
  $form['config']['space_separator'] = array(
    '#id' => 'edit-config-space-separator',
    '#type' => 'checkbox',
    '#title' => t('Space Separator'),
    '#description' => t('Use a character to replace spaces and "+" characters.'),
    '#default_value' => !empty($item->config['space_separator']),
  );
  $form['config']['space_separator_char'] = array(
    '#type' => 'textfield',
    '#title' => t('Separator character'),
    '#description' => t("Character used to separate words in titles. This will replace any spaces and + characters. Using a space or + character can cause unexpected results.  Leave empty if you don't want to replace these characters.  This happens after the text above is removed."),
    '#maxlength' => 1,
    '#size' => 1,
    '#default_value' => (isset($item->config['space_separator_char'])) ? $item->config['space_separator_char'] : '_',
    '#dependency' => array('edit-config-space-separator' => array(1)),
  );
  $form['config']['path_case'] = array(
    '#type' => 'select',
    '#title' => t('Path Case'),
    '#description' => t('How to transform the case of the path. This happens after the separator character is replaced.'),
    '#options' => array(
      '' => t('No transform'),
      'uppercase' => t('Uppercase'),
      'lowercase' => t('Lowercase'),
     ),
    '#default_value' => (isset($item->config['path_case'])) ? $item->config['path_case'] : '',
  );


  return $form;
}

/**
 * Helper function for options array for entity/bundle selection.
 * @param bool $exclude
 *   indicator to exclude allready defined instances
 * @param string $include
 *   instance to include, although it might have been allready defined
 * @param bool $flat
 *   indiactor to flatten array, else array will returned as optgrouped
 * @return array
 *   either optgroup or flat array for usage in
 */
function _entity_path_instance_options($exclude = TRUE, $include = NULL, $flat = TRUE) {
  $info = entity_get_info();
  $options = array();
  foreach ($info as $entity_type => $spec) {
    $entity_name = check_plain($info[$entity_type]['label']) . ' (' . check_plain($entity_type) .')';
    // Any or no bundle
    foreach (array_merge(array(''), array_keys($spec['bundles'])) as $bundle) {
      $instance = _entity_path_get_instance($entity_type, $bundle);
      // Only add instances to options, when we do not exclude, it is especially
      // included or is not defined as entity path allready.
      if (!$exclude
          || $include == $instance
          || empty($spec['entity path config'][$bundle])) {
        if ($bundle) {
          $bundle_name = check_plain($spec['bundles'][$bundle]['label']) . '(' . check_plain($bundle) .')';
        }
        else {
          $bundle_name = t('- Any bundle -');
        }

        // Build a flat array, if specified.
        if ($flat) {
          $options[$instance] = "$entity_name | $bundle_name";
        }
        // Build an optgroup array.
        else {
          $options[$entity_name][$instance] = $bundle_name;
        }
      }
    }
  }
  return $options;
}

/**
 * Additional submission callback for ui form.
 */
function entity_path_ctools_export_ui_form_submit(&$form, &$form_state) {
  entity_info_cache_clear();
}

/**
 * Ajax helper callback for the replacement pattern fieldset.
 * @param array $form
 * @param array $form_state
 * @return array
 */
function entity_path_ctools_export_ui_form_ajax_replacement($form, $form_state) {
  return $form['replacement'];
}
