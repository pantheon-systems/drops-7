<?php

/**
 * @file
 * Utility functions.
 */

/**
 * Drupal JSON Output - CORS - allows cross domain requests
 * (adapted from: drupal_json_output)
 * @param type $var
 */
function lingotek_json_output_cors($var = NULL, $status = "200", $params = array()) {
  // We are returning JSON, so tell the browser.
  $methods_allowed = isset($params['methods_allowed']) ? $params['methods_allowed'] : 'GET,PUT,POST,DELETE';
  drupal_add_http_header('Status', $status);
  drupal_add_http_header('Content-Type', 'application/json');
  drupal_add_http_header('Access-Control-Allow-Origin', "*");
  drupal_add_http_header('Access-Control-Allow-Methods', $methods_allowed);
  drupal_add_http_header('Access-Control-Allow-Headers', 'Content-Type');
  drupal_add_http_header('X-Powered-By', 'Lingotek');
  if (isset($var)) {
    echo drupal_json_encode($var);
  }
}

/*
 * Helper function, for storing additional information with an Entity.
 * Example usage (GET):
 *    lingotek_keystore($entity_type, 'all') - returns all entities and properties
 *    lingotek_keystore($entity_type, 5) - returns all properties for the specified Entity ID (i.e., 5)
 *    lingotek_keystore($entity_type, 5,'node_sync_status') - returns the value for the specified property (i.e., node_sync_status) for the specified Entity ID (i.e., 5)
 * Example usage (SET):
 *     lingotek_keystore($entity_type, 5,'node_sync_status','CURRENT') - sets the value to 'CURRENT' for the property 'node_sync_status' of Entity 5 of type $entity_type
 *
 * @param $nid
 *  Entity ID.
 * @param $key
 *  (optional) "" Key to look up in the database.  If no key is specified, then
 *  every key for the Entity is returned with it's value.
 * @param $value
 *  (optional) "" Value to save.  If "" or no value is given for $value, then
 *  it will return the $value of the first found instance of the specified $key
 *  in the database.  Returns FALSE if no value is found.
 */

function lingotek_keystore($entity_type, $entity_id, $key = "", $value = "", $update_on_dup = TRUE) {
  if ($entity_id == 'all') {
    $lingo_node = array();
    $result = db_select('{lingotek_entity_metadata}', 'n')
        ->condition('entity_type', $entity_type)
      ->fields('n', array(db_escape_field('entity_id'), db_escape_field('entity_key'), db_escape_field('value')))
      ->execute();
    foreach ($result as $row) {
      $lingo_node[$row->nid][$row->entity_key] = check_plain($row->value);
    }
    return $lingo_node;
  }
  if ($entity_id == -1) {
    LingotekLog::error("Invalid -1 entity ID passed to lingotek_keystore().", array('@nid' => $entity_id, '@key' => $key, '@value' => $value));
    return FALSE;
  }
  if (is_numeric($entity_id) && $entity_id) {
    //Return an array with all of the keys and values.
    if ($key === "") {
      $lingo_node = array();

      $result = db_select('{lingotek_entity_metadata}', 'n')
          ->fields('n', array(db_escape_field('entity_key'), db_escape_field('value')))
          ->condition('entity_type', $entity_type)
        ->condition(db_escape_field('entity_id'), $entity_id)
        ->execute();
      foreach ($result as $row) {
        $lingo_node[$row->entity_key] = check_plain($row->value);
      }

      return $lingo_node;
    }
    //Get value for the specified key, only returns the first match
    elseif ($value === "") {
      $result = db_select('{lingotek_entity_metadata}', 'n')
          ->fields('n', array(db_escape_field('value')))
        ->condition('entity_type', $entity_type)
        ->condition(db_escape_field('entity_id'), $entity_id)
        ->condition(db_escape_field('entity_key'), $key)
        ->execute();
      $row = $result->fetchObject();

      if ($row) {
        return check_plain($row->value);
      }
      else {
        return FALSE;
      }
    }
    //Insert/Update the specified key and value
    else {
      $timestamp = time();
      $row = array(
        db_escape_field('entity_type') => $entity_type,        
        db_escape_field('entity_id') => $entity_id,
        db_escape_field('entity_key') => $key,
        db_escape_field('value') => $value,
        db_escape_field('created') => $timestamp,
        db_escape_field('modified') => $timestamp
      );
      $existing_value = lingotek_keystore($entity_type, $entity_id, $key);
      if ($existing_value === FALSE) { // insert
        $success = drupal_write_record('lingotek_entity_metadata', $row);
        return $success ? "$entity_id : $key => $value (INSERTED)" : "$entity_id : $key !=> $value (INSERT FAILED)";
      }
      elseif ($update_on_dup) { // key exists -> update
        unset($row[db_escape_field('created')]); // retain original created timestamp
        $success = drupal_write_record('lingotek_entity_metadata', $row, array(db_escape_field('entity_type'), db_escape_field('entity_id'), db_escape_field('entity_key')));
        return $success ? "$entity_id : $key => $value (UPDATED)" : "$entity_id : $key !=> $value (UPDATE FAILED)";
      }
      else { // key exists -> ignore (ignore on duplicate key)
        return "$entity_id : $key => $existing_value (IGNORED)";
      }
    }
  }
  else {
    LingotekLog::error("Invalid entity ID (@entity_id) passed to lingotek_keystore().", array('@entity_id' => $entity_id, '@key' => $key, '@value' => $value));
    return FALSE;
  }
}

/*
 * Helper function to delete a specified variable from the Lingotek table
 *
 * @param int $nid
 *    node id for the variable that needs to be deleted
 *
 * @param string $lingokey
 *    variable name to be deleted
 *
 */

function lingotek_keystore_delete($entity_type, $id, $key) {
  $query = db_delete('{lingotek_entity_metadata}');
  $query->condition('entity_type', $entity_type);
  $query->condition('entity_id', $id);
  $query->condition('entity_key', $key);
  return $query->execute();
}

/**
 * Helper function to delete a specified variable from multiple nodes in the Lingotek table
 *
 * @param int array $entity_ids
 *    array of node ids for the variable that needs to be deleted
 *    if a single nid is passed in, it will be converted to an array before processing
 *
 * @param string $lingokey
 *    variable name to be deleted
 *
 * @param string $condition
 *    additional condition checking (i.e. 'LIKE', '<' '=' '>', etc...)
 *    defaults to '='
 */
function lingotek_keystore_delete_multiple($entity_type, $entity_ids, $lingokey, $condition = '=') {
  if (!is_array($entity_ids)) {
    $entity_ids = array($entity_ids);
  }

  $query = db_select('{lingotek_entity_metadata}', 'l') // Make sure that the variable exists in the table for every input nid
      ->fields('l', array('entity_id'))
      ->condition('entity_type', $entity_type)
      ->condition('entity_id', $entity_ids, 'IN')
      ->condition('entity_key', $lingokey, $condition);
  $delete_nids = $query->execute()->fetchCol();

  if (!empty($delete_nids)) {
    $delete = db_delete('{lingotek_entity_metadata}') // delete for every input nid where the variable exists
        ->condition('entity_type', $entity_type)
        ->condition('entity_id', $delete_nids, 'IN')
        ->condition('entity_key', $lingokey, $condition)
        ->execute();
  }
}

function lingotek_keystore_delete_all($entity_type, $id) {
  db_delete('{lingotek_entity_metadata}')
      ->condition('entity_type', $entity_type)
    ->condition('entity_id', $id)->execute();
}

function lingotek_keystore_delete_all_multiple($entity_type, $ids) {
  db_delete('{lingotek_entity_metadata}')
      ->condition('entity_type', $entity_type)
    ->condition('entity_id', $ids, 'IN')->execute();
}

/*
 * Filter for removing unchecked checkboxes from an array for drupal forms
 */

function lingotek_unselected($var) {
  return ($var != "0");
}

/*
 * COALESCE(LingotekVariable, DrupalVariable, Default)
 */

function lingotek_variable_get($var, $drupal, $default) {
  if ($var === FALSE) {
    return variable_get($drupal, $default);
  }
  else {
    return $var;
  }
}

/*
 * Get a string representation of an object
 *
 * @param $obj
 *  Object to be var_dump'ed
 * @return
 *  String with the output of var_dump
 */

function lingotek_dump($obj) {
  ob_start();
  var_dump($obj);
  $string = ob_get_contents();
  ob_end_clean();
  return $string;
}

/**
 * Formats a complex object for presentation in a watchdog message.
 */
function watchdog_format_object($object) {
  return '<pre>' . htmlspecialchars(var_export($object, TRUE)) . '</pre>';
}

function lingotek_oneoff_translate($node) {
  // TODO: Run checks on this if necessary
  // (It appears this is currently unnecessary, as all nodes
  // that make it to this point are already requested for
  // translation.)
  return TRUE;
}

function lingotek_get_translatable_field_types() {
  // What types of fields DO we translate?
  $included_fields = array('text', 'text_long', 'text_textfield', 'text_textarea', 'text_textarea_with_summary', 'field_collection_embed');
  if (module_exists('link')) {
    $included_fields[] = 'link_field';
  }
  return $included_fields;
}

