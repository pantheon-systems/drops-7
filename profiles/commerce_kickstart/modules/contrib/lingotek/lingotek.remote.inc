<?php

/**
 * @file
 * Call the Lingotek APIs
 */
$GLOBALS['_lingotek_client'] = new LingotekSession();

/*
 * This function is only used for node-based translations.
 *
 * @param $node
 *  Node in the source language
 * @param $drupal_language_code
 *  The language which we are dealing with
 */

function lingotek_get_target_node($node, $drupal_language_code) {

  // Allow the source to be overwritten as the target, if applicable, and update
  // the current language to be the same as the target.
  if ($node->language == $drupal_language_code && !empty($node->lingotek['allow_source_overwriting'])) {
    lingotek_keystore('node', $node->nid, 'source_language_' . Lingotek::convertDrupal2Lingotek($drupal_language_code), $drupal_language_code);
    return $node;
  }

  $tset = lingotek_node_get_translations($node->nid);

  if ($node->tnid == 0) {
    $node->tnid = $node->nid;
    db_update('node')
        ->fields(array('tnid' => $node->nid))
      ->condition('nid', $node->nid)
      ->execute();
  }

  if (isset($tset[$drupal_language_code])) {
    // If revisioning is enabled for this content type,
    // then create a new revision of it;
    // otherwise, just load the existing one to be updated.
    $current_node = lingotek_node_load_default($tset[$drupal_language_code]->nid);
    $content_type_options = variable_get('node_options_' . $current_node->type, array());
    if (in_array('revision', $content_type_options)) {
      $current_node->revision = TRUE;
      $current_node->log = "Downloaded translation changes from Lingotek for language code '$drupal_language_code'.";
      $current_node->status = 0;
      node_save($current_node);
      $localized_node = lingotek_node_load_default($tset[$drupal_language_code]->nid);
    }
    else {
      $localized_node = $current_node;
    }
  }
  else {
    $localized_node = new stdClass();
    $localized_node->type = $node->type;
    node_object_prepare($localized_node);

    // Grandfather the lingotek settings (ie. profile, etc.) on the new node.
    if (!empty($node->lingotek)) {
      $localized_node->lingotek = $node->lingotek;
    }
    $localized_node->title = $node->title . ' (' . $drupal_language_code . ')';
    $localized_node->tnid = $node->tnid;
    $localized_node->language = $drupal_language_code;


    $localized_node->uid = $node->uid;
    $localized_node->name = $node->name;
    $localized_node->comment = $node->comment;
    $localized_node->promote = $node->promote;
    $localized_node->sticky = $node->sticky;
    $localized_node->status = $node->status;
    $localized_node->auto_upload = FALSE;

    // Grandfather field settings/values from source node to target.
    $source_fields = field_info_instances('node', $node->type);
    foreach (array_keys($source_fields) as $key) {
      $copied_field = $node->$key;
      if (!empty($copied_field[$node->language])) {
        $copied_field[$localized_node->language] = $copied_field[$node->language];
        unset($copied_field[$node->language]);
      }
      $localized_node->$key = $copied_field;
    }

    $lingotek_fields = variable_get('lingotek_enabled_fields');
    foreach ($lingotek_fields['node'][$localized_node->type] as $field_name) {
      // Continue if translation of original node titles is enabled.
      if ($field_name == 'title') {
        continue;
      }
      $field = $node->$field_name;
      $f = array();
      if (isset($field[$node->language])) {
        foreach ($field[$node->language] as $key => $value) {
          if (isset($value['format'])) {
            $f[$drupal_language_code][$key]['format'] = $value['format'];
          }
        }
      }

      $localized_node->$field_name = $f;
    }

    node_save($localized_node);
    LingotekSync::setUploadStatus('node', $localized_node->nid, LingotekSync::STATUS_TARGET);
    // Child node should keep its parent node's profile, for rules integration.
    $parent_profile = lingotek_keystore('node', $node->nid, 'profile');
    if ($parent_profile !== FALSE) {
      lingotek_keystore('node', $localized_node->nid, 'profile', $parent_profile);
    }
  }

  return $localized_node;
}

