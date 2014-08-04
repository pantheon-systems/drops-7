<?php

/**
 * @file
 * Central location for batch create functions, before control is handed off to individual batch command files.
 */

/**
 * Field Language Data Cleanup Utility
 * 
 * Creates a batch to cleanup nodes with data in an 'und' language field.
 *
 * @param bool $autoset_batch
 * If this batch was NOT created from a form_submit() handler, then pass in TRUE
 */
function lingotek_field_language_data_cleanup_batch_create($entity_type, $autoset_batch = FALSE) {
  LingotekLog::trace(__METHOD__);
  $operations = array();
  $source_language = lingotek_get_source_language();
  $translated_types = lingotek_translatable_types($entity_type);
  $disabled_types = lingotek_get_disabled_bundles($entity_type);
  $managed_entities = lingotek_get_enabled_entities_by_type($entity_type);
  $enabled_types = array_diff($translated_types, $disabled_types);
  $info = entity_get_info($entity_type);

  // Fix node level language settings
  // This selects all the nodes that are language undefined and are content
  // types we need to translate.  We need to change these nodes from language
  // undefined to the source language.
  $query1 = new EntityFieldQuery();
  $entities1 = $query1->entityCondition('entity_type', $entity_type)
      ->propertyCondition('language', LANGUAGE_NONE, '=')
      ->execute();
  unset($query1);
  if (isset($entities1[$entity_type])) {
    foreach ($entities1[$entity_type] as $entity) {
      $entity_id = $entity->{$info['entity keys']['id']};
      if (isset($managed_entities[$entity_id])) {
        $operations[] = array('lingotek_source_language_cleanup_batch_worker', array($entity_type, $entity_id, $source_language));
      }
    }
  }

  // Fix field languages
  $query2 = new EntityFieldQuery();
  $entities2 = $query2->entityCondition('entity_type', $entity_type)
      ->execute();
  unset($query2);
  if (isset($entities2[$entity_type])) {
    foreach ($entities2[$entity_type] as $entity) {
      $entity_id = $entity->{$info['entity keys']['id']};
      if (isset($managed_entities[$entity_id])) {
        $operations[] = array('lingotek_field_language_data_cleanup_batch_worker', array($entity_type, $entity->{$info['entity keys']['id']}));
      }
    }
  }

  // Add missing URL aliases
  $query = db_select('{url_alias}', 'ua')
      ->fields('ua', array('source'))
      ->condition('ua.source', "$entity_type/%", 'LIKE')
      ->condition('ua.language', LANGUAGE_NONE)
      ->execute();
  foreach ($query->fetchCol() as $source) {
    try {
      list($et, $id) = explode('/', $source);
      if (isset($managed_entities[$id])) {
        $operations[] = array('lingotek_url_alias_source_language_cleanup_batch_worker', array($entity_type, $id, $source_language));
      }
    } catch (Exception $ex) {
      LingotekLog::trace('Found url alias that did not fit the normal pattern: @alias.  Ignoring.');
    }
  }

  $batch = array(
    'title' => t('Lingotek Field Language Updater'),
    'operations' => $operations,
    'finished' => 'lingotek_field_language_data_cleanup_batch_finished',
    'file' => 'lingotek.batch.inc'
  );

  if ($autoset_batch) { // If this batch was NOT created from a form_submit() handler, do this to initiate the batch.
    batch_set($batch);
    batch_process('<front>'); // Needed if not inside a form _submit handler.  Setting redirect in batch_process.
  }
  else {
    return $batch;
  }
}

/**
 * Batch Create - Sync:  Uploads new and changed documents for translation and Downloads translated documents.
 *
 * Creates the batch operations array.  Downloads first, then uploads.
 */