function lingotek_get_translatable_fields_by_content_type($entity_type, $type) {
  $all_fields = field_info_instances($entity_type, $type);
  $translatable_field_types = lingotek_get_translatable_field_types();
  $desired_fields = array();
  foreach ($all_fields as $field_name => $field_info) {
    if (in_array($field_info['widget']['type'], $translatable_field_types)) {
      $desired_fields[$field_name] = $field_name;
    }
  }
  return $desired_fields;
}

function lingotek_get_enabled_fields($entity_type, $bundle) {
  $translate = variable_get('lingotek_enabled_fields', array());
  $fields_desired = isset($translate[$entity_type][$bundle]) ? $translate[$entity_type][$bundle] : array();
  if (empty($fields_desired)) {
    //this won't work for anything more than nodes yet
    $fields_desired = lingotek_get_translatable_fields_by_content_type($bundle);
  }

  return $fields_desired;
}

/**
 * Return the xml representation of the source content for a node.
 *
 * @param object $node
 *   A Drupal node.
 *
 * @return string
 *   The XML representation of the node in Lingotek format.
 */
function lingotek_entity_xml_body($entity_type, $entity) {
  $translatable = array();

  $translate = variable_get('lingotek_enabled_fields', array());
  list($id, $vid, $bundle) = lingotek_entity_extract_ids($entity_type, $entity);
  $fields_desired = isset($translate[$entity_type][$bundle]) ? $translate[$entity_type][$bundle] : array();
  if (empty($fields_desired) && lingotek_oneoff_translate($entity)) {
    $fields_desired = lingotek_get_translatable_fields_by_content_type($entity_type, $bundle);
  }

  foreach ($fields_desired as $value) {
    $field = field_info_field($value);
    if (isset($field)) {
      array_push($translatable, $value);
    }
  }

  $content = lingotek_xml_fields($entity, $translatable, $entity->language);
  
  /* deprecated with config translation
    //Menus related to the page:
    // Do we still want this? Config translation translates these items
    $menu = menu_link_get_preferred('node/' . $entity->nid);
    $txt = $menu['link_title'];
    if ($txt != "") {
    $content = $content . "<menu_title><![CDATA[$txt]]></menu_title>\n";
    } */

  if ($entity_type == 'node') {
    //URL Alias related to the page:
    $url_alias_translation = isset($entity->lingotek['url_alias_translation']) ? $entity->lingotek['url_alias_translation'] : 0;
    if ($url_alias_translation == 1) {
      $conditions = array('source' => 'node/' . $entity->nid);
      if ($entity->language != LANGUAGE_NONE) {
        $conditions['language'] = $entity->language;
      }
      $path = path_load($conditions);
      if ($path !== FALSE) {
        $url = $path['alias'];
        $content = $content . "<url_alias><![CDATA[$url]]></url_alias>\n";
      }
    }
  }

  return "<?xml version=\"1.0\" encoding=\"UTF-8\"?><contents>$content</contents>";
}

function lingotek_xml_fields($entity, $translatable, $language) {
  $content = '';
  
  foreach ($translatable as $field) {
    $content_container = & $entity->$field;

    $field_language = is_array($content_container) && array_key_exists($language, $content_container) ? $language : LANGUAGE_NONE;

    // Deal with not being initialized right, such as pre-existing titles.
    if (!isset($content_container[$field_language])) {
      continue;
    }

    // We may split compound Drupal fields into several Lingotek fields.
    $target_keys = array(
      'value' => '', // Most text fields
      'summary' => 'summary', // "Long text with summary" fields have this sub-field value as well.
    );
    if (module_exists('link')) {
      $target_keys['link'] = array(
        'title' => '',
        'url' => '',
      );
    }

    // Create fields from all target keys.
    foreach ($target_keys as $target_key => $element_suffix) {
      $array_key = NULL;
      if (is_array($element_suffix)) {
        foreach ($element_suffix as $tarkey => $val) {
          $array_key = $tarkey;
          continue;
        }
      }
      foreach ($content_container[$field_language] as $delta) {
        if (!empty($delta[$target_key]) || (isset($array_key) && !empty($delta[$array_key]))) {
          $element_name = $field;
          if ($target_key == 'value' && isset($delta['revision_id']) && module_exists('field_collection')) {
            $current_field = '<' . $element_name . '>';
            $enabled_fields = variable_get('lingotek_enabled_fields');
            $sub_entity = lingotek_entity_load_single('field_collection_item', $delta['value']);
            $sub_fields = field_info_instances('field_collection_item', $field);
            $translatable_field_types = lingotek_get_translatable_field_types();
            $translatable_sub_fields = array();
            foreach ($sub_fields as $sub_field => $f) {
              if (in_array($f['widget']['type'], $translatable_field_types) && isset($enabled_fields['field_collection_item'][$field]) && in_array($sub_field, $enabled_fields['field_collection_item'][$field])) {
                $translatable_sub_fields[] = $sub_field;
              }
            }
            $current_field .= lingotek_xml_fields($sub_entity, $translatable_sub_fields, $field_language);
            $current_field .= '</' . $element_name . '>';
            $content .= $current_field;
            continue;
          }
          if (!is_array($element_suffix) && !empty($element_suffix)) {
            $element_name .= '__' . $element_suffix;
          }
          $current_field = '<' . $element_name . '>';
          if (is_array($element_suffix)) {
            foreach ($element_suffix as $t_key => $t_val) {
              if (isset($delta[$t_key])) {
                $delta[$t_key] = lingotek_filter_placeholders($delta[$t_key]);
                $current_field .= '<element><![CDATA[' . $delta[$t_key] . ']]></element>' . "\n";
              }
            }
          }
          else {
            $delta[$target_key] = lingotek_filter_placeholders($delta[$target_key]);
            $current_field .= '<element><![CDATA[' . $delta[$target_key] . ']]></element>' . "\n";
          }
          $current_field .= '</' . $element_name . '>';
          $content .= $current_field;
        }
      }
    }
  }

  return $content;
}

/**
 * Outputs the support footer renderable array.
 */
function lingotek_support_footer() {
  return array(
    '#type' => 'markup',
    '#markup' => theme('table', array('header' => array(), 'rows' => array(
        array(t('<strong>Support Hours:</strong><br>9am - 6pm MDT'),
          t('<strong>Phone:</strong><br> (801) 331-7777'),
          t('<strong>Email:</strong><br> <a href="mailto:support@lingotek.com">support@lingotek.com</a>')
        )),
      'attributes' => array(
        'style' => 'width:500px; margin-top: 20px;'
      )
    ))
  );
}

/**
 * Menu access callback.
 *
 * Only display Lingotek tab for node types, which have translation enabled
 * and where the current node is not language neutral (which should span
 * all languages).
 */
function lingotek_access($node, $permission) {
  if (Lingotek::isSupportedLanguage($node->language) && node_access('update', $node) && $node->lingotek['profile'] != LingotekSync::PROFILE_DISABLED) {
    return user_access($permission);
  }
  return FALSE;
}

/*
 * Top-level menu item
 */

function lingotek_access_tlmi($permission) {
  $hide_top_level_menu_item = variable_get('lingotek_hide_tlmi', 0);
  if ($hide_top_level_menu_item)
    return FALSE;
  else
    return user_access($permission);
}

function lingotek_access_by_plan_type($types, $permission = NULL) {
  $account = LingotekAccount::instance();
  $access = FALSE;
  if (is_string($types)) {
    $types = array($types);
  }
  if (!is_array($types)) {
    return $access;
  }
  foreach ($types as $type) {
    $access = TRUE;
    if (!$account->isPlanType($type)) {
      return FALSE;
    }
  }
  return is_null($permission) ? $access : user_access($permission);
}

function lingotek_access_dev_tools($node, $permission) {
  // Special case:  hide the Lingotek Developer Tools when the node is managed by entity translation and has not been pushed to Lingotek
  // OR when not showing advanced features

  $user_access = user_access($permission);
  if (($user_access && module_exists('entity_translation') && entity_translation_node_supported_type($node->type) && !lingotek_node_pushed($node)) || !LingotekAccount::instance()->showAdvanced()) {
    return FALSE;
  }
  // Default: Standard user access
  return $user_access;
}

function lingotek_enabled_bundle($entity_type, $bundle) {
  $setting = variable_get('lingotek_entity_profiles');
  return isset($setting[$entity_type][$bundle]) && $setting[$entity_type][$bundle] != LingotekSync::PROFILE_DISABLED;
}

/**
 * Returns an array of disabled bundles for a given entity_type
 *
 * @return
 *   Array of bundle key values (e.g., 'article', 'page').
 */
function lingotek_get_disabled_bundles($entity_type) {
  return lingotek_get_bundles_by_profile($entity_type, LingotekSync::PROFILE_DISABLED);
}

function lingotek_get_bundles_by_profile($entity_type, $profile) {
  $entities = variable_get('lingotek_entity_profiles');
  $profiled_bundles = array();
  if (isset($entities[$entity_type])) {
    foreach ($entities[$entity_type] as $bundle_name => $cur_profile) {
      if ((string)$profile === $cur_profile) {
        $profiled_bundles[] = $bundle_name;
      }
    }
  }
  return $profiled_bundles;
}