function lingotek_process_entity_xml($xml, &$entity, $entity_type, $langcode, $node_based_translation = FALSE, $url_alias_translation = 0) {

  $language_list = language_list('language');
  $source_entity = $entity;

  $hook_params = array(
    'entity_type' => $entity_type,
    'entity' => $entity,
    'xml' => $xml,
    'langcode' => $langcode,
  );

  // First allow other modules to manipulate the downloaded content.
  drupal_alter('lingotek_entity_download', $hook_params);

  // Special handling of menu links
  if ($entity_type == 'menu_link') {
    $translation_set = lingotek_get_translation_set($entity->mlid, $entity->menu_name);
    lingotek_process_menu_link_xml($xml, $entity, $entity_type, $langcode, $translation_set);
    return;
  }

  // Set $node_based_translation only if the entity type is a node, inherit otherwise.
  // (The entity could be a field collection or other entity type, underneath)
  if ($entity_type == 'node') {
    $node_based_translation = lingotek_uses_node_translation($entity) ? TRUE : FALSE;
    if ($node_based_translation) {
      $source_entity = lingotek_get_source_node($entity);
    }
  }

  list($id, $vid, $bundle) = lingotek_entity_extract_ids($entity_type, $entity);
  $fields_arrays = array();
  $delta = 0;
  $last_field_name = NULL;

  foreach ($xml as $field_name => $content) {
    // URL alias translation (currently available for nodes only).
    if ($entity_type == 'node') {
      // Check if we use normal title instead of field title.
      if (variable_get('lingotek_translate_original_node_titles', FALSE) && $field_name == 'title' && $node_based_translation) {
        $target = check_plain($content);
        db_update('node')
          ->fields(array('title' => $target))
          ->condition('nid', $id)
          ->execute();
        db_update('node_revision')
          ->fields(array('title' => $target))
          ->condition('vid', $vid)
          ->execute();

        // Set URL alias.
        if ($url_alias_translation == 2 && module_exists('pathauto') && $entity->language != LANGUAGE_NONE) {
          module_load_include('inc', 'pathauto');
          $uri = entity_uri('node', $entity);
          $entity_unchanged = entity_load_unchanged('node', $id);
          pathauto_create_alias('node', 'update', $uri['path'], array('node' => clone $entity_unchanged), $entity->type, $langcode);
        }
      }
      elseif ($field_name == 'url_alias' && $url_alias_translation == 1) {
        lingotek_save_url_alias($content, $entity, $langcode);
        $last_field_name = $field_name;
        continue;
      }
    }

    // Handle multiple instances of the same field (handled in Drupal using 'delta')
    $delta = ($field_name == $last_field_name) ? ++$delta : 0;

    // Get field info or skip if not found
    $field_info = field_info_field($field_name);
    if (empty($field_info)) {
      LingotekLog::warning('Invalid field downloaded from Lingotek: @field_name.  The field may no longer exist; or if you have upgraded the Lingotek module since you last uploaded the document, the document format may have changed.  You should re-upload @entity_type #@entity_id', array('@field_name' => $field_name, '@entity_type' => $entity_type, '@entity_id' => $id));
      $last_field_name = $field_name;
      continue;
    }

    $translatable_field = !empty($field_info['translatable']);
    $field_language = $node_based_translation && !$translatable_field ? LANGUAGE_NONE : $langcode;

    // Field-Collection Fields: Pull the underlying entities and recurse.
    if (module_exists('field_collection') && $field_info['type'] == 'field_collection') {
      lingotek_process_field_collection_xml($content, $entity_type, $entity, $field_name, $delta, $langcode, $node_based_translation);
      $last_field_name = $field_name;
      continue;
    }

    // Field-Collection Entities: Set all field-collection fields to translatable,
    // if the parent node is not using node-based translation.
    if ($entity_type == 'field_collection_item' && !$node_based_translation && $field_info['translatable'] != 1) {
      $field_info['translatable'] = 1;
      field_update_field($field_info);
    }

    // Handle individual fields from here down.
    $insert_array = array(
      'entity_type' => $entity_type,
      'bundle' => $bundle,
      'entity_id' => $id,
      'revision_id' => $vid === NULL || $entity_type == 'comment' ? $id : $vid,
      'language' => $field_language,
      'delta' => $delta,
      'deleted' => '0',
    );

    foreach ($content as $column_name => $text) {
      $db_column_name = $field_info['field_name'] . '_' . $column_name;
      $insert_array[$db_column_name] = lingotek_unfilter_placeholders(decode_entities((string) $text->element));

      // Truncate too-long title fields
      if ($db_column_name == 'title_field_value' && strlen($insert_array[$db_column_name]) > 254) {
        $insert_array[$db_column_name] = substr($insert_array[$db_column_name], 0, 254);
        $langcode = $insert_array['language'];
        LingotekLog::info('The @lang (@langcode) title was truncated, since the translation exceeded the maximum of 255 characters.', array('@lang' => $language_list[$langcode]->name, '@langcode' => Lingotek::convertDrupal2Lingotek($langcode)));
      }
    }

    // Try to carry over untranslated columns.
    try {
      $source_field_data = field_get_items($entity_type, $source_entity, $field_name);
    } catch (EntityMalformedException $e) {
      $source_field_data = array();
    }
    $curr_field_data = & $entity->$field_name;
    lingotek_add_untranslated_source_fields($insert_array, $source_field_data, $curr_field_data, $field_info, $entity->language, $delta);

    // Save to both field_data tables and field_revision tables
    $field_table_names = array('field_revision_' . $field_name, 'field_data_' . $field_name);
    foreach ($field_table_names as $fname) {
      lingotek_save_field_record($fname, $insert_array);
      // Node-based translations within a field-based translation system by default
      // should also have the language-neutral fields saved for normalization if
      // underneath a field collection. This prevents source-language content
      // from blocking out translated content from being shown.
      if ($node_based_translation && $entity_type == 'field_collection_item' && $field_language != LANGUAGE_NONE) {
        $insert_array['language'] = LANGUAGE_NONE;
        lingotek_save_field_record($fname, $insert_array);
        $insert_array['language'] = $field_language;
      }
    }

    // After every field insert, reset the caches and reload the entity
    // TODO: Do we really need to do this every time we save a field, or
    // can we just do this once at the end?
    cache_clear_all('field:' . $entity_type . ':' . $id, 'cache_field');
    entity_get_controller($entity_type)->resetCache(array($id));
    $entity = lingotek_entity_load_single($entity_type, $id);

    //Set URL alias
    if ($field_name == 'title_field' && $url_alias_translation == 2 && module_exists('pathauto') && $entity->language != LANGUAGE_NONE) {
      lingotek_save_pathauto_alias($entity_type, $entity, $langcode);
    }
    $last_field_name = $field_name;
  }
}