function lingotek_sync_batch_create($upload_config = array(), $download_targets = array(), $download_targets_incomplete = array(), $redirect = '', $extra_operations = array()) {

  $has_upload_config = !empty($upload_config);
  $has_download = !empty($download_targets);
  $has_download_incomplete = !empty($download_targets_incomplete);

  // Grab the Nodes that need to be Downloaded & Uploaded.  These are batch operation arrays.
  $download_commands = $has_download ? lingotek_get_sync_download_batch_elements($download_targets, LingotekSync::STATUS_CURRENT) : array();
  $download_commands_inc = $has_download_incomplete ? lingotek_get_sync_download_batch_elements($download_targets_incomplete, LingotekSync::STATUS_PENDING) : array();
  $upload_config_commands = $has_upload_config ? lingotek_get_sync_upload_config_batch_elements($upload_config) : array();

  $operations = array();
  $operations = array_merge($operations, $download_commands, $download_commands_inc, $upload_config_commands);
  $operations = array_merge($operations, $extra_operations);

  // Where to send the user after the batch has processed. If redirect_url GET param exists, then use it
  if (empty($redirect)) {
    $redirect = (isset($_GET['redirect_url']) && strlen($_GET['redirect_url'])) ? $_GET['redirect_url'] : LINGOTEK_MENU_LANG_BASE_URL;
  }

  if (count($operations) > 0) {

    $batch = array(
      'title' => t('Syncing Content and Translations'),
      'operations' => $operations,
      'file' => 'lingotek.batch.inc',
      'finished' => 'lingotek_sync_batch_finished'
    );

    batch_set($batch);
    batch_process($redirect); // Needed if not inside a form _submit handler.  Setting redirect in batch_process.
  }
  else {
    $options = strpos($redirect, '//') !== FALSE ? array('external' => TRUE) : array();
    drupal_goto($redirect, $options);
  }
}

function lingotek_sync_batch_finished($success, $results, $operations) {
  $downloads = isset($results['downloads']) ? $results['downloads'] : 0;
  $uploads = isset($results['uploads']) ? $results['uploads'] : 0;

  $message = "[Lingotek Sync] uploads:" . $uploads . ", downloads: " . $downloads;
  // $message .= empty($download_commands_inc) ? '' : " (" . count($download_commands_inc) . " incomplete translations)";
  drupal_set_message(check_plain($message));
}

/**
 * Sync - Upload Batch Elements:  Creates the batch elements for nodes/documents that need to be uploaded.
 */
function lingotek_get_sync_upload_batch_elements($entity_type, $upload_nids = array()) {

  $operations = array();

  if (is_array($upload_nids)) {
    foreach ($upload_nids as $nid) {
      $operations[] = array('lingotek_entity_upload', array($nid, $entity_type));
    }
  }

  return $operations;
}

/**
 * Sync - Upload Config Batch Elements:  Creates the batch elements for config (ie. menus, taxonomies,
 * etc.), that need to be uploaded.
 */
function lingotek_get_sync_upload_config_batch_elements($upload_configs = array()) {

  // pull outstanding imports from localize.drupal.org, if desired
  if (variable_get('lingotek_use_translation_from_drupal')) {
    module_load_include('admin.inc', 'lingotek');
    $modules = lingotek_admin_get_enabled_modules();
    lingotek_admin_load_l10n_update_batch($modules);  // sets a separate batch for processing module imports from Drupal
  }

  $operations = array();
  if (is_array($upload_configs)) {
    foreach ($upload_configs as $cid) {
      $operations[] = array('lingotek_sync_upload_config_chunk', array($cid));
    }
  }
  return $operations;
}
/**
 * Sync - Download Batch Elements:  Creates the batch elements for nodes/documents that need to be downloaded.
 * 
 * @param download_targets 
 *        list of objects (document_id, lingotek_locale)  //json_decode([ {"document_id": "191", "locale": "fr_FR" }, ... ]);
 */
