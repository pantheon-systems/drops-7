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
 * Helper function, for storing additional information with a Node.
 * Example usage (GET):
 *    lingotek_lingonode('all') - returns all nodes and properties
 *    lingotek_lingonode(5) - returns all properties for the specified NodeId (i.e., 5)
 *    lingotek_lingonode(5,'node_sync_status') - returns the value for the specified property (i.e., node_sync_status) for the specified NodeId (i.e., 5)
 * Example usage (SET):
 *     lingotek_lingonode(5,'node_sync_status','CURRENT') - sets the value to 'CURRENT' for the property 'node_sync_status' of NodeId 5
 *
 * @param $nid
 *  NodeId.
 * @param $key
 *  (optional) "" Key to look up in the database.  If no key is specified, then
 *  every key for the Node is returned with it's value.
 * @param $value
 *  (optional) "" Value to save.  If "" or no value is given for $value, then
 *  it will return the $value of the first found instance of the specified $key
 *  in the database.  Returns FALSE if no value is found.
 */

function lingotek_lingonode($nid, $key = "", $value = "") {
  if ($nid == 'all') {
    $lingo_node = array();
    $result = db_select('lingotek', 'n')->fields('n', array(db_escape_field('nid'), db_escape_field('lingokey'), db_escape_field('lingovalue')))->execute();
    foreach ($result as $row) {
      $lingo_node[$row->nid][$row->lingokey] = check_plain($row->lingovalue);
    }
    return $lingo_node;
  }
  if ($nid == -1) {
    LingotekLog::error("Invalid -1 nid passed to lingotek_lingonode().", array('@nid' => $nid, '@key' => $key, '@value' => $value));
    return FALSE;
  }
  if (is_numeric($nid) && $nid) {
    //Return an array with all of the keys and values.
    if ($key === "") {
      $lingo_node = array();

      $result = db_select('lingotek', 'n')->fields('n', array(db_escape_field('lingokey'), db_escape_field('lingovalue')))->condition(db_escape_field('nid'), $nid)->execute();
      foreach ($result as $row) {
        $lingo_node[$row->lingokey] = check_plain($row->lingovalue);
      }

      return $lingo_node;
    }
    //Get value for the specified key, only returns the first match
    elseif ($value === "") {
      $result = db_select('lingotek', 'n')->fields('n', array(db_escape_field('lingovalue')))->condition(db_escape_field('nid'), $nid)->condition(db_escape_field('lingokey'), $key)->execute();
      $row = $result->fetchObject();

      if ($row) {
        return check_plain($row->lingovalue);
      }
      else {
        return FALSE;
      }
    }
    //Insert/Update the specified key and value
    else {
      $row = array(
        db_escape_field('nid') => $nid,
        db_escape_field('lingokey') => $key,
        db_escape_field('lingovalue') => $value,
      );

      if (lingotek_lingonode($nid, $key) === FALSE) { //Insert
        drupal_write_record('lingotek', $row);
        return "$nid : $key => $value INSERTED";
      }
      else { //Update
        drupal_write_record('lingotek', $row, array(db_escape_field('nid'), db_escape_field('lingokey')));
        return "$nid : $key => $value UPDATED";
      }
    }
  }
  else {
    LingotekLog::error("Invalid nid (@nid) passed to lingotek_lingonode().", array('@nid' => $nid, '@key' => $key, '@value' => $value));
    return FALSE;
  }
}

/*
 * When a node is deleted, this is called to clean out the extra data from that node in the Lingotek table.
 *
 * @param $nid
 *  NodeId.
 */

function lingotek_lingonode_delete($nid) {
  db_delete('lingotek')->condition('nid', $nid)->execute();
}

/*
 * Revert from having been embedded in XML
 *
 * @param $text
 *  Text to be unescaped
 * @return
 *  Text that has been unescaped
 */

function lingotek_xml_decode($text) {
  //return htmlspecialchars_decode($text);
  $text = str_replace("&lt;", "<", $text);
  $text = str_replace("&gt;", ">", $text);
  $text = str_replace("&apos;", "'", $text);
  $text = str_replace("&quot;", "\"", $text);
  $text = str_replace("&amp;", "&", $text);
  return $text;
}