/*
 * Inserts newly downloaded menu link data into the menu_links table
 */
function lingotek_process_menu_link_xml($xml, &$entity, $entity_type, $langcode, $translation_set) {
  $item = array(
    'link_title' => '',
    'mlid' => 0,
    'plid' => 0,
    'menu_name' => $entity->menu_name,
    'weight' => 0,
    'link_path' => $entity->link_path,
    'options' => array(),
    'customized' => 1,
    'module' => 'menu',
    'expanded' => 0,
    'hidden' => 0,
    'has_children' => 0,
    'language' => $langcode,
  );

  foreach ($xml as $field_name => $content) {
    foreach ($content as $column_name => $text) {
      $value = lingotek_unfilter_placeholders(decode_entities((string) $text->element));

      if ($field_name == 'title') {
        $item['link_title'] = $value;
      }
      elseif ($field_name == 'description') {
        $item['options']['attributes']['title'] = $value;
      }
    }
  }

  $translations = array(
    $langcode => $item,
  );

  $translation_set->add_translations($translations);
  $item['translation_set'] = $translation_set;
  menu_link_save($item);
}

/*
 * Carry over the columns that weren't translated from the source field
 */
function lingotek_add_untranslated_source_fields(&$params, $source_field_data, $curr_field_data, $field_info, $entity_langcode, $delta) {

  if (empty($field_info['columns']) || empty($field_info['field_name'])) {
    return;
  }

  foreach (array_keys($field_info['columns']) as $column_name) {
    $db_column_name = $field_info['field_name'] . '_' . $column_name;
    // if it's set in the translated stuff, then don't carry over.
    if (isset($params[$db_column_name])) {
      continue;
    }

    $field_column = NULL;
    if (isset($source_field_data[$delta][$column_name])) {
      $field_column = $source_field_data[$delta][$column_name];
    }
    elseif (isset($curr_field_data[$entity_langcode][$delta][$column_name])) {
      $field_column = $curr_field_data[$entity_langcode][$delta][$column_name];
    }
    if ($field_column !== NULL) {
      // Arrays, such as those in attributes fields, should be serialized first.
      if (is_array($field_column)) {
        $field_column = serialize($field_column);
      }
      $params[$db_column_name] = $field_column;
    }
  }
}