function lingotek_get_sync_download_batch_elements($download_targets = NULL, $sync_success_target = LingotekSync::STATUS_CURRENT) {
  $operations = array();

  if (is_null($download_targets)) {
    $target_locales = lingotek_get_target_locales();
    foreach ($target_locales as $lingotek_locale) {

      // get all nodes that have pending translations
      $key = 'target_sync_status_' . $lingotek_locale;
      $query = db_select('{lingotek_entity_metadata}', 'l')->fields('l');
      $query->condition('entity_type', 'node');
      $query->condition('entity_key', $key);
      $status_or = db_or()
        ->condition('value', LingotekSync::STATUS_PENDING)
        ->condition('value', LingotekSync::STATUS_READY);
      $query->condition($status_or);
      $result = $query->execute();
      while ($record = $result->fetchAssoc()) {
        $operations[] = array('lingotek_entity_download', array($record['entity_id'], 'node', $lingotek_locale));
      }
      // get all config chunks that have pending translations
      $key = 'target_sync_status_' . $lingotek_locale;
      $query = db_select('{lingotek_config_metadata}', 'meta')->fields('meta');
      $query->condition('config_key', $key);
      $status_or = db_or()
        ->condition('value', LingotekSync::STATUS_PENDING)
        ->condition('value', LingotekSync::STATUS_READY);
      $query->condition($status_or);
      $result = $query->execute();
      $ran = FALSE;
      while ($record = $result->fetchAssoc()) {
        $ran = TRUE;
        $operations[] = array('lingotek_sync_download_chunk_target', array($record['id'], $lingotek_locale, $sync_success_target));
      }
      if ($ran) {
        $operations[] = array('drupal_flush_all_caches', array());
      }
    }
  }
  elseif (is_array($download_targets)) {
    $doc_ids = array();
    $updates = array();
    foreach ($download_targets as $download_target) {
      list($id, $entity_type) = LingotekSync::getEntityIdFromDocId($download_target->document_id);
      if ($id != NULL) {
        $lingotek_locale = $download_target->locale;
        $doc_ids[] = $download_target->document_id;
        $operations[] = array('lingotek_entity_download', array($id, $entity_type, $lingotek_locale));
      }
      else { // since no node was found associated with the document ID, check config chunks
        $cid = LingotekConfigChunk::getIdByDocId($download_target->document_id);
        if ($cid) {
          $lingotek_locale = $download_target->locale;
          $operations[] = array('lingotek_sync_download_chunk_target', array($cid, $lingotek_locale, $sync_success_target));
        }
      }
    }
    $doc_ids = array_unique($doc_ids);
    $operations = array_merge($updates, $operations);
  }

  return $operations;
}

/**
 * Batch Create: Lingotek Disassociate Translations
 */
function lingotek_batch_disassociate_content($remove_from_TMS) {
  $doc_ids = LingotekSync::getAllLocalDocIds();
  $api = LingotekApi::instance();
  $operations = array();

  /*
    //one at a time
    foreach ($doc_ids as $doc_id) {
    $operations[] = array('lingotek_batch_disassociate_content_worker', array($api, $doc_id));
    }
   */

  // all at once
  $operations[] = array('lingotek_batch_disassociate_content_worker', array($api, $doc_ids));


  $operations[] = array('LingotekSync::disassociateAllEntities', array());
  $operations[] = array('LingotekSync::disassociateAllChunks', array());
  drupal_set_message(t('All local translations have been disassociated from Lingotek.'));

  $batch = array(
    'title' => t('Disassociating Translations'),
    'operations' => $operations,
  );

  batch_set($batch);
  batch_process('admin/settings/lingotek/settings');
}

function lingotek_batch_disassociate_content_worker($api, $doc_id) {
  return $api->removeDocument($doc_id);
}

function lingotek_sync_upload_node_finished($success, $results, $operations) {
  if ($success) {
    $count = isset($results['uploads']) ? $results['uploads'] : 0;
    $message = format_plural($count, 'One entity uploaded to Lingotek.', '@count entities uploaded to Lingotek.');
    $message .= (isset($results['uploaded_nids']) && is_array($results['uploaded_nids'])) ? ' (' . format_plural($count, 'entity id', 'entity ids') . ': ' . implode(", ", $results['uploaded_nids']) . ')' : '';
    $message .= (isset($results['disabled'])) ? ' ' . t("<i>(Note: entities with the Disabled profile must be set to an enabled profile to be uploaded)</i>") : "";
    $status = (isset($results['disabled'])) ? 'warning' : 'status';
    drupal_set_message($message, $status);
  }
  else {
    $message = t('Finished with an error.');
    drupal_set_message($message, 'error');
  }
}