/**
 * Run this on nodes you aren't sure if they've been initialized yet for use with Lingotek.
 */
function lingotek_node_init_default($node) {
  if (lingotek_lingonode($node->nid, 'document_id') === FALSE) {
    LingotekLog::trace("lingotek_node_init_default", array('nid' => $node->nid, 'language' => $node->language));
    LingotekApi::instance()->addContentDocument($node, TRUE);
    lingotek_lingonode($node->nid, 'phase_template_id', variable_get('lingotek_phase_template', 2));
  }
}

/**
 * Synchronize the node's content with current translations as stored on the Lingotek platform.
 *
 * @param object $node
 *   A Drupal node object.
 */
function lingotek_node_sync(&$node) {
  // Keep synchronized?
  $sync_method = lingotek_variable_get(lingotek_lingonode($node->nid, 'sync_method'), 'lingotek_sync', 100);
  $lingotek_document_id = lingotek_lingonode($node->nid, 'document_id');
  LingotekLog::trace("lingotek_node_sync", array("nid" => $node->nid, 'sync_method' => $sync_method));
  if ($sync_method != "0" && $lingotek_document_id) {
    $progress = lingotek_get_document_targets(lingotek_lingonode($node->nid, 'document_id'));
    // Check for changes in % complete.
    foreach ($progress as $lingotek_locale => $target) {
      $key = 'percent_complete_' . $lingotek_locale;
      $before = lingotek_lingonode($node->nid, $key);
      if ($before == "") {
        $before = "0";
      }
      $after = $target->percentComplete;
      if (
          ($sync_method == "1" && $before !== (string) $after) ||
          ($sync_method == "100" && $after == "100" && $before !== (string) $after)
      ) {
        LingotekLog::trace("lingotek_node_sync percent complete changed", array('target_locale' => $lingotek_locale, 'nid' => $node->nid, 'percent' => $after));
        lingotek_lingonode($node->nid, $key, $after);
        lingotek_download_document($node, $lingotek_locale);
      }
      else {
        LingotekLog::trace("lingotek_node_sync percent complete stayed the same", array('target_locale' => $lingotek_locale, 'nid' => $node->nid, 'percent' => $after));
      }
    }
  }
}

/*
 * Filter for removing unchecked checkboxes from an array for drupal forms
 */

function lingotek_unselected($var) {
  return ($var != "0");
}

/*
 * Create and return an empty default node
 */