/**
 * Returns whether the given node type has support for translations.
 *
 * @return
 *   Boolean value.
 */
function lingotek_supported_type($type) {
  $lingotek_supported_explicitly = variable_get('language_content_type_' . $type, NULL) != '0';
  return ($lingotek_supported_explicitly && lingotek_enabled_bundle('node', $type));
}

function lingotek_supported_node($node) {
  return $node->lingotek['profile'] != LingotekSync::PROFILE_DISABLED;
}

/**
 * Returns if the node type an entity_translation managed node
 *
 * @return
 *   Boolean value.
 */
function lingotek_managed_by_entity_translation($type) {
  return module_exists('entity_translation') ? entity_translation_node_supported_type($type) : FALSE;
}

/**
 * Returns whether the given node type is an entity_translation node and has been pushed to lingotek.
 *
 * @return
 *   Boolean value.
 */
function lingotek_node_pushed($node) {
  return isset($node->lingotek['document_id']) && !empty($node->lingotek['document_id']);
}

/**
 * Returns whether the given field type has support for translations.
 *
 * @return
 *   Boolean value.
 */
function lingotek_supported_field_type($type) {
  return in_array($type, array('text_long', 'text_with_summary', 'text')); //'taxonomy_term_reference'));
}

/**
 * Returns whether caching is enabled.
 *
 * @return
 *   Boolean value.
 */
function lingotek_do_cache() {
  return !(variable_get('lingotek_flush_cache', FALSE) && user_access('use lingotek developer tools'));
}

/**
 * Gets the phase ID of the "current" phase for a translation target.
 *
 * @param array $phases
 *   An array of phase data from the result of a getTranslationTarget Lingotek API call.
 *
 * @return int
 *   The Phase ID of the current phase. Note that if all phases are marked as complete,
 *   the ID of the last phase will be returned.
 */
function lingotek_current_phase($phases) {
  $phase_id = -1;

  $current_phase = 0;

  foreach ($phases as $phase) {
    if (!$phase->isMarkedComplete) {
      $phase_id = $phase->id;
      break;
    }
    $current_phase++;
  }

  // All phases are complete, use last phase as current.
  if (!empty($phases) && $phase_id == -1) {
    $last_phase = end($phases);
    $phase_id = $last_phase->id;
  }

  return $phase_id;
}

/**
 * Return the Drupal default language (ie:  en / es / de)
 */
function lingotek_get_source_language() {
  return language_default('language');
}

// END:  lingotek_get_source_language()


/**
 * Return a whitelist of entity types supported for translation by the Lingotek module
 */
function lingotek_managed_entity_types($include_all = FALSE) {
  $type_info = entity_get_info();
  //$types = array_keys($type_info);

  $whitelist = array('node', 'comment', 'message_type', 'fieldable_panels_pane'); // supported types
  if ($include_all) {
    // include the entities that should not normally be managed separately
    if (module_exists('field_collection')) {
      $whitelist[] = 'field_collection_item';
    }
  }
  if (module_exists('taxonomy') && variable_get('lingotek_advanced_taxonomy_terms', FALSE)) {
    $whitelist[] = 'taxonomy_term';
  }
  $whitelist = array_flip($whitelist);
  $enabled_types = array_intersect_key($type_info, $whitelist);

  // labels
  if (isset($enabled_types['comment'])) {
    $enabled_types['comment']['label'] = 'Comments';
  }
  if (isset($enabled_types['node'])) {
    $enabled_types['node']['label'] = 'Nodes';
  }
  if (isset($enabled_types['message_type'])) {
    $enabled_types['message_type']['label'] = 'Message Types';
  }
  if (isset($enabled_types['field_collection_item'])) {
    $enabled_types['field_collection_item']['label'] = 'Field Collections';
  }
  if (isset($enabled_types['taxonomy_term'])) {
    $enabled_types['taxonomy_term']['label'] = 'Taxonomy Terms';
  }
  if (isset($enabled_types['fieldable_panels_pane'])) {
    $enabled_types['fieldable_panels_pane']['label'] = 'Fieldable Panels Panes';
  }

  return $enabled_types;
}

/**
 * Return content types linked to 'translatable' fields for the given entity type.
 */
function lingotek_translatable_types($entity_type) {

  $types = array();

  $entity_types = lingotek_translatable_field_details();
  if (!isset($entity_types[$entity_type])) {
    // No fields found for the given entity type.
    return $types;
  }
  foreach ($entity_types[$entity_type] as $field) {
    foreach ($field['bundles'] as $bundle) {
      $types[$bundle] = $bundle;
    }
  }
  if (count($types) > 0) {
    $types = array_keys($types);
  }
  return $types;
}

/**
 * Goes though ALL the fields in the system and gets the details about the ones that are marked 'translatable'.
 */
function lingotek_translatable_field_details() {

  $fields = field_info_fields();
  $translatable_fields = array();

  foreach ($fields as $field_id => $field) {
    foreach ($field['bundles'] as $type => $instance) {
      if (field_is_translatable($type, $field)) {
        //echo '<br>Translatable: YES!' ;
        if (!isset($field['storage']['details']['sql']['FIELD_LOAD_CURRENT'])) {
            LingotekLog::trace("field '$field_id' is marked as translatable but does not publish its details.  Skipping.");
          continue;
        }
        $field_db_table = array_keys($field['storage']['details']['sql']['FIELD_LOAD_CURRENT']);
        $field_db_table = array_shift($field_db_table);
        $translatable_fields[$type][] = array(
          'entity_type' => $type,
          'machine_name' => $field['field_name'],
          'db_table' => $field_db_table,
          'bundles' => $field['bundles'][$type],
        );
      }
    }
  }
  return $translatable_fields;
}

// --- Active and Target Language management Functions

/**
 * Get the currently active languages for this Drupal installation.
 */
function lingotek_get_active_languages($get_details = FALSE) {
  $active_languages = current(language_list('enabled'));

  if ($get_details === FALSE) {
    $active_languages = array_keys($active_languages);
  }
  /*
    Get_Details -> FALSE
    array (
    0 => 'en',
    1 => 'es',
    )

    Get_Details -> TRUE
    array (
    'en' =>
    stdClass::__set_state(array(
    'language' => 'en',
    'name' => 'English',
    'native' => 'English',
    'direction' => '0',
    'enabled' => '1',
    'plurals' => '0',
    'formula' => '',
    'domain' => '',
    'prefix' => '',
    'weight' => '0',
    'javascript' => '',
    )),
   */

  return $active_languages;
}

/**
 * Get numbered array with all node source languages
 */
function lingotek_get_node_source_languages() {
  $lingo_langs = array();
  $query = db_select('{node}', 'n')->distinct()
      ->fields('n', array('language'));
  $query = $query->execute();
  $result = $query->fetchCol(0);
  foreach ($result as $lang) {
    $lingo_lang = Lingotek::convertDrupal2Lingotek($lang);
    if ($lingo_lang) {
      $lingo_langs[$lingo_lang] = $lingo_lang;
    }
  }
  return $lingo_langs;
}

// END:  lingotek_get_active_languages()

/**
 * Flags a target language as active:FALSE in the Target Language tracking.
 */
function lingotek_delete_target_language($lingotek_locale) {
  $result = FALSE;

  if (is_string($lingotek_locale) && strlen($lingotek_locale)) {
    db_update('{languages}')
        ->fields(array(
          //'enabled' => 0,
          'lingotek_enabled' => 0,
        ))
        ->condition('lingotek_locale', $lingotek_locale) //->condition('language',  $drupal_language_code)
        ->execute();
    LingotekLog::info("Target language removed: @lingotek_locale"
        , array(
      '@lingotek_locale' => $lingotek_locale)
    );

    // Remove the Target Language from the Lingotek Project.
    $project_id = variable_get('lingotek_project', '');
    $api = LingotekApi::instance();
    $result = $api->removeTranslationTarget(NULL, $project_id, $lingotek_locale);
  }

  return $result;
}

/**
 * Sets the extended target language locale in the languages table and whether or not it is enabled
 * 
 * @param $drupal_language_code
 * @param $lingotek_enable whether or not to enable the language on TMS (default: 1)
 * @param $lingotek_locale the lingotek locale that the drupal code should be associated with (it will try to pick the right one when not passed in)
 * @param $api_add boolean whether or not to add the language to all documents in the project (default: TRUE)
 * 
 */