/**
 * Upload Batch Worker Function: Upload Config Chunk for Translation
 */
function lingotek_sync_upload_config_chunk($cid, &$context) {

  LingotekLog::trace('upload config chunk: @chunk_id', array('@chunk_id' => $cid));

    if ($context) {
    $context['message'] = t('Uploading configuration set @cid for translation', array('@cid' => $cid));
  }
  
  $api = LingotekApi::instance();
  $chunk = LingotekSync::getConfigChunk($cid);

  // Push this chunk for translation.
  if ($existing_document = $chunk->hasLingotekDocId()) {
    LingotekLog::trace('existing document: @existing', array('@existing' => $existing_document));
    // Update an existing Lingotek Document.
    $result = $api->updateContentDocument($chunk);
  }
  else {
    // Create a new Lingotek Document.
    $result = $api->addContentDocument($chunk, TRUE);
  }

  // WTD: This section has not been updated to accommodate results from config-chunk documents,
  // as I couldn't find where the references to $context were being used after making this call
  if ($result) {
    $context['results']['uploads'] = isset($context['results']['uploads']) && is_numeric($context['results']['uploads']) ? $context['results']['uploads'] + 1 : 1;
    if (!isset($context['results']['uploaded_cids']) || !is_array($context['results']['uploaded_cids'])) {
      $context['results']['uploaded_cids'] = array();
    }
    $context['results']['uploaded_cids'][] = $cid;
  }
  else {
    $context['results']['upload_fails'] = isset($context['results']['upload_fails']) && is_numeric($context['results']['upload_fails']) ? $context['results']['upload_fails'] + 1 : 1;
    if (!isset($context['results']['upload_fail_cids']) || !is_array($context['results']['upload_fail_cids'])) {
      $context['results']['upload_fail_cids'] = array();
    }
    $context['results']['upload_fail_cids'][] = $cid;
  }
}

function lingotek_sync_download_chunk_target($cid, $lingotek_locale, $sync_success_status, &$context) {
  
  if ($context) {
    $context['message'] = t('Downloading "@locale" translation for configuration set #@cid', array('@locale' => $lingotek_locale, '@cid' => $cid));
  }
  
  LingotekLog::trace('download chunk: @chunk_id (@language)', array('@chunk_id' => $cid, '@language' => $lingotek_locale));
  $trans_obj = LingotekConfigChunk::loadById($cid);
  if ($trans_obj) {
    $result = $trans_obj->downloadTriggered($lingotek_locale);
  }
  if ($result) {
    $context['results']['downloads'] = isset($context['results']['downloads']) && is_numeric($context['results']['downloads']) ? $context['results']['downloads'] + 1 : 1;
    if (!isset($context['results']['downloaded_chunk_targets']) || !is_array($context['results']['downloaded_chunk_targets'])) {
      $context['results']['downloaded_chunk_targets'] = array();
    }
    $context['results']['downloaded_chunk_targets'][] = array("cid" => $cid, "locale" => $lingotek_locale);
  }
  else {
    $context['results']['download_fails'] = isset($context['results']['download_fails']) && is_numeric($context['results']['download_fails']) ? $context['results']['download_fails'] + 1 : 1;
    if (!isset($context['results']['download_fail_chunk_targets']) || !is_array($context['results']['download_fail_chunk_targets'])) {
      $context['results']['download_fail_chunk_targets'] = array();
    }
    $context['results']['download_fail_chunk_targets'][] = array("cid" => $cid, "locale" => $lingotek_locale);
  }
}

function lingotek_sync_download_target_finished($success, $results, $operations) {
  if ($success) {
    $count = isset($results['downloads']) ? $results['downloads'] : 0;
    $message = format_plural($count, 'One translation downloaded.', '@count translations downloaded.');
  }
  else {
    $message = t('Finished with an error.');
  }
  drupal_set_message($message);
}