/*
 * Save a PathAuto alias
 */
function lingotek_save_pathauto_alias($entity_type, $entity, $langcode) {
  list($id, $vid, $bundle) = lingotek_entity_extract_ids($entity_type, $entity);
  module_load_include('inc', 'pathauto');
  $uri = entity_uri($entity_type, $entity);
  $entity_unchanged = entity_load_unchanged($entity_type, $id);
  pathauto_create_alias($entity_type, 'update', $uri['path'], array($entity_type => clone $entity_unchanged), $bundle, $langcode);
}

/*
 * Save a field, either insert or update
 */
function lingotek_save_field_record($fname, $params) {
  // using drupal_write_record to avoid node_save - node_save overwrites publications unless called on both revised and published versions of the node (i.e. workbench_moderation)
  // UPDATE: This could perhaps be rewritten now that workbench_moderation is supported through the rules module
  $entity_type = $params['entity_type'];
  $entity_id = $params['entity_id'];
  $langcode = $params['language'];
  $lingotek_locale = Lingotek::convertDrupal2Lingotek($langcode);
  try {
    LingotekSync::deleteLastSyncError($entity_type, $entity_id);
    drupal_write_record($fname, $params);
  } catch (PDOException $e) {

    /**
     *Error logging became necessary for this try-catch when writing to the database
     *because the drupal_write_record() was failing and we had no way of knowing
     *why it was happening or even that is was happening because of the try-catch.
     *
     *The following code logs the error with the approriate detail and also displays
     *a drupal message is the error is that the data was too long to be inserted
     *into a specific column so that end users can know what happened and know why
     *a particular translation is not working
     *
     *@author t.murphy 
     *
     */

    $state_error = $e->errorInfo[0];
    $driver_error = $e->errorInfo[1];
    $error_message = $e->errorInfo[2];

    if ($state_error !== '23000' && $driver_error !== '1062') {
      $log_message = t('The following error occurred while writing to the database: SQLSTATE error @sqlError (Driver-specific error code @driverError): @errorMessage', array( '@sqlError' => $state_error, '@driverError' => $driver_error, '@errorMessage' => $error_message));
      LingotekLog::error($log_message, $e);
    }
    if ($state_error === '22001') {
      $str_args = array();
      foreach ($e->args as $key => $value) {
        array_push($str_args, (string)$value);
      }
      $message = t('Entity @entity_id had the translation @errorDetail', array('@entity_id' => $str_args[3], '@errorDetail' => $e->errorInfo[2]));
      drupal_set_message($message, 'error');

      LingotekSync::setTargetStatus($entity_type, $entity_id, $lingotek_locale, LingotekSync::STATUS_ERROR);
      LingotekSync::setLastSyncError($entity_type, $entity_id, $error_message);
    }

    $primary_keys = array(
      'entity_type',
      'entity_id',
      'revision_id',
      'deleted',
      'delta',
      'language',
    );
    drupal_write_record($fname, $params, $primary_keys);
  }
}


/*
 * Process a field-collection field's xml
 */
function lingotek_process_field_collection_xml($xml, $entity_type, &$entity, $field_name, $delta, $langcode, $node_based_translation) {
  $field_info = field_info_field($field_name);
  $curr_field_data = &$entity->$field_name;
  $default = language_default();
  $default_language = $default->language;

  if (isset($curr_field_data[LANGUAGE_NONE][$delta]['value'])) {
    $field_collection_id = $curr_field_data[LANGUAGE_NONE][$delta]['value'];
  }
  elseif (isset($curr_field_data[$default_language][$delta]['value'])) {
    $field_collection_id = $curr_field_data[$default_language][$delta]['value'];
  }
  else {
    if (!$node_based_translation) {
      // The field-collection field must be empty.
      return;
    }
    // If it does not exist and the profile is node-based, create a new FC.
    $field_collection_item = entity_create('field_collection_item', array('field_name' => $field_info['field_name']));

    // Grandfather fields (anything that's an array) from the source field collection
    $original_entity = lingotek_entity_load_single($entity_type, $entity->tnid);
    $original_field_collection = lingotek_entity_load_single('field_collection_item', $original_entity->{$field_name}[LANGUAGE_NONE][$delta]['value']);
    if (!empty($original_field_collection)) {
      foreach ($original_field_collection as $k => $v) {
        if (is_array($v)) {
          $field_collection_item->$k = $v;
        }
      }
    }

    $field_collection_item->setHostEntity($entity_type, $entity);
    $field_collection_item->save();
    $field_collection_id = $field_collection_item->item_id;
  }
  $field_collection_item = lingotek_entity_load_single('field_collection_item', $field_collection_id);
  if (!$field_collection_item) {
    // The field collection was removed, so disregard any info on it
    return;
  }
  $field_collection_item->type = $field_info['field_name'];
  $field_collection_item->language = $entity->language;

  // RECURSION FOR FIELD COLLECTIONS
  lingotek_process_entity_xml($xml, $field_collection_item, 'field_collection_item', $langcode, $node_based_translation);
}