function lingotek_empty_node() {
  $node = new stdClass();
  $node->nid = -2;
  $node->language = LANGUAGE_NONE;
  return $node;
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

/**
 * Return the xml representation of the source content for a node.
 *
 * @param object $node
 *   A Drupal node.
 *
 * @return string
 *   The XML representation of the node in Lingotek format.
 */
function lingotek_xml_node_body($node) {
  $translatable = array();

  foreach ($node as $key => $value) {
    $field = field_info_field($key);
    if (isset($field) && array_key_exists('lingotek_translatable', $field) && $field['lingotek_translatable'] == 1) {
      array_push($translatable, $key);
    }
  }

  $content = '';
  foreach ($translatable as $field) {
    $language = $node->language;
    if (!array_key_exists($language, $node->$field)) {
      $language = LANGUAGE_NONE;
    }
    $text = & $node->$field;
    // Deal with not being initialized right, such as pre-existing titles.
    if (!array_key_exists($language, $node->$field) || !array_key_exists(0, $text[$language])) {
      continue;
    }

    // We may split compound Drupal fields into several Lingotek fields.
    $target_keys = array(
      'value' => '', // Most text fields
      'summary' => 'summary' // "Long text with summary" fields have this sub-field value as well.
    );

    // Create fields from all target keys.
    foreach ($target_keys as $target_key => $element_suffix) {
      if (!empty($text[$language][0][$target_key])) {
        $element_name = $field;
        if (!empty($element_suffix)) {
          $element_name .= '__' . $element_suffix;
        }

        $current_field = '<' . $element_name . '>';

        foreach ($text[$language] as $key => $value) {
          if (!array_key_exists('value', $value)) {
            //TODO add TAGs to be translatable
            /*
              $terms = $node->$field;
              foreach ($terms[$language] as $term) {
              // Do something.
              }
             */
            continue;
          }

          $current_field .= '<element><![CDATA[' . $value[$target_key] . ']]></element>' . "\n";
        }

        $current_field .= '</' . $element_name . '>';
        $content .= $current_field;
      }
    }
  }

  //Menus related to the page:
  $menu = menu_link_get_preferred('node/' . $node->nid);
  $txt = $menu['link_title'];
  if ($txt != "") {
    $content = $content . "<menu_title><![CDATA[$txt]]></menu_title>\n";
  }

  //URL Alias related to the page:
  $url_alias_translation = lingotek_variable_get(lingotek_lingonode($node->nid, 'url_alias_translation'), 'lingotek_url_alias_translation', 1);
  if ($url_alias_translation == 1) {
    $conditions = array('source' => 'node/' . $node->nid);
    if ($node->language != LANGUAGE_NONE) {
      $conditions['language'] = $node->language;
    }
    $path = path_load($conditions);
    if ($path !== FALSE) {
      $url = $path['alias'];
      $content = $content . "<url_alias><![CDATA[$url]]></url_alias>\n";
    }
  }

  return "<?xml version=\"1.0\" encoding=\"UTF-8\"?><contents>$content</contents>";
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
  if (LingotekAccount::instance()->isEnterprise()) {
    if (Lingotek::isSupportedLanguage($node->language) && lingotek_supported_type($node->type) && node_access('update', $node)) {
      if (lingotek_managed_by_entity_translation($node->type) && !lingotek_node_pushed($node)) {
        return FALSE;
      }
      return user_access($permission);
    }
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
  // OR when not Enterprise
  $user_access = user_access($permission);
  if ((module_exists('entity_translation') && entity_translation_node_supported_type($node->type) && $user_access && !lingotek_node_pushed($node))
      || !LingotekAccount::instance()->isEnterprise()) {
    return FALSE;
  }
  // Default: Standard user access
  return user_access($permission);
}

/**
 * Returns whether the given node type has support for translations.
 *
 * @return
 *   Boolean value.
 */
function lingotek_supported_type($type) {
  $lingotek_supported_explicitly = variable_get('language_content_type_' . $type, NULL) == LINGOTEK_ENABLED;
  $entity_translation_managed = lingotek_managed_by_entity_translation($type);
  return $lingotek_supported_explicitly || $entity_translation_managed; //we also support entity_translation managed content
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
  $lingonode = lingotek_lingonode($node->nid, 'document_id');
  $pushed_to_lingotek = $lingonode !== FALSE; //!empty($lingonode);
  return $pushed_to_lingotek;
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
  return !(variable_get('lingotek_flush_cache', FALSE) && user_access('dev'));
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
 * Gets the Lingotek Source Language ( Drupal variable: lingotek_source_language).
 * Returns a language code (ie:  en / es / de)
 * Uses the Drupal default language as a fallback.
 */
function lingotek_get_source_language() {

  $source_language = variable_get('lingotek_source_language', NULL);
  if (empty($source_language)) {
    $drupal_default_language = language_default();
    $source_language = $drupal_default_language->language;
  }

  return $source_language;
}

// END:  lingotek_get_source_language()

/**
 * Content node types linked to 'translatable' fields.
 */
function lingotek_translatable_node_types() {

  $types = array();

  $fields = lingotek_translatable_node_field_details();
  foreach ($fields as $field) {
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
function lingotek_translatable_node_field_details() {

  $fields = field_info_fields();
  $translatable_fields = array();

  foreach ($fields as $field_id => $field) {
    foreach ($field['bundles'] as $type => $instance) {
      /*
        echo '<br>FieldID: ' . $field_id;
        echo '<br>Field: ' . $field;
        echo '<br>Type: ' . $type;
        echo '<br>Instance: ' . $instance;
       */

      if (field_is_translatable($type, $field)) {
        //echo '<br>Translatable: YES!' ;
        $field_db_table = array_keys($field['storage']['details']['sql']['FIELD_LOAD_CURRENT']);
        $field_db_table = array_shift($field_db_table);
        $translatable_fields[] = array(
          'entity_type' => $type,
          'machine_name' => $field['field_name'],
          'db_table' => $field_db_table,
          'bundles' => $field['bundles'][$type],
        );
      }
      //echo '<br>';
    }
  }

  /* Return data format
    array (
    0 =>
    array (
    'entity_type' => 'node',
    'machine_name' => 'body',
    'db_table' => 'field_data_body',
    'bundles' =>
    array (
    0 => 'page',
    1 => 'article',
    ),
    ),
    1 =>
    array (
    'entity_type' => 'node',
    'machine_name' => 'title_field',
    'db_table' => 'field_data_title_field',
    'bundles' =>
    array (
    0 => 'article',
    1 => 'page',
    ),
    ),
    )
   */
  return $translatable_fields;
}

// END:  lingotek_translatable_node_field_details()
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

// END:  lingotek_get_active_languages()

/**
 * Flags a target language as active:FALSE in the Target Language tracking.
 */
function lingotek_delete_target_language($lingotek_locale) {
  $result = FALSE;

  if (is_string($lingotek_locale) && strlen($lingotek_locale)) {

    db_update('languages')
        ->fields(array(
          'enabled' => 0,
          'lingotek_enabled' => 0,
        ))
        ->condition('lingotek_locale', $lingotek_locale) //->condition('language',  $drupal_language_code)
        ->execute();
    LingotekLog::info("Target language removed: @lingotek_locale"
        , array(
      '@lingotek_locale' => $lingotek_locale)
    );

    // Removes the node sync target language entries from the lingotek table.
    LingotekSync::deleteTargetEntriesForAllNodes($lingotek_locale);

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
 * @param $lingotek_enabled whether or not to enable the language as being lingotek_enabled (default: 1)
 * @param $lingotek_locale the lingotek locale that the drupal code should be associated with (it will try to pick the right one when not passed in)
 * @param $api_add boolean whether or not to add the language to all documents in the project (default: TRUE)
 * 
 */
function lingotek_set_target_language($drupal_language_code, $lingotek_enabled = 1, $lingotek_locale = NULL, $api_add = TRUE) {
  $result = FALSE;

  if (is_string($drupal_language_code) && strlen($drupal_language_code)) {
    $lingotek_locale = is_null($lingotek_locale) ? Lingotek::convertDrupal2Lingotek($drupal_language_code, FALSE) : $lingotek_locale;
    db_update('languages')
        ->fields(array(
          'enabled' => 1,
          'lingotek_enabled' => $lingotek_enabled ? 1 : 0,
          'lingotek_locale' => $lingotek_locale
        ))
        ->condition('language', $drupal_language_code)
        ->execute();
    LingotekLog::info("Target language added: @drupal_language_code", array('@drupal_language_code' => $drupal_language_code));

    if ($lingotek_enabled && $lingotek_locale && $api_add) { // Add the Target Language to the Lingotek Project.
      $api = LingotekApi::instance();
      $projects = LingotekSync::getSyncProjects();
      foreach ($projects as $project_id) {
        $result = $api->addTranslationTarget(NULL, $project_id, $lingotek_locale);
      }
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
  lingotek_add_missing_locales(); // fills in any missing lingotek_locale values to the languages table
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

  LingotekSync::insertTargetEntriesForAllNodes($lingotek_locale); // Add the node sync target language entries to the lingotek table.
  return lingotek_set_target_language($drupal_language_code, 1, $lingotek_locale, $call_api);
}

/**
 * Fills in any missing lingotek_locale values to the languages table
 */
function lingotek_add_missing_locales() {
  LingotekLog::trace(__METHOD__);
  $languages = language_list();
  $default_language = language_default();
  $update_static_language_list = FALSE;
  //$source_language = variable_get( 'lingotek_source_language', $default_language->language );
  foreach ($languages as $target) {
    if (isset($target->lingotek_locale) && !strlen($target->lingotek_locale)) {
      $drupal_language_code = $target->language;
      // Set default when Enterprise to 1 (unless source language, then set to 0)
      //$lingotek_enabled = LingotekAccount::instance()->isEnterprise() ? (($target->language == $default_language->language) ? 0 : 1) : $target->lingotek_enabled;
      $lingotek_enabled = $target->lingotek_enabled;
      $lingotek_locale = Lingotek::convertDrupal2Lingotek($drupal_language_code, FALSE);
      lingotek_enable_language_by_code($drupal_language_code, $lingotek_enabled, $lingotek_locale);
      $update_static_language_list = TRUE;
    }
  }
  if ($update_static_language_list) {
    drupal_static_reset('language_list');
  }
}

/**
 * Add lingotek_locale and lingotek_enable to the language table for the passed in drupal_language_code 
 */
function lingotek_enable_language_by_code($drupal_language_code, $lingotek_enabled = 1, $lingotek_locale = NULL) {
  $field_data = array(
    'enabled' => 1,
    'lingotek_enabled' => $lingotek_enabled ? 1 : 0,
  );
  if (!is_null($lingotek_locale)) {
    $field_data['lingotek_locale'] = $lingotek_locale;
  }
  db_update('languages')
      ->fields($field_data)
      ->condition('language', $drupal_language_code)
      ->execute();
  return $lingotek_locale;
}

/**
 * Get only the languages that are lingotek_enabled
 */
function lingotek_get_target_locales($codes_only = TRUE) {
  $target_languages = db_query("SELECT * FROM {languages} WHERE lingotek_enabled = :lingotek_enabled", array(':lingotek_enabled' => 1))->fetchAll();
  $target_codes = array();
  foreach ($target_languages as $target_language) {
    $target_codes[] = $target_language->lingotek_locale;
  }
  return $codes_only ? $target_codes : $target_languages;
}

/**
 * Get only the languages that are lingotek_enabled
 */
function lingotek_get_target_languages($codes_only = TRUE) {
  $target_languages = db_query("SELECT * FROM {languages} WHERE lingotek_enabled = :lingotek_enabled", array(':lingotek_enabled' => 1))->fetchAll();
  $target_codes = array();
  foreach ($target_languages as $target_language) {
    $target_codes[] = $target_language->language;
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

function lingotek_get_content_types($ids_only = FALSE) {
  $raw_types = node_type_get_types();
  $types = array();
  foreach ($raw_types as $raw_type_id => $raw_type) {
    $lingotek_supported_explicitly = variable_get('language_content_type_' . $raw_type_id, NULL) == LINGOTEK_ENABLED;
    if ($lingotek_supported_explicitly) {
      $types[$raw_type_id] = $raw_type;
    }
  }
  return $ids_only ? array_keys($types) : $types;
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

/**
 * Clean-up utility
 */
function lingotek_cleanup_utility($show_messages = TRUE) {
  LingotekLog::trace(__METHOD__);

  lingotek_set_priority();
  lingotek_set_defaults();
  lingotek_migration_1();
  lingotek_migration_2();
  lingotek_migration_3();
  lingotek_add_missing_locales(); // fills in any missing lingotek_locale values to the languages table

  lingotek_field_language_data_cleanup_batch_create();
  if ($show_messages) {
    drupal_set_message(t('The field data cleanup utility completed.'));
  }

  lingotek_batch_identify_content(); // Identify pre-existing content (based on these new content-type settings)
  if ($show_messages) {
    drupal_set_message(t('Translatable content has been identified.'));
  }
}

/**
 * Sets the priority of the Lingotek Translation module
 */
function lingotek_set_priority() {
  db_update('system')->fields(array('weight' => 12))->condition('name', 'lingotek')->execute();
}

/**
 * Sets the global defaults for the Lingotek Translation module
 */
function lingotek_set_defaults() {
  LingotekLog::trace(__METHOD__);
  $defaults = array(
    'lingotek_sync' => 1, // auto-download
    'lingotek_create_documents_by_default' => 1, // auto-upload
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
    lingotek_enable_language_by_code($drupal_language_code, 1);
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
      //dpm($old_key . " => " . $new_key);
      $query = db_update('lingotek', $ret)
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
    else {
      //dpm("skip: ".$old_key);
    }
  }
  $ret['total_affected_rows'] = $total_affected_rows;
  //drupal_set_message("fields updated");
  return $ret;
}