/////// FIELD CLEAN-UP

/**
 * Functions for the Batch:  lingotek_field_language_data_cleanup_batch_create()
 */

/**
 * Batch API worker for changing the entity's language setting.
 */
function lingotek_source_language_cleanup_batch_worker($entity_type, $entity_id, $source_language, &$context) {

  $context['message'] = t("Setting language for @entity_type #@id to '@source_language'", array('@entity_type' => $entity_type, '@id' => $entity_id, '@source_language' => $source_language));

  $loaded_entity = lingotek_entity_load_single($entity_type, $entity_id);
  $info = entity_get_info($entity_type);
  // Default to string 'language' if no language-related entity key is found.
  $language_field = !empty($info['entity keys']['language']) ? $info['entity keys']['language'] : 'language';
  if (isset($loaded_entity->$language_field) && $loaded_entity->$language_field != $source_language) {
    $loaded_entity->$language_field = $source_language;
    $loaded_entity->lingotek_upload_override = FALSE; // Set 0 : Ensure that uploading does not occur. Set 1 : Force uploading to occur
    entity_save($entity_type, $loaded_entity);
    if (!isset($context['results']['entity_cleanup'])) {
      $context['results']['entity_cleanup'] = 0;
    }
    $context['results']['entity_cleanup'] ++;
  }
}

/**
 * Batch API worker for changing the url alias language setting
 */
function lingotek_url_alias_source_language_cleanup_batch_worker($entity_type, $id, $source_language, &$context) {

  $conditions = array('source' => $entity_type . '/' . $id);
  $conditions['language'] = $source_language;
  $source_alias = path_load($conditions);

  if (!isset($context['results']['url_alias_cleanup'])) {
    $context['results']['url_alias_cleanup'] = array('searched' => 0, 'added' => 0);
  }
  $context['results']['url_alias_cleanup']['searched'] ++;

  if ($source_alias === FALSE) { // if no url alias exists for this entity in the source language
    $conditions['language'] = LANGUAGE_NONE;
    $und_alias = path_load($conditions);

    if ($und_alias !== FALSE) { // if a url alias exists for language none (
      $context['message'] = t("Saving language-neutral path alias for @entity_type #@id to language '@sl'", array('@entity_type' => $entity_type, '@nid' => $id, '@sl' => $source_language));
      $conditions['language'] = $source_language;
      $conditions['alias'] = $und_alias['alias'];
      path_save($conditions);
      $context['results']['url_alias_cleanup']['added'] ++;
    }
  }
}

/**
 * Batch API processor for field data language updates.
 */
function lingotek_field_language_data_cleanup_batch_worker($entity_type, $entity_id, &$context) {

  $result = lingotek_field_language_data_cleanup_update_entity($entity_type, $entity_id, $context);

  if (!isset($context['results']['field_cleanup'])) {
    $context['results']['field_cleanup'] = 0;
  }
  if ($result) {
    $context['results']['field_cleanup'] ++;
  }
  $context['finished'] = 1;
}

/**
 * Ensures correct language-specific field data for the specified item.
 * 
 * Logic: Look at each translatable_field (Any field marked for lingotek 
 * translation management) for the given node.  If the field has data in the
 * language 'und' area, and is empty in the language area that this node is,
 * copy the data over.  So if this node is marked as English, but there is no
 * data in the English language spots, but there IS in the 'und' spots, move
 * the data to the English locations.
 *
 * @param int $nid
 *   The node ID of the item to be updated.
 *
 * @return bool
 *   TRUE if specified node's field data was updated. FALSE if no changes made.
 */