/*
 * Save a URL Alias (nodes only)
 */
function lingotek_save_url_alias($content, $node, $drupal_langcode) {
  $target = check_plain($content);

  //URL Alias related to the page:
  $conditions = array('source' => 'node/' . $node->nid);
  if ($node->language != LANGUAGE_NONE) {
    $conditions['language'] = $node->language;
  }
  $path = path_load($conditions);
  if ($path !== FALSE) {
    $conditions['language'] = $drupal_langcode;
    if ($path['alias'] != $target || $node->language != $drupal_langcode) {
      $original = path_load($conditions);
      $conditions['alias'] = $target;
      if ($original === FALSE) {
        path_save($conditions);
      }
      else {
        path_delete($original);
        path_save($conditions);
      }
    }
  }
}

#API ADDERS

/*
 * Create a project and return it's id.
 * @param $name
 *  Project name being created
 */

function lingotek_add_project($name) {
  $output = LingotekApi::instance()->request('addProject', array('projectName' => $name));
  if ($output->results == "success") {
    variable_set('lingotek_project', $output->id);
    return $output->id;
  }
}

/*
 * Create a vault, and return it's id.
 * @param $name
 *  Vault name being created
 */

function lingotek_add_vault($name) {
  $output = LingotekApi::instance()->request('addTMVault', array('tmVaultName' => $name));
  if ($output->results == "success") {
    variable_set('lingotek_vault', $output->id);
    return $output->id;
  }
}

/**
 * Add the current vault to the current project.  It doesn't hurt to call this more than once.
 */
function lingotek_add_vault_to_project() {
  $vault_id = variable_get('lingotek_vault', '');
  $project_id = variable_get('lingotek_project', '');
  if ($vault_id != '' && $project_id != '') {
    $param = array(
      'project_id' => $project_id,
      'index_id' => $vault_id
    );
    LingotekApi::instance()->request('addProjectTMVault', $param);
  }
}

/**
 * Analyze the Project
 */
function lingotek_analyze_project() {
  LingotekApi::instance()->request("analyzeProject", array('projectId' => variable_get('lingotek_project', -1)));
}

#GETTERS

/*
 * Get available URL alias methods
 */

function lingotek_get_url_alias_translations() {
  $methods = array();
  $methods[0] = t("Don't translate");
  $methods[1] = t("Translate the URL alias");
  $methods[2] = t("Use the translated page title");
  return $methods;
}

/**
 * Get the target language objects for a Lingotek document associated with a node.
 *
 * @param int $document_id
 *   A Lingotek Document ID.
 * @param bool $flush_cache
 *   Whether or not to force a refresh from the server, as opposed to using cached data.
 *
 * @return array
 *   An array of translation target items.
 */
function lingotek_get_document_targets($document_id, $flush_cache = FALSE) {
  global $_lingotek_client;

  $targets = &drupal_static(__FUNCTION__);

  // Use static cache to ensure that we don't go to the server more than once per page for targets.
  if (isset($targets[$document_id])) {
    return $targets[$document_id];
  }

  $results = array();
  $cache_id = 'lingotek_targets_' . $document_id;
  $cache = cache_get($cache_id);
  if (lingotek_do_cache() && !$flush_cache && !empty($cache->data)) {
    LingotekLog::trace("lingotek_get_document_targets USING CACHE", array('document_id' => $document_id, 'flushCache' => $flush_cache));
    $results = $cache->data;
  }
  else {
    $output = LingotekApi::instance()->getDocument($document_id);
    if (!empty($output->translationTargets)) {
      foreach ($output->translationTargets as $target) {
        $results[$target->language] = $target;
      }
    }
    LingotekLog::trace("lingotek_get_document_targets GENERATING NEW CACHE DATA getDocument", array('document_id' => $document_id, 'flushCache' => $flush_cache));
    $targets[$document_id] = $results;
    if (!empty($results)) {
      cache_set($cache_id, $results, 'cache', time() + 900);
    }
  }

  return $results;
}