function lingotek_set_target_language($drupal_language_code, $lingotek_enable = 1, $lingotek_locale = NULL, $api_add = TRUE) {
  $result = FALSE;
  $one_success = FALSE;  //tracks if any result has been true
  $lingotek_locale = is_null($lingotek_locale) ? Lingotek::convertDrupal2Lingotek($drupal_language_code, FALSE) : $lingotek_locale;

  if (is_string($drupal_language_code) && strlen($drupal_language_code) && $lingotek_enable && $lingotek_locale && $api_add) {
    $api = LingotekApi::instance();
    $projects = LingotekSync::getSyncProjects();
    foreach ($projects as $project_id) {
      $result = $api->addTranslationTarget(NULL, $project_id, $lingotek_locale);
      if ($result) {
        LingotekSync::insertTargetEntriesForAllEntities($lingotek_locale);
      }
      $one_success = $one_success || $result;
    }

    if ($one_success == TRUE) {
      db_update('{languages}')
          ->fields(array(
            'enabled' => 1,
            'lingotek_enabled' => $lingotek_enable ? 1 : 0,
            'lingotek_locale' => $lingotek_locale
          ))
          ->condition('language', $drupal_language_code)
          ->execute();
      drupal_static_reset('language_list');
      LingotekLog::info("Target language added: @drupal_language_code (@lingotek_locale)", array(
        '@drupal_language_code' => $drupal_language_code,
        '@lingotek_locale' => $lingotek_locale));
    }
    else {
      drupal_set_message(t($lingotek_locale . ' could not be added as a language for Lingotek to translate.'), 'error', FALSE);
      LingotekLog::error("Target language could not be added: @drupal_language_code (@lingotek_locale)", array(
        '@drupal_language_code' => $drupal_language_code,
        '@lingotek_locale' => $lingotek_locale));
    }
  }

  return $result;
}

function lingotek_lookup_language_by_locale($lingotek_locale) {
  $languages = language_list();
  foreach ($languages as $language) {
    if (isset($language->lingotek_locale) && strcmp($language->lingotek_locale, $lingotek_locale) == 0) {
      return $language;
    }
  }
  return FALSE;
}