function lingotek_field_language_data_cleanup_update_entity($entity_type, $entity_id, &$context) {

  $edited = FALSE;

  $entity = lingotek_entity_load_single($entity_type, $entity_id);

  if (!$entity) {
    LingotekLog::error('Attempted to update field data for a non-existent @entity_type: @entity_id', array('@entity_type' => $entity_type, '@entity_id' => $entity_id));
    return $edited;
  }
  $info = entity_get_info($entity_type);
  $label_field = $info['entity keys']['label'];
  // Use 'language' as a fallback in case the entity doesn't give lang field.
  $language_field = isset($info['entity keys']['language']) ? $info['entity keys']['language'] : 'language';
  $context['message'] = t('Preparing translatable content for @entity_type: @entity_title', array('@entity_type' => $entity_type, '@entity_title' => $entity->$label_field));
  if ($entity->$language_field != 'und') {
    $translatable_fields = lingotek_translatable_fields();
    foreach ($translatable_fields as $field_name) {
      if (!empty($entity->{$field_name}['und']) && empty($entity->{$field_name}[$entity->{$language_field}])) {
        $entity->{$field_name}[$entity->{$language_field}] = $entity->{$field_name}['und'];
        $edited = TRUE;
      }
    }
  }

  if ($edited) {
    $entity->lingotek_upload_override = FALSE;
    entity_save($entity_type, $entity);
  }

  return $edited;
}

/**
 * FINISHED CALLBACK:  lingotek_field_language_data_cleanup_batch_create()
 */
function lingotek_field_language_data_cleanup_batch_finished($success, $results, $operations) {
  if ($success) {
    $def_lang = language_default('name');
    if (isset($results['entity_cleanup'])) {
      $num_nodes = (int) $results['entity_cleanup'];
      drupal_set_message(format_plural($num_nodes, t('Converted @count entities from language neutral to @language', array('@count' => $num_nodes, '@language' => $def_lang)), t('Converted @count entities from language neutral to @language', array('@count' => $num_nodes, '@language' => $def_lang))));
    }
    if (isset($results['url_alias_cleanup'])) {
      $searched = (int) $results['url_alias_cleanup']['searched'];
      $added = (int) $results['url_alias_cleanup']['added'];
      drupal_set_message(format_plural($searched, t('Searched @search_count entity for url aliases, added @add_count in @language.', array('@search_count' => $searched, '@add_count' => $added, '@language' => $def_lang)), t('Searched @search_count entities for url aliases, added @add_count in @language.', array('@search_count' => $searched, '@add_count' => $added, '@language' => $def_lang))));
    }
    if (isset($results['field_cleanup'])) {
      $num_nodes = (int) $results['field_cleanup'];
      drupal_set_message(format_plural($num_nodes, t('Added language-specific fields for @count entity with language-neutral fields.', array('@count' => $num_nodes)), t('Added language-specific fields for @count entities with language-neutral fields.', array('@count' => $num_nodes))));
    }
    else {
      drupal_set_message(t('No requested entities currently require any preparation.'), 'status', FALSE);
    }
  }
}

function lingotek_update_target_progress_batch_create($entity_type, $nids) {
  $document_ids = LingotekSync::getDocIdsFromEntityIds($entity_type, $nids);
  $batch = array(
    'title' => t('Syncing Translation Progress with Lingotek'),
  );
  $operations = array();

  $segment = array();
  $offset = 0;
  $offset_interval = 5;
  do {
    $segment = array_slice($document_ids, $offset, $offset_interval, TRUE);
    if (!empty($segment)) {
      $operations[] = array('lingotek_get_and_update_target_progress', array($entity_type, $segment));
    }
    $offset += $offset_interval;
  } while (count($segment) >= $offset_interval);

  $batch['operations'] = $operations;
  $redirect = 'admin/settings/lingotek/manage/' . $entity_type;

  batch_set($batch);
  batch_process($redirect);
}

/**
 * Batch worker function for lingotek_keystore operations
 */
function lingotek_batch_keystore_worker($entity_type, $entity_id, $key, $value, $update_on_dup, &$context) {
  $result = lingotek_keystore($entity_type, $entity_id, $key, $value, $update_on_dup);
  $context['message'] = t('Updating tracking information for @et #@eid', array('@et' => $entity_type, '@eid' => $entity_id));
  $context['results'][] = $result;
}