/**
 * Gets the phase name of the specified phase.
 *
 * This fetches a workflow step's name (known as a Phase in the Lingotek platform).
 *
 * @param int $phase_id
 *   A Lingotek phase ID.
 *
 * @return string
 *   Name for the workflow step (phase name).
 *
 * @todo Move the actual call to getPhase onto LingotekApi class.
 */
function lingotek_get_phase_name($phase_id) {
  $phases = &drupal_static(__FUNCTION__);

  $phase_name = '';

  if (!empty($phases[$phase_id])) {
    $phase_name = $phases[$phase_id]->name;
  }
  else {
    $params = array('phaseId' => $phase_id);
    $output = LingotekApi::instance()->request('getPhase', $params);

    if ($output->results == 'success') {
      $phases[$phase_id] = $output;
      $phase_name = $output->name;
    }
  }

  return $phase_name;
}

/**
 * Get the url to open the Lingotek Workbench.
 *
 * This fetches a link.
 *
 * @param object $node
 *   A Drupal node.
 * @param $lingotek_locale
 *   A target language.
 * @param mixed $label
 *   The label to use as text for the link. Possible values are
 *   TRUE, FALSE, or a string to use as a the custom label for the link.
 * @param bool $force
 *   Force the link to use the label of the first returned workflow phase for the target Document.
 *
 * @return string
 *   Either a link pointing the the url, or the url itself if $label is FALSE
 */
function lingotek_get_workbench_url($document_id, $lingotek_locale, $label = FALSE, $force = FALSE) {

  if ($lingotek_locale === FALSE) {
    return "";
  }

  $api = LingotekApi::instance();

  $targets = lingotek_get_document_targets($document_id, TRUE); //Make sure we get the current phases for the links and not out of date ones (so caches don't combine)

  if (count($targets) == 0) {
    return '';
  }

  foreach ($targets as $lang => $translation_target) {
    if ($lang != $lingotek_locale) {
      continue;
    }

    $target = $api->getTranslationTarget($translation_target->id);
    $phases = ($target) ? $target->phases : array();

    return lingotek_get_workbench_url_by_phases($document_id, $phases, $label, $force);
  }

  LingotekLog::error('lingotek_get_workbench_url - Specified language target not found', $document_id);

  return '';
}

/**
 * Get the url to open the Lingotek Workbench.
 *
 * This fetches a link.
 *
 * @param object $node
 *   A Drupal node.
 * @param $lingotek_locale
 *   A target language.
 * @param mixed $label
 *   The label to use as text for the link. Possible values are
 *   TRUE, FALSE, or a string to use as a the custom label for the link.
 * @param bool $force
 *   Force the link to use the label of the first returned workflow phase for the target Document.
 *
 * @return string
 *   Either a link pointing the the url, or the url itself if $label is FALSE
 */
function lingotek_get_workbench_url_by_phases($document_id, $phases, $label = FALSE, $force = FALSE) {

  $phase_id = -1;
  $which_phase = 0;
  foreach ($phases as $phase) {
    if (!$phase->isMarkedComplete || $force) {
      $phase_id = $phase->id;
      break;
    }
    $which_phase++;
  }

  // All phases are complete, use last phase as current.
  if (!empty($phases) && $phase_id == -1) {
    $last_phase = end($phases);
    $phase_id = $last_phase->id;
  }

  $l = '';
  if ($phase_id != -1) {
    if ($document_id && $workbench_url = LingotekApi::instance()->getWorkbenchLink($document_id, $phase_id)) {
      if ($label === FALSE) {
        $l = $workbench_url;
      }
      else {
        $path = $workbench_url;
        if ($label === TRUE) {
          $label = lingotek_get_phase_name($phase_id);
        }
        list($nid, $entity_type) = LingotekSync::getEntityIdFromDocId($document_id);
        $l = l($label, '', array('attributes' => array(
            'onclick' => 'window.open(\'' . $path . '\'); return false;',
            'onmouseover' => 'jQuery("#node-' . $nid . '").addClass("lingotek-highlight");',
            'onmouseout' => 'jQuery("#node-' . $nid . '").removeClass("lingotek-highlight");',
        )));
      }
    }
  }

  return $l;
}