function lingotek_lookup_locale_exists($drupal_language_code) {
  $languages = language_list();
  foreach ($languages as $target) {
    if (isset($target->language) && strcmp($target->language, $drupal_language_code) == 0) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Adds the target language as being enabled.
 */
function lingotek_add_target_language($lingotek_locale, $call_api = TRUE) {
  if (is_null($lingotek_locale)) {
    return FALSE;
  }
  lingotek_add_missing_locales(FALSE); // fills in any missing lingotek_locale values to the languages table
  $language = lingotek_lookup_language_by_locale($lingotek_locale);

  if ($language) {
    // ALREADY EXISTS IN LANGUAGE TABLE
    // If already in the languages table then just tack on the lingotek_locale and enable it
    $drupal_language_code = $language->language;
  }
  else {
    // DOES NOT EXIST, INSERT NEW INTO LANGUAGE TABLE
    // If not add it to the languages table first and then tack on the lingotek_locale and enable it
    $drupal_language_code = Lingotek::convertLingotek2Drupal($lingotek_locale, FALSE);
    if (lingotek_lookup_locale_exists($drupal_language_code)) { // drupal code is already being used, generate another
      $errors = array($drupal_language_code);
      $drupal_language_code = strtolower(str_replace("_", "-", $lingotek_locale));
      if (lingotek_lookup_locale_exists($drupal_language_code)) {
        $errors[] = $drupal_language_code;
        LingotekLog::error("Cannot add language code.  Attempted language codes already being used: !errors", array('!errors' => $errors));
        return FALSE; // do not add the language.
      }
    }

    $name = isset($_POST['language']) ? $_POST['language'] : NULL;
    $native = isset($_POST['native']) ? $_POST['native'] : NULL;
    $direction = isset($_POST['direction']) && (strcasecmp('RTL', $_POST['direction']) == 0 ) ? LANGUAGE_RTL : LANGUAGE_LTR;
    $domain = '';
    $prefix = '';
    locale_add_language($drupal_language_code, $name, $native, $direction, $domain, $prefix); // Function from the Locale module.
  }

  return lingotek_set_target_language($drupal_language_code, 1, $lingotek_locale, $call_api);
}

/**
 * Fills in any missing lingotek_locale values to the languages table
 */
function lingotek_add_missing_locales($show_message = TRUE) {
  LingotekLog::trace(__METHOD__);
  $languages = language_list();
  $default_language = language_default();
  $update_static_language_list = FALSE;
  $added_locales = array();
  foreach ($languages as $target) {
    if (isset($target->lingotek_locale) && !strlen($target->lingotek_locale)) {
      $drupal_language_code = $target->language;
      $lingotek_locale = Lingotek::convertDrupal2Lingotek($drupal_language_code, FALSE);
      lingotek_enable_language_by_code($drupal_language_code, $lingotek_locale);
      $update_static_language_list = TRUE;
      $added_locales[] = $lingotek_locale;
    }
  }
  if ($update_static_language_list) {
    drupal_static_reset('language_list');
  }
  if (!empty($added_locales)) {
    drupal_set_message(t('Added the following locales to the languages table: @added.', array('@added' => implode(', ', $added_locales))));
  }
  else {
    if ($show_message) {
      drupal_set_message(t('All locales already set in the languages table.'));
    }
  }
}

/**
 * Add lingotek_locale and lingotek_enable to the language table for the passed in drupal_language_code 
 */
function lingotek_enable_language_by_code($drupal_language_code, $lingotek_locale = NULL) {
  $field_data = array(
    'enabled' => 1,
    //'lingotek_enabled' => $lingotek_enabled ? 1 : 0,
  );
  if (!is_null($lingotek_locale)) {
    $field_data['lingotek_locale'] = $lingotek_locale;
  }
  db_update('{languages}')
      ->fields($field_data)
      ->condition('language', $drupal_language_code)
      ->execute();
  return $lingotek_locale;
}

/**
 * Get only the languages that are enabled
 */
function lingotek_get_target_locales($codes_only = TRUE) {
  $target_languages = db_query("SELECT * FROM {languages} WHERE lingotek_enabled = :enabled", array(':enabled' => 1))->fetchAll();
  $target_codes = array();
  foreach ($target_languages as $target_language) {
    $target_codes[] = $target_language->lingotek_locale;
  }
  return $codes_only ? $target_codes : $target_languages;
}

/*
 * Get the Lingotek Content Types - returns the content types OR ids only
 *
 * @param $ids_only
 *  Boolean - return ids only rather than associative array of content type definitions
 * @return
 *  Mixed - associative array (or an array of keys when ids_only is TRUE)
 */

function lingotek_get_content_types() {
  $fields = variable_get('lingotek_enabled_fields');
  return array_keys($fields['node']);
}

/*
 * Custom unix timestamp formatter
 *
 * @param int $unix_timestamp
 *    Unix timestamp to format
 *
 * @param bool $as_array
 *    if true, return string formatted like '[number] [time interval]'.  For example, '22 hours'.
 *    if false, returns number and time interval as an array where the array is ('number' => [number], 'interval' => [time interval]).
 */

function lingotek_human_readable_timestamp($unix_timestamp, $as_array = FALSE) {
  $time = time() - $unix_timestamp;

  $intervals = array(
    31536000 => t('year'),
    2592000 => t('month'),
    604800 => t('week'),
    86400 => t('day'),
    3600 => t('hour'),
    60 => t('minute'),
    1 => t('second')
  );

  foreach ($intervals as $unit => $text) {
    if ($time < $unit) {
      continue;
    }
    $number = floor($time / $unit);
    if ($as_array) {
      return array('number' => $number, 'interval' => $text . (($number > 1) ? 's' : ''));
    }
    else {
      return $number . ' ' . $text . (($number > 1) ? 's' : '');
    }
  }
  if ($as_array) {
    return array('number' => 0, 'interval' => t('seconds'));
  }
  else {
    return '0 ' . t('seconds');
  }
}

/**
 * Returns the site name, or base url if that isn't set.
 * */
function lingotek_get_site_name() {
  $site_name = variable_get('site_name', NULL);
  if (empty($site_name)) {
    global $base_root;
    $site_url = parse_url($base_root);
    $site_name = $site_url['host'];
  }
  return $site_name;
}

function lingotek_get_module_info($field = NULL) {
  $result = db_query("SELECT info FROM {system} WHERE type = :type AND name = :name", array(':type' => 'module', ':name' => 'lingotek'))->fetchObject();
  $info = unserialize(current($result));
  return is_null($field) ? $info : (isset($info[$field]) ? $info[$field] : NULL);
}

function lingotek_cleanup_field_languages($entity_type) {
  $cleanup_batch = lingotek_field_language_data_cleanup_batch_create($entity_type, FALSE);
  batch_set($cleanup_batch);
}

function lingotek_cleanup_field_languages_for_nodes() {
  lingotek_cleanup_field_languages('node');
}

function lingotek_cleanup_field_languages_for_comments() {
  lingotek_cleanup_field_languages('comment');
}

function lingotek_cleanup_field_languages_for_fieldable_panels_panes() {
  lingotek_cleanup_field_languages('fieldable_panels_pane');
}

function lingotek_cleanup_field_languages_for_taxonomy_terms() {
  lingotek_cleanup_field_languages('taxonomy_term');
}

/**
 * Report all Lingotek translations to the Entity Translation module
 */
function lingotek_cleanup_notify_entity_translation() {
  $entity_list = array();
  $results = db_select('{lingotek_entity_metadata}', 'l')
      ->fields('l', array('entity_id', 'entity_type', 'entity_key', 'created', 'modified'))
    ->condition('entity_key', 'target_sync_status_%', 'LIKE')
    ->condition('value', LingotekSync::STATUS_CURRENT)
    ->execute();
  $total_updates = 0;
  foreach ($results as $r) {
    $locale = str_replace('target_sync_status_', '', $r->entity_key);
    $entity = lingotek_entity_load_single($r->entity_type, $r->entity_id);
    if ($entity) {
      $info = entity_get_info($r->entity_type);
      $bundle_name = !empty($info['entity keys']['bundle']) ? $info['entity keys']['bundle'] : NULL;
      if ($bundle_name && lingotek_managed_by_entity_translation($entity->$bundle_name)) {
        $addtl_params = array('created' => $r->created, 'changed' => $r->modified);
        list($languages_updated, $updates) = lingotek_entity_translation_save_status($r->entity_type, $entity, array(Lingotek::convertLingotek2Drupal($locale)), $addtl_params);
        $total_updates += $updates;
      }
    }
  }
  if ($total_updates > 0) {
    $translation_str = format_plural($total_updates, t('translation was'), t('translations were'));
    drupal_set_message(t('@num_updates @translations reported to the Entity Translation module.', array('@num_updates' => $total_updates, '@translations' => $translation_str)));
  }
  else {
    drupal_set_message(t('The Entity Translation module was already aware of all current translations.'));
  }
}

/**
 * lingotek_refresh_api_cache utility
 */
function lingotek_refresh_api_cache($show_messages = TRUE) {
  LingotekLog::trace(__METHOD__);

  $api = LingotekApi::instance();

  // Call methods with $reset = TRUE to update our locally cached values.
  $api->listProjects(TRUE);
  $api->listWorkflows(TRUE);
  $api->listVaults(TRUE);

  if ($show_messages) {
    drupal_set_message(t('Project, workflow, and vault information has been refreshed.'));
  }
}

/**
 * Sets the priority of the Lingotek Translation module
 */
function lingotek_set_priority() {
  db_update('{system}')->fields(array('weight' => 12))->condition('name', 'lingotek')->execute();
}

/**
 * Gets the default profile info, mapped by entity type
 */
function lingotek_load_profile_defaults($entity_type) {
  $profile_defaults = lingotek_get_profiles();
  $entity_profile_defaults = variable_get('lingotek_entity_profiles', array());
  $profile_map = array();
  if (array_key_exists($entity_type, $entity_profile_defaults)) {
    foreach ($entity_profile_defaults[$entity_type] as $k => $v) {
      $profile_map[$k] = is_numeric($v) ? $profile_defaults[$v] : array();
      $profile_map[$k]['profile'] = $v;
      unset($profile_map[$k]['name']);
    }
    return $profile_map;
  }
  return array();
}

/**
 * Sets the global defaults for the Lingotek Translation module
 */
function lingotek_set_defaults() {
  LingotekLog::trace(__METHOD__);
  $defaults = array(
    'lingotek_advanced_parsing' => TRUE
  );

  // Check if vars are set.  If so, use what is already set.  If not, set them using the defaults provided above.
  foreach ($defaults as $k => $default_value) {
    variable_set($k, variable_get($k, $default_value));
  }

  lingotek_set_default_advanced_xml();
}

/**
 * Migration 1
 */
function lingotek_migration_1() {
  $spec = array(
    'type' => 'int',
    'description' => "Lingotek enabled",
    'not null' => TRUE,
    'default' => 0
  );
  try {
    db_add_field('languages', 'lingotek_enabled', $spec);
  } catch (DatabaseSchemaObjectExistsException $e) {
    // already exists (no need to do anything)
  }

  // store lingotek enabled fields in languages table rather than variable table
  $target_languages = array_filter(explode('|', variable_get('lingotek_target_languages', '')));
  foreach ($target_languages as $drupal_language_code) {
    lingotek_enable_language_by_code($drupal_language_code);
  }
  variable_del('lingotek_target_languages');
  drupal_static_reset('language_list');
}

/**
 * Migration 2
 */
function lingotek_migration_2() {
  $spec = array(
    'type' => 'varchar',
    'description' => "Locale mapping",
    'length' => 10,
    'not null' => TRUE,
    'default' => ''
  );
  try {
    db_add_field('languages', 'lingotek_locale', $spec);
  } catch (DatabaseSchemaObjectExistsException $e) {
    // already exists (no need to do anything)
  }
  drupal_static_reset('language_list');
}

/**
 * Migration 3 - Upgrade lingotek table entries from drupal_codes to lingotek_locales (whenever applicable) 
 */
function lingotek_migration_3() {
  $ret = array();
  $field_name_prefix = 'target_sync_status_';
  $result = db_query("SELECT lingokey, COUNT(*) as num FROM {lingotek} WHERE lingokey LIKE :pattern GROUP BY lingokey"
      , array(':pattern' => db_like($field_name_prefix) . '%'));

  $total_affected_rows = 0;
  foreach ($result as $record) {
    $old_key = $record->lingokey;
    $code = @end(explode("_", $old_key));
    $lingotek_locale = Lingotek::convertDrupal2Lingotek($code, FALSE); //will return FALSE when lingotek_locales are passed in (so, they'll be skipped)
    if ($lingotek_locale) {
      $new_key = $field_name_prefix . $lingotek_locale;
      $query = db_update('{lingotek}', $ret)
          ->fields(array('lingokey' => $new_key))
          ->condition('lingokey', $old_key, '=');
      try {
        $affected_rows = $query->execute();
      } catch (PDOException $e) {
        // skip these:  manually delete for later rows that key violation constraint issues (if it already exists it need not succeed)
        $affected_rows = 0;
      }
      $total_affected_rows += $affected_rows;
    }
  }
  $ret['total_affected_rows'] = $total_affected_rows;
  //drupal_set_message("fields updated");
  return $ret;
}

function lingotek_profile_condition($entity_type, $table_alias, $table_profile, $profile_id) {
  $or = db_or();
  
  $fields = variable_get('lingotek_enabled_fields');
  $types = array();
  foreach (lingotek_load_profile_defaults($entity_type) as $content_type => $profile) {
    if ($profile['profile'] == (string)$profile_id && isset($fields[$entity_type][$content_type])) {
      $types[] = $content_type;
    }
  }
  
  if (!empty($types)) {
    $an = db_and();
    $an->condition($table_alias . '.type', $types, 'IN');
    $an->condition($table_profile . '.value', NULL);
    $or->condition($an);
  }
  $or->condition($table_profile . '.value', $profile_id);
  
  return $or;
}

/**
 * Save a translation to the Entity Translation table with status and additional parameters
 */
function lingotek_entity_translation_save_status($entity_type, $entity, $language_codes, $addtl_params = NULL) {

  global $user;
  $languages_updated = array();
  $updates = 0;
  if (!entity_translation_enabled($entity_type)) {
    return array($languages_updated, $updates);
  }
  if ($entity_type == 'node' && !entity_translation_node_supported_type($entity->type)) {
    return array($languages_updated, $updates);
  }

  $handler = entity_translation_get_handler($entity_type, $entity);
  $source_langcode = lingotek_entity_langcode($entity_type, $entity);

  $translations = $handler->getTranslations();
  if (empty($translations->original)) {
    $handler->initTranslations();
    $handler->saveTranslations();
  }

  foreach ($language_codes as $langcode) {
    if ($langcode !== 0) {
      $et_params = array(
        'language' => $langcode,
        'translate' => isset($entity->translate) ? $entity->translate : 0,
        'uid' => !empty($entity->uid) ? $entity->uid : $user->uid,
        'changed' => !empty($addtl_params['changed']) ? $addtl_params['changed'] : time(),
      );
      if (!empty($addtl_params['created'])) {
        $et_params['created'] = $addtl_params['created'];
      }
      if ($langcode != $source_langcode) {
        $et_params['source'] = $source_langcode;
      }
      if (isset($addtl_params['status_request'])) {
        $et_params['status'] = $addtl_params['status_request'];
      }
      else {
        // Check for a current status for the given langcode.
        if (isset($entity->translations->data[$langcode]['status'])) {
          $et_params['status'] = $entity->translations->data[$langcode]['status'];
        }
        // If the current langcode doesn't have status, get from parent entity.
        elseif (isset($entity->translations->data[$entity->language])) {
          $et_params['status'] = $entity->translations->data[$entity->language]['status'];
        }
        else {
          // Set to published as a default.
          $et_params['status'] = 1; // published
        }
      }
      $handler->setTranslation($et_params);
      $languages_updated[$langcode] = lingotek_language_name($langcode);
      $updates++;
    }
  }
  $handler->saveTranslations();
  return array($languages_updated, $updates);
}

function lingotek_workbench_icon($entity_type, $id, $lingotek_locale, $tooltip = 'Edit this translation') {
  drupal_add_css(drupal_get_path('module', 'lingotek') . '/style/workbench-link.css', array('group' => CSS_DEFAULT));
  
  $text = '<img src="' . base_path() . drupal_get_path('module', 'lingotek') . '/images/ico_chevs_dd.png" >';
  $url = 'lingotek/workbench/' . $entity_type . '/' . $id . '/' . $lingotek_locale;
  return l($text, $url, array('attributes' => array('class' => array('lingotek-translation-link'), 'target' => '_blank', 'title' => $tooltip), 'html' => TRUE));
}

function lingotek_list_entities_with_language($entity_type, $bundle, $language) {
  $ids = array();
  $fields = field_info_instances($entity_type, $bundle);
  foreach ($fields as $name => $field_info) {
    $ids = $ids + lingotek_list_entities_with_field_in_language_by_bundle($entity_type, $bundle, $name, $language);
  }

  return $ids;
}

function lingotek_list_nodes_translated_in_language($language, $bundle = NULL) {
  $query = db_select('{node}')
      ->fields('node', array('tnid'));
  if (!is_null($bundle)) {
    $query->condition('node.type', $bundle);
  }
  $query->condition('node.tnid', 0, '<>')
      ->condition('language', $language)
      ->where('node.nid != node.tnid');
  $nodes = $query->execute()->fetchCol();

  return $nodes;
}

function lingotek_list_entities_with_field_in_language_by_bundle($entity_type, $bundle, $field, $language, $exclude_source = TRUE) {
  $query = new EntityFieldQuery;
  $query->entityCondition('entity_type', $entity_type);
  if (!is_null($bundle)) {
    $query->entityCondition('bundle', $bundle);
  }
  else {
    // Include only the enabled bundle types
    $disabled_bundles = lingotek_get_disabled_bundles($entity_type);
    if (!empty($disabled_bundles)) {
      $query->entityCondition('bundle', $disabled_bundles, 'NOT IN');
    }
  }
  if ($exclude_source) {
    $query->propertyCondition('language', $language, "!=");
  }
  $query->entityCondition('deleted', 0)
      ->fieldLanguageCondition($field, $language);
  $entities = $query->execute();

  return isset($entities['node']) ? array_keys($entities['node']) : array();
}

function lingotek_list_entities_with_field_in_language($entity_type, $field, $language, $exclude_source = TRUE) {
  return lingotek_list_entities_with_field_in_language_by_bundle($entity_type, NULL, $field, $language, $exclude_source);
}

/**
 * Gets a list of fields with translation enabled.
 *
 * @return array
 *   An array of the machine names for translatable fields in the system.
 */
function lingotek_translatable_fields() {
  $fields = field_info_fields();
  $translatable_fields = array();
  foreach ($fields as $field_id => $field) {
    foreach ($field['bundles'] as $type => $instance) {
      if (field_is_translatable($type, $field)) {
        $translatable_fields[] = $field['field_name'];
      }
    }
  }

  return $translatable_fields;
}

/**
 * Wrap placeholder tags in specific tags that will be ignored by Lingotek
 *
 * @param $segment_text
 *   a string containing the segment to be translated
 *
 * @return string
 *   the original input plus any additional reference tags to be ignored.
 */
function lingotek_filter_placeholders($segment_text, $protect_vars = FALSE) {
  // NOTE: This regex is only a generalization of the variable names possible using
  // the t-function's variable interpolation.  This finds all sets of word
  // characters (A-Z, a-z, - or _) that begin with either !, @, or %, that do not
  // fall between angle brackets (which would indicate being on the inside
  // of an HTML tag).  It also protects everything inside square brackets
  // that do not fall inside angle brackets.
  $patterns = array(
    '/(\[[!@%\w:=\/\s_-]+\]\s*)(?![^<]*\>)/', // wrap everything in square brackets
  );
  if ($protect_vars) {
    $patterns[] = '/([!@%][\w_-]+\s*)(?![^<]*\>)/'; // wrap everything beginning with !,@,%
  }
  $replacement = '<drupalvar>${1}</drupalvar>';
  foreach ($patterns as $p) {
    $segment_text = preg_replace($p, $replacement, $segment_text);
  }
  return $segment_text;
}

/**
 * Unwrap placeholder tags in specific tags that will be ignored by Lingotek
 *
 * @param $segment_text
 *   a string containing the segment that potentially contains drupal variables
 *
 * @return string
 *   the original input less any additional reference tags added prior to upload
 */
function lingotek_unfilter_placeholders($segment_text) {
  $pattern = '/<\/?drupalvar>/';
  $replacement = '';
  return preg_replace($pattern, $replacement, $segment_text);
}

/**
 * Return redirect-path information by entity type for Lingotek-supported entities
 *
 * @param $entity_type
 *   a string containing the name of the given entity type
 *
 * @param $next
 *   whether to redirect to the next setup destination or just return the current one
 */
function lingotek_get_entity_setup_path($entity_type, $next=FALSE) {
  $node_next = 'admin/config/lingotek/comment-translation-settings';
  if (!module_exists('comment')) {
    $node_next = 'admin/config/lingotek/additional-translation-settings';
  }
  $entities = array(
    'node' => array('current' => 'admin/config/lingotek/node-translation-settings',
      'next' => $node_next),
    'comment' => array('current' => 'admin/config/lingotek/comment-translation-settings',
      'next' => 'admin/config/lingotek/additional-translation-settings'),
    'config' => array('current' => 'admin/config/lingotek/additional-translation-settings',
      'next' => 'admin/settings/lingotek'),
  );
  if (isset($entities[$entity_type])) {
    if ($next) {
      return $entities[$entity_type]['next'];
    }
    else {
      return $entities[$entity_type]['current'];
    }
  }
  return NULL;
}

/**
 * Return the number of entities
 *
 * @param $entity_type
 *   a string containing the name of the given entity type
 */
function lingotek_get_entity_count($entity_type) {
  $query = db_select('{' . $entity_type . '}', 'n')
      ->fields('n')
    ->execute();
  $num_results = $query->rowCount();
  return $num_results;
}

/*
 * Return an associative array of entity types, with an array of bundles
 * enabled for the given profile.
 */
function lingotek_get_bundles_by_profile_id($profile_id) {
  $bundles = array();
  if (is_array($profile_id)) { // allow profile_ids to be an array of profiles to find bundles for
    $mapped_results = array_map('lingotek_get_bundles_by_profile_id', $profile_id);
    foreach ($mapped_results as $result) {
      $bundles = array_merge_recursive($result, $bundles);
    }
    return $bundles;
  }
  $entities = entity_get_info();
  $lentities = variable_get('lingotek_entity_profiles');
  foreach ($entities as $entity_name => $entity) {
    if (!isset($lentities[$entity_name])) {
      unset($entities[$entity_name]);
    }
    foreach ($entity['bundles'] as $bundle_name => $bundle) {
      if (isset($lentities[$entity_name][$bundle_name]) && $lentities[$entity_name][$bundle_name] === (string)$profile_id) {
        if (!isset($bundles[$entity_name])) {
          $bundles[$entity_name] = array();
        }
        $bundles[$entity_name][$bundle_name] = TRUE;
      }
    }
  }
  return $bundles;
}

/*
 * Return an array tree of entity_types -> entity_ids -> document_ids
 */
function lingotek_get_document_id_tree() {
  $query = db_select('{lingotek_entity_metadata}', 'lem')
      ->fields('lem', array('entity_id', 'entity_type', 'value'))
      ->condition('lem.entity_key', 'document_id')
      ->execute();
  $results = $query->fetchAll();
  $doc_id_tree = array();
  foreach ($results as $r) {
    if (!isset($doc_id_tree[$r->entity_type])) {
      $doc_id_tree[$r->entity_type] = array();
    }
    $doc_id_tree[$r->entity_type][$r->entity_id] = $r->value;    
  }
  return $doc_id_tree;
}

function lingotek_get_all_entities_by_profile($profile_id) {
  // gather all bundles for searching, as some entities may be one-offs
  // even though Lingotek is not enabled for the entire bundle.
  $all_profiles = lingotek_get_profiles();
  $all_profiles[LingotekSync::PROFILE_CUSTOM] = TRUE;
  $all_profiles[LingotekSync::PROFILE_DISABLED] = TRUE;
  $all_bundles = lingotek_get_bundles_by_profile_id(array_keys($all_profiles));
  $all_entities = array();
  // aggregate all entity-type-specific results into a single numbered array
  foreach (array_keys($all_bundles) as $entity_type) {
    $entities = lingotek_get_entities_by_profile_and_entity_type($profile_id, $entity_type);
    foreach ($entities as $e) {
      $all_entities[] = $e;
    }
  }
  return $all_entities;
}

/*
 * Get all entities that are enabled for lingotek translation, by profile and entity_type
 * 
 * Return an array of entity-specific arrays containing document_id, type, and id
 */
function lingotek_get_entities_by_profile_and_entity_type($profile_id, $entity_type) {
  // get all bundles that belong to the given profile
  $all_bundles = lingotek_get_bundles_by_profile_id($profile_id); // this function accepts an id or an array of ids
  $bundles = array();
  $entities = array();
  
  if (isset($all_bundles[$entity_type])) {
    $bundles = array($entity_type => $all_bundles[$entity_type]);
  }
  
  // get all entities that belond to those bundles
  foreach ($bundles as $entity_type => $entity_bundles) {
    if ($entity_type == 'comment') {
      $ref_tables = array();
      foreach (array_keys($entity_bundles) as $key) {
        $tmp_array = explode('_', $key);
        $key = implode('_', array_slice($tmp_array, 2));
        $ref_tables[] = $key;
      }
      $query = db_select('{' . $entity_type . '}', 'e')
          ->fields('e', array('cid'));
      $query->join('{node}', 'n', "n.nid = e.nid AND n.type IN ('" . implode("','", $ref_tables) . "')");
      $results = $query->execute()->fetchCol();
      foreach ($results as $id) {
        $entities[] = array('id' => $id, 'type' => $entity_type);
      }
    }
    else {
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', $entity_type)
          ->entityCondition('bundle', array_keys($entity_bundles), 'IN');
      $result = $query->execute();
      unset($query);
      if (isset($result[$entity_type])) {
        foreach ($result[$entity_type] as $id => $entity_data) {
          $entities[] = array('id' => $id, 'type' => $entity_type);
        }
      }
    }
    // END OPTIMIZED WAY
  }
  
  // subtract all entities specifically *not* set to the given profile
  $query = db_select('{lingotek_entity_metadata}', 'lem')
      ->fields('lem', array('entity_id', 'entity_type'))
      ->condition('lem.entity_key', 'profile')
      ->condition('lem.value', $profile_id, is_array($profile_id) ? 'NOT IN' : '!=')
      ->condition('lem.entity_type', $entity_type);
  $result = $query->execute();
  $subtract_entity_ids = $result->fetchAll();

  $doc_ids = lingotek_get_document_id_tree();
  $subtractions = array();
  foreach ($subtract_entity_ids as $sei) {
    if (!isset($subtractions[$sei->entity_type])) {
      $subtractions[$sei->entity_type] = array();
    }
    $subtractions[$sei->entity_type][$sei->entity_id] = TRUE;
  }
  $filtered_entities = array();
  foreach ($entities as $e) {
    if (!isset($subtractions[$e['type']][$e['id']])) {
      if (isset($doc_ids[$e['type']][$e['id']])) {
        $e['document_id'] = $doc_ids[$e['type']][$e['id']];
      }
      $filtered_entities[$e['id']] = $e;
    }
  }

  // add all entities specifically set to the given profile
  $query = db_select('{lingotek_entity_metadata}', 'lem')
      ->fields('lem', array('entity_id', 'entity_type'))
      ->condition('lem.entity_key', 'profile')
      ->condition('lem.value', $profile_id,  is_array($profile_id) ? 'IN' : '=');
  if ($entity_type != 'all') {
    $query->condition('lem.entity_type', $entity_type);
  }
  $result = $query->execute();
  $add_entity_ids = $result->fetchAll();
  foreach ($add_entity_ids as $aei) {
    $addition = array('id' => $aei->entity_id, 'type' => $aei->entity_type);
    if (isset($doc_ids[$aei->entity_type][$aei->entity_id])) {
      $addition['document_id'] = $doc_ids[$aei->entity_type][$aei->entity_id];
    }
    $filtered_entities[$aei->entity_id] = $addition;
  }
  
  return $filtered_entities;
}

/**
 * Clean up field-collection fields
 */
function lingotek_cleanup_field_collection_fields() {
  // todo: This function is MySQL-specific.  Needs refactoring to be db-agnostic.

  $field_collection_field_types = field_read_fields(array('type' => 'field_collection'));
  $disabled_types = lingotek_get_disabled_bundles('field_collection_item');

  // Update all lines in field-collection field tables to be undefined,
  // ignoring duplicates, then delete all lines in field-collection
  // field tables that aren't undefined.

  $fc_fields_found = 0;
  $fc_dups_found = 0;
  foreach (array_keys($field_collection_field_types) as $fc_field_name) {
    $fc_langs = lingotek_cleanup_get_dirty_field_collection_fields("field_data_" . $fc_field_name);
    if (!$fc_langs) {
      // No language-specific field-collection fields exist in this table
      continue;
    }
    if (in_array($fc_field_name, $disabled_types)) {
      // Lingotek does not manage this field-collection type.  Abort.
      continue;
    }
    // Make sure one of each field has an undefined entry.
    // We do this by first adding the count of affected fields, and then
    // removing the remaining ones (duplicates) below, to avoid double counting
    // fields that were actually changed.  (Some would already have language-
    // neutral and some would not.)
    foreach ($fc_langs as $fc_lang) {
      $fc_fields_found += (int) $fc_lang;
    }
    db_query("UPDATE IGNORE {field_data_" . $fc_field_name . "} SET language = '" . LANGUAGE_NONE . "'");

    // Find all unnecessary fields (language-specific) for removal.
    $fc_langs = lingotek_cleanup_get_dirty_field_collection_fields("field_data_" . $fc_field_name);
    // Subtract the ones that were not removed from the update above.
    foreach ($fc_langs as $dups) {
      $fc_dups_found += (int) $dups;
      $fc_fields_found -= (int) $dups;
    }
    db_query("DELETE FROM {field_data_" . $fc_field_name . "} WHERE language <> '" . LANGUAGE_NONE . "'");
    db_query("UPDATE IGNORE {field_revision_" . $fc_field_name . "} SET language = '" . LANGUAGE_NONE . "'");
    db_query("DELETE FROM {field_revision_" . $fc_field_name . "} WHERE language <> '" . LANGUAGE_NONE . "'");
  }
  if ($fc_fields_found) {
    drupal_set_message(format_plural($fc_fields_found, t('Set 1 field-collection entry to be language neutral'), t('Set @ff field-collection entries to be language neutral.', array('@ff' => $fc_fields_found))));
  }
  if ($fc_dups_found) {
    drupal_set_message(format_plural($fc_dups_found, t('Removed 1 unnecessary language-specific field-collection entry'), t('Removed @ff unnecessary language-specific field-collection entries.', array('@ff' => $fc_dups_found))));
  }
  if (!$fc_fields_found && !$fc_dups_found) {
    drupal_set_message(t('All field-collection entries were already correctly set to be language neutral.'));
  }
}

function lingotek_cleanup_get_dirty_field_collection_fields($field_collection_table) {
  $query = "SELECT COUNT(entity_id) FROM {" . $field_collection_table . "} WHERE language != '" . LANGUAGE_NONE . "' GROUP BY entity_type, bundle, entity_id, delta";
  $result = db_query($query)->fetchCol();
  $empty_array = array();
  return ($result && is_array($result) ? $result : $empty_array);
}

/**
 * Set all message types with empty or undefined languages to be the site's default language
 */
function lingotek_cleanup_message_types() {
  $default_lang = lingotek_get_source_language();
  $def_lang_name = language_default('name');
  $message_types = message_type_load();
  $disabled_types = lingotek_get_disabled_bundles('message_type');
  $missing_lang = 0;
  $und_lang = 0;
  foreach ($message_types as $mt) {
    if (in_array($mt->category, $disabled_types)) {
      // Lingotek does not manage this message type category.  Abort.
      continue;
    }
    if (!($mt->language) || $mt->language == LANGUAGE_NONE) {
      if (!($mt->language)) {
        $missing_lang++;
      }
      elseif ($mt->language == LANGUAGE_NONE) {
        $und_lang++;
      }
      $mte = lingotek_entity_load_single('message_type', $mt->id);
      $mte->language = $default_lang;
      message_type_save($mte);
    }
  }
  if ($missing_lang) {
    drupal_set_message(format_plural($missing_lang, t('Set 1 message type with missing language to @lang.', array('@lang' => $def_lang_name)), t('Set @count message types with missing languages to @lang.', array('@count' => $missing_lang, '@lang' => $def_lang_name))));
  }
  if ($und_lang) {
    drupal_set_message(format_plural($und_lang, t('Set 1 language-neutral message type to @lang.', array('@lang' => $def_lang_name)), t('Set @count language-neutral message types to @lang.', array('@count' => $und_lang, '@lang' => $def_lang_name))));
  }
  if (!$missing_lang && !$und_lang) {
    drupal_set_message(t('All message types were already correctly set to a specific language.'));
  }
}

function lingotek_has_unconventional_fc_translations() {
  $field_collection_field_types = field_read_fields(array('type' => 'field_collection'));
  $query_join_items = array(
    'fca.entity_type = fcb.entity_type',
    'fca.bundle = fcb.bundle',
    'fca.deleted = fcb.deleted',
    'fca.entity_id = fcb.entity_id',
    'fca.revision_id = fcb.revision_id',
    'fca.delta = fcb.delta',
    'fca.language != fcb.language',
  );
  foreach (array_keys($field_collection_field_types) as $fc_field_name) {
    $query = db_select('{field_data_' . $fc_field_name . '}', 'fca');
    $query->join('{field_data_' . $fc_field_name . '}', 'fcb', implode(' AND ', $query_join_items) . ' AND fca.' . $fc_field_name . '_value != fcb.' . $fc_field_name . '_value');
    $query->groupBy('fca.entity_id');
    $query->fields('fca', array('entity_id'));
    $result = $query->execute();
    if ($result && $unconventional_fc = $result->fetchAll()) {
      return $unconventional_fc;
    }
    return FALSE;
  }
}

/**
 * Batch Create: Lingotek Identify Content - create informative lingotek_entity_ data (in lingotek table) for pre-existing content
 */
function lingotek_batch_identify_translations($process_batch = FALSE) {
  LingotekLog::trace(__METHOD__);

  $operations = array();
  $existing_languages = language_list();
  $entity_types = lingotek_managed_entity_types();

  // I. Identify field-based translations and set statuses

  $fields_to_test_for_translations = array('body'); // tables to test for pre-existing translations
  if (module_exists('comment') && db_field_exists('comment', 'comment_body')) {
      $fields_to_test_for_translations[] = 'comment_body';
  }
  // if a translation for a specific language exists in this table then add a translation status in the lingotek table
  foreach ($entity_types as $entity_type => $entity_type_details) {
    foreach ($existing_languages as $langcode => $language_details) {
      $lingotek_locale = Lingotek::convertDrupal2Lingotek($langcode);
      foreach ($fields_to_test_for_translations as $field) {
        // exclude when language is the source of the node
        $entity_ids = lingotek_list_entities_with_field_in_language($entity_type, $field, $langcode, TRUE);
        foreach ($entity_ids as $entity_id) {
          $operations[] = array('lingotek_batch_keystore_worker', array($entity_type, $entity_id, 'target_sync_status_' . $lingotek_locale, LingotekSync::STATUS_UNTRACKED, FALSE));
        }
      }
    }
  }

  // II. Identify node-based translations and set statuses
  $entity_type = 'node';
  foreach ($existing_languages as $langcode => $language_details) {
    $entity_ids = lingotek_list_nodes_translated_in_language($langcode);
    $lingotek_locale = Lingotek::convertDrupal2Lingotek($langcode);
    foreach ($entity_ids as $entity_id) {
      $operations[] = array('lingotek_batch_keystore_worker', array($entity_type, $entity_id, 'target_sync_status_' . $lingotek_locale, LingotekSync::STATUS_CURRENT, FALSE));
    }
  }

  $batch = array(
    'title' => t('Identifying Existing Translations'),
    'operations' => $operations,
    'finished' => 'lingotek_batch_identify_translations_finished'
  );
  batch_set($batch);
  // If this batch was NOT created from a form_submit() handler, do this to initiate the batch.
  if ($process_batch) {
    batch_process('admin/settings/lingotek/settings');
  }
}

function lingotek_batch_identify_translations_finished($success, $results, $operations) {
  $status_added = 0;
  foreach ($results as $result) {
    if (strpos($result, 'INSERT') !== FALSE) {
      $status_added++;
    }
  }
  $message = format_plural(count($results), t('Identified just one existing translation.'), t('Identified @count existing translations.')) . ' ';
  $message .= $status_added > 0 ? format_plural($status_added, t('One translation status was added.'), t('@count translation statuses were added.')) : t('Translation statuses were already present.');
  drupal_set_message($message);
}

function lingotek_utility_router($method, $params = array()) {
  $callback = "lingotek_" . $method;
  if (is_callable($callback)) {
    call_user_func($callback);
  }
  else {
    return drupal_set_message("Utility not callable: " . $callback, 'error');
  }
}

/**
 * Determine whether site has the required dependencies for node-based translation
 * 
 * @return boolean
 */
function lingotek_node_based_trans_ready() {
  if (module_exists('translation') && module_exists('i18n_node')) {
    return TRUE;
  }
  return FALSE;
}

function lingotek_entity_changed($entity_type, $entity, $entity_id) {
  $updated_entity = $entity;
  if ($entity_type == 'node') {
    $updated_entity = lingotek_node_load_default($entity_id);
  }
  $xml = lingotek_entity_xml_body($entity_type, $updated_entity);
  $hash = md5($xml);
  $oldhash = lingotek_keystore($entity_type, $entity_id, 'hash');

  if (!$oldhash || strcmp($hash, $oldhash)) {
    lingotek_keystore($entity_type, $entity_id, 'hash', $hash);
    return TRUE;
  }
  return FALSE;
}

/**
 * Initialize cleanup batch processing
 * 
 * @param array $context
 * @return boolean
 */
function lingotek_cleanup_batch_init(&$context) {
  // A stub function for now.
  return TRUE;
}

/**
 * Run a function that sets a batch
 *
 * These are functions that set a batch to be processed at the next
 * batch_process() call.
 */
function lingotek_set_batch_function($func_name) {
  $functions = array(
    'lingotek_function_node_languages' => 'lingotek_cleanup_field_languages_for_nodes',
    'lingotek_function_identify_translations' => 'lingotek_batch_identify_translations',
  );
  if (module_exists('comment')) {
    $functions['lingotek_function_comment_languages'] = 'lingotek_cleanup_field_languages_for_comments';
  }
  if (isset($functions[$func_name])) {
    $functions[$func_name]();
  }
}

/**
 * Get all entities enabled for Lingotek, by entity type
 * 
 * @param string $entity_type
 *   The entity type to search for entities
 * @return array
 *   an array of arrays containing entity type and entity ID
 */
function lingotek_get_enabled_entities_by_type($entity_type) {
  $profiles = lingotek_get_profiles();
  $profile_ids = array_keys($profiles);
  return lingotek_get_entities_by_profile_and_entity_type($profile_ids, $entity_type);
}

function lingotek_previously_managed_translations($entity_type, $entity_ids = NULL) {

  // retrieve all entities of the given type with document IDs
  $query = db_select('{lingotek_entity_metadata}', 'lem');
  $query->fields('lem', array('entity_id'))
      ->condition('lem.entity_type', $entity_type)
      ->condition('lem.entity_key', 'document_id');
  if ($entity_ids && is_array($entity_ids)) {
    $query->condition('lem.entity_id', $entity_ids, 'IN');
  }
  $result = $query->execute();
  $entity_ids = $result->fetchCol();

  if (!$entity_ids) {
    return FALSE;
  }

  // filter the entity IDs by ones that still exist
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', $entity_type)
      ->entityCondition('entity_id', $entity_ids, 'IN');
  $entities = $query->execute();
  $entity_ids = array_keys($entities[$entity_type]);

  $enabled_entities = lingotek_get_enabled_entities_by_type($entity_type);
  $entity_ids = array_diff($entity_ids, array_keys($enabled_entities));
  if (empty($entity_ids)) {
    // No disabled entities found.
    return FALSE;
  }
  return TRUE;
}

/**
 * Return the translation source node ID, if it exists, FALSE otherwise
 *
 * @param object $node
 * @return mixed translation source node ID or FALSE if not node translation
 */
function lingotek_is_node_translation($node = NULL) {
  // The case of creating a new node-based translation.
  if (isset($_GET['translation']) && isset($_GET['target']) && (entity_load_single('node', (int) $_GET['translation']))) {
    return $_GET['translation'];
  }
  // locate a translation source node ID, if available
  $tnid = !empty($node->tnid) && $node->tnid !== $node->nid && $node->tnid !== 0 ? $node->tnid : NULL;
  if (!$tnid) {
    $tnid = !empty($node->translation_source->nid) && (empty($node->nid) || ($node->nid !== $node->translation_source->nid && $node->nid !== 0)) ? $node->translation_source->nid : NULL;
  }
  // The case of updating an existing node-based translation.
  if ($tnid) {
    return $tnid;
  }
  // Not a node-based translation.
  return FALSE;
}

function lingotek_managed_entity($entity_type, $entity) {
  list($id, $vid, $bundle) = lingotek_entity_extract_ids($entity_type, $entity);

  // If there is no $id or $bundle property, then nothing to manage for now.
  if (!$id || !$bundle) {
    return FALSE;
  }
  // Check the entity itself for overrides.
  $query = db_select('{lingotek_entity_metadata}', 'lem')
      ->fields('lem', array('value'))
      ->condition('lem.entity_type', $entity_type)
      ->condition('lem.entity_id', $id)
      ->condition('lem.entity_key', 'profile');
  $result = $query->execute()->fetchField();
  if ($result !== FALSE) {
    if ($result === LingotekSync::PROFILE_DISABLED) {
      return FALSE;
    }
    else {
      return TRUE;
    }
  }
  // Check the entity's bundle.
  return lingotek_enabled_bundle($entity_type, $bundle);
}

function lingotek_entity_extract_ids($entity_type, $entity) {
  try {
    list($entity_id, $vid, $bundle_name) = entity_extract_ids($entity_type, $entity);
  } catch (EntityMalformedException $ex) {
    // check for non-standard entity types
    if ($entity_type == 'taxonomy_term') {
      // TODO: populate taxonomy_term's entity fields
      $entity_id = $entity->tid;
      $vid = NULL;
      $bundle_name = $entity->type;
    }
  }
  return array($entity_id, $vid, $bundle_name);
}

function lingotek_enabled_langcode($langcode) {
  $enabled_languages = lingotek_get_target_locales(TRUE);
  $requested_language = Lingotek::convertDrupal2Lingotek($langcode);
  if (in_array($requested_language, $enabled_languages)) {
    return TRUE;
  }
  return FALSE;
}

function lingotek_entity_langcode($entity_type, $entity) {
  $info = entity_get_info($entity_type);
  // Default to string 'language' if no language-related entity key is found.
  $lang_key = !empty($info['entity keys']['language']) ? $info['entity keys']['language'] : 'language';
  return !empty($entity->$lang_key) ? $entity->$lang_key : NULL;
}

function lingotek_get_source_node($node) {
  if (!empty($node->tnid)) {
    return lingotek_entity_load_single('node', $node->tnid);
  }
  return lingotek_entity_load_single('node', $node->nid);
}
