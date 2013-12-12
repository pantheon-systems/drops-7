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
function lingotek_field_language_data_cleanup_batch_create($autoset_batch = FALSE) {
  LingotekLog::trace(__METHOD__);
  $operations = array();
  $entity_type = 'node';
  $source_language = lingotek_get_source_language();
  $translated_types = lingotek_translatable_node_types(); // Get the list of content types that we translate.
  // Fix node level language settings
  // This selects all the nodes that are language undefined and are content types we need to translate.  We need to change these nodes from language undefined to the source language.
  $query1 = new EntityFieldQuery();
  $nodes1 = $query1->entityCondition('entity_type', $entity_type)
      ->entityCondition('bundle', $translated_types)
      ->propertyCondition('language', 'und', '=')
      ->execute();

  if (isset($nodes1[$entity_type])) {
    foreach ($nodes1[$entity_type] as $node1) {
      $operations[] = array('lingotek_node_source_language_cleanup_batch_worker', array($node1->nid, $source_language));
      $operations[] = array('lingotek_node_url_alias_source_language_cleanup_batch_worker', array($node1->nid, $source_language));
      $operations[] = array('lingotek_field_language_data_cleanup_batch_worker', array($node1->nid));
    }
  }


  // Fix field languages
  // This selects all nodes that have a language defined.  It does NOT select the UND language nodes.
  $query2 = new EntityFieldQuery();
  $nodes2 = $query2->entityCondition('entity_type', $entity_type)
      ->propertyCondition('language', 'und', '<>')
      ->execute();

  if (isset($nodes2[$entity_type])) {
    foreach ($nodes2[$entity_type] as $node2) {
      $operations[] = array('lingotek_node_url_alias_source_language_cleanup_batch_worker', array($node2->nid, $source_language));
      $operations[] = array('lingotek_field_language_data_cleanup_batch_worker', array($node2->nid));
    }
  }

  if (count($operations) > 0) {

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
  } // END:  if operations
}

/**
 * Batch Create - Sync:  Uploads new and changed documents for translation and Downloads translated documents.
 *
 * Creates the batch operations array.  Downloads first, then uploads.
 */
function lingotek_sync_batch_create($upload_nids = array(), $upload_config = array(), $download_targets = array(), $download_targets_incomplete = array(), $redirect = '', $extra_operations = array()) {

  $has_upload = !empty($upload_nids);
  $has_upload_config = !empty($upload_config);
  $has_download = !empty($download_targets);
  $has_download_incomplete = !empty($download_targets_incomplete);

  // Grab the Nodes that need to be Downloaded & Uploaded.  These are batch operation arrays.
  $download_commands = $has_download ? lingotek_get_sync_download_batch_elements($download_targets, LingotekSync::STATUS_CURRENT) : array();
  $download_commands_inc = $has_download_incomplete ? lingotek_get_sync_download_batch_elements($download_targets_incomplete, LingotekSync::STATUS_PENDING) : array();
  $upload_commands = $has_upload ? lingotek_get_sync_upload_batch_elements($upload_nids) : array();
  $upload_config_commands = $has_upload_config ? lingotek_get_sync_upload_config_batch_elements($upload_config) : array();

  $operations = array();
  $operations = array_merge($operations, $download_commands, $download_commands_inc, $upload_commands, $upload_config_commands);
  $operations = array_merge($operations, $extra_operations);

  // Where to send the user after the batch has processed. If redirect_url GET param exists, then use it
  if (empty($redirect)) {
    $redirect = (isset($_GET['redirect_url']) && strlen($_GET['redirect_url'])) ? $_GET['redirect_url'] : LINGOTEK_MENU_LANG_BASE_URL;
  }

  if (count($operations) > 0) {

    // Note, the first step of the batch process sets a session variable that tracks that we are in a sync state.
    // The Node update hook uses that so it knows NOT to reupload the content we just recieved.
    // The last step of the sync process clears the sync flag.   $_SESSION['lingotek_sync_in_progress']
    // As a backup (in case there is an error and the batch doesnt complete successfully) there is a backup on the lingotek_dashboard() that clears the sync status flag.
    $batch = array(
      'title' => t('Syncing Content and Translations'),
      'operations' => $operations,
      'file' => 'lingotek.batch.inc',
      'finished' => 'lingotek_sync_batch_finished'
    );

    batch_set($batch);
    lingotek_mt_sync_set_status('set'); // this status unsets itself when the dashboard is loaded on redirect
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
function lingotek_get_sync_upload_batch_elements($upload_nids = array()) {

  $operations = array();

  if (is_null($upload_nids)) { //deprecated
    // Grab nodes that are currently marked 'EDITED'
    $query = db_select('lingotek', 'l')->fields('l');
    $query->condition('lingokey', 'node_sync_status');
    $query->condition('lingovalue', LingotekSync::STATUS_EDITED);

    $result = $query->execute();

    while ($record = $result->fetchAssoc()) {
      $operations[] = array('lingotek_sync_upload_node', array($record['nid']));
    }
  }
  elseif (is_array($upload_nids)) {
    foreach ($upload_nids as $nid) {
      $operations[] = array('lingotek_sync_upload_node', array($nid));
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
      $query = db_select('lingotek', 'l')->fields('l');
      $query->condition('lingokey', $key);
      $status_or = db_or()
        ->condition('lingovalue', LingotekSync::STATUS_PENDING)
        ->condition('lingovalue', LingotekSync::STATUS_READY);
      $query->condition($status_or);
      $result = $query->execute();
      while ($record = $result->fetchAssoc()) {
        $operations[] = array('lingotek_sync_download_node_target', array($record['nid'], $lingotek_locale, $sync_success_target));
      }
      // get all config chunks that have pending translations
      $key = 'target_sync_status_' . $lingotek_locale;
      $query = db_select('lingotek_config_metadata', 'meta')->fields('meta');
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
      $nid = LingotekSync::getNodeIdFromDocId($download_target->document_id);
      if ($nid != NULL) {
        $lingotek_locale = $download_target->locale;
        $doc_ids[] = $download_target->document_id;
        $operations[] = array('lingotek_sync_download_node_target', array($nid, $lingotek_locale, $sync_success_target, FALSE));
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
    foreach ($doc_ids as $doc_id) {
      //$updates[] = array('lingotek_get_and_update_target_progress', array($doc_ids));
    }
    $operations = array_merge($updates, $operations);
  }

  return $operations;
}

/**
 * Batch Create: Lingotek Identify Content - create informative lingonode data (in lingotek table) for pre-existing content 
 */
function lingotek_batch_identify_content($front = FALSE) {
  LingotekLog::trace(__METHOD__);
  $result = db_query('SELECT DISTINCT nid FROM {lingotek}');
  $existing_nids = $result->fetchCol();
  $operations = array();

  global $language;
  $existing_languages = language_list();
  unset($existing_languages[$language->language]);

  foreach (lingotek_get_content_types() as $type) {

    $nodes = node_load_multiple(array(), array('type' => $type));

    $nodes_in_language_field = array();
    $nodes_in_language_node = array();

    foreach ($existing_languages as $lang) {
      $nodes_in_language_field[$lang->language] = lingotek_list_entities_with_language('node', $type, $lang->language);
      $nodes_in_language_node[$lang->language] = lingotek_list_nodes_translated_in_language($type, $lang->language);
    }

    foreach ($nodes as $node) {
      if (!in_array($node->nid, $existing_nids)) {
        if ($node->tnid == 0 || $node->tnid == $node->nid) { //skip node translation target nodes
          // Add content nodes to lingotek table, to indicate that they are machine translatable nodes
          $operations[] = array('LingotekSync::setNodeStatus', array($node->nid, LingotekSync::STATUS_EDITED));

          foreach ($existing_languages as $lang) {
            if (in_array($node->nid, $nodes_in_language_field[$lang->language]) || in_array($node->nid, $nodes_in_language_node[$lang->language])) {
              $lingotek_locale = Lingotek::convertDrupal2Lingotek($lang->language);
              lingotek_lingonode($node->nid, 'target_sync_status_' . $lingotek_locale, LingotekSync::STATUS_CURRENT);
            }
          }
        }
      };
    }
  }

  $batch = array(
    'title' => t('Identifying Translatable Content'),
    'operations' => $operations,
      //'finished' => 'lingotek_batch_identify_content_finished'
  );

  batch_set($batch);
  if ($front) {
    batch_process('<front>'); // Needed if not inside a form _submit handler.  Setting redirect in batch_process.
  }
}

/**
 * Batch Create: Lingotek Disassociate Translations
 */
function lingotek_batch_disassociate_content() {
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


  $operations[] = array('LingotekSync::disassociateAllNodes', array());
  $operations[] = array('LingotekSync::disassociateAllEntities', array());
  $operations[] = array('LingotekSync::disassociateAllChunks', array());
  drupal_set_message(t('All local translations have been disassociated from Lingotek.'));

  $operations[] = array('lingotek_cleanup_utility', array(FALSE, FALSE)); // identify translatable content
  drupal_set_message(t('Translation management settings for all nodes have been set to the defaults.'));

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

/**
 * Lingotek Translation Sync Process.
 */

/**
 * Upload Batch Worker Function: Upload Node for Translation
 */
function lingotek_sync_upload_node($nid, &$context) {

  LingotekLog::trace('upload node: @node_id', array('@node_id' => $nid));

  if ($context) {
    $context['message'] = t('Uploading node @nid for translation', array('@nid' => $nid));
  }
  
  $api = LingotekApi::instance();
  $node = lingotek_node_load_default($nid);

  if ($node->lingotek['profile'] == LingotekSync::PROFILE_DISABLED || $node->lingotek['node_sync_status'] == LingotekSync::STATUS_DISABLED) {
    return;
  }

  // clear any node caching by entitycache module
  if (module_exists('entitycache')) {
    cache_clear_all($nid, 'cache_entity_node');
  }

  // Push this node for translation.
  if ($existing_document = isset($node->lingotek['document_id']) ? $node->lingotek['document_id'] : NULL) {
    // Update an existing Lingotek Document.
    $result = $api->updateContentDocument($node);
  }
  else {
    // Create a new Lingotek Document.
    $ln = LingotekNode::load($node);
    lingotek_node_save_readonly($node);
    $result = $api->addContentDocument($ln, TRUE);
  }

  if ($result) {
    $context['results']['uploads'] = isset($context['results']['uploads']) && is_numeric($context['results']['uploads']) ? $context['results']['uploads'] + 1 : 1;
    if (!isset($context['results']['uploaded_nids']) || !is_array($context['results']['uploaded_nids'])) {
      $context['results']['uploaded_nids'] = array();
    }
    $context['results']['uploaded_nids'][] = $nid;
  }
  else {
    $context['results']['upload_fails'] = isset($context['results']['upload_fails']) && is_numeric($context['results']['upload_fails']) ? $context['results']['upload_fails'] + 1 : 1;
    if (!isset($context['results']['upload_fail_nids']) || !is_array($context['results']['upload_fail_nids'])) {
      $context['results']['upload_fail_nids'] = array();
    }
    $context['results']['upload_fail_nids'][] = $nid;
  }
  //$context['results'][] = t('Uploaded: Node #@nid', array('@nid' => $nid));
}

function lingotek_sync_upload_node_finished($success, $results, $operations) {
  if ($success) {
    $count = isset($results['uploads']) ? $results['uploads'] : 0;
    $message = format_plural($count, 'One node uploaded to Lingotek.', '@count nodes uploaded to Lingotek.');
    $message .= (isset($results['uploaded_nids']) && is_array($results['uploaded_nids'])) ? ' (' . format_plural($count, 'node id', 'node ids') . ': ' . implode(", ", $results['uploaded_nids']) . ')' : '';
  }
  else {
    $message = t('Finished with an error.');
  }
  drupal_set_message($message);
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

/**
 * Download Batch Worker Function: Download Translated Node Content
 */
function lingotek_sync_download_node_target($nid, $lingotek_locale, $sync_success_status, $update_progress = TRUE, &$context) {

  if ($context) {
    $context['message'] = t('Downloading "@locale" translation for node @nid', array('@locale' => $lingotek_locale, '@nid' => $nid));
  }
  
  $node = lingotek_node_load_default($nid);
  if ($node->language == Lingotek::convertLingotek2Drupal($lingotek_locale)) {
    return;
  }
  LingotekLog::trace('download node: @node_id (@language)', array('@node_id' => $nid, '@language' => $lingotek_locale));

  $status = LingotekSync::getTargetStatus(LingotekSync::getDocIdFromNodeId($nid), $lingotek_locale);
  if ($status == LingotekSync::STATUS_READY || $status == LingotekSync::STATUS_CURRENT) { // Overide sync_success_status variable because statuses will be current for all downloads
    $sync_success_status = LingotekSync::STATUS_CURRENT;
  }
  else {
    $sync_success_status = LingotekSync::STATUS_PENDING;
  }

  if ($sync_success_status != LingotekSync::STATUS_CURRENT && $update_progress) {
    //lingotek_get_and_update_target_progress(LingotekSync::getDocIdFromNodeId($nid));
  }
  elseif ($sync_success_status == LingotekSync::STATUS_CURRENT && $update_progress) {
    lingotek_lingonode($nid, 'target_sync_progress_' . $lingotek_locale, 100);
    lingotek_lingonode($nid, 'target_sync_last_progress_updated_' . $lingotek_locale, time());
  }

  $result = lingotek_download_document($node, $lingotek_locale, $sync_success_status);
  cache_clear_all('field:node:' . $nid, 'cache_field');

  if ($result) {
    $context['results']['downloads'] = isset($context['results']['downloads']) && is_numeric($context['results']['downloads']) ? $context['results']['downloads'] + 1 : 1;
    if (!isset($context['results']['downloaded_node_targets']) || !is_array($context['results']['downloaded_node_targets'])) {
      $context['results']['downloaded_node_targets'] = array();
    }
    $context['results']['downloaded_node_targets'][] = array("nid" => $nid, "locale" => $lingotek_locale);
  }
  else {
    $context['results']['download_fails'] = isset($context['results']['download_fails']) && is_numeric($context['results']['download_fails']) ? $context['results']['download_fails'] + 1 : 1;
    if (!isset($context['results']['download_fail_node_targets']) || !is_array($context['results']['download_fail_node_targets'])) {
      $context['results']['download_fail_node_targets'] = array();
    }
    $context['results']['download_fail_node_targets'][] = array("nid" => $nid, "locale" => $lingotek_locale);
  }
}

function lingotek_sync_download_chunk_target($cid, $lingotek_locale, $sync_success_status, &$context) {
  
  if ($context) {
    $context['message'] = t('Downloading "@locale" translation for configuration set #@cid', array('@locale' => $lingotek_locale, '@cid' => $cid));
  }
  
  LingotekLog::trace('download chunk: @chunk_id (@language)', array('@chunk_id' => $cid, '@language' => $lingotek_locale));
  $trans_obj = LingotekConfigChunk::loadById($cid);
  if ($trans_obj) {
    $result = $trans_obj->updateLocalContentByTarget($lingotek_locale);
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

/**
 * Sets and clears session sync flags.
 */
function lingotek_mt_sync_set_status($status, &$context=NULL) {

  switch ($status) {
    case 'set':
      $_SESSION['lingotek_sync_in_progress'] = 'syncing';
      break;
    case 'clear':
      if (isset($_SESSION['lingotek_sync_in_progress'])) {
        unset($_SESSION['lingotek_sync_in_progress']);
      }
      break;
  }
}

/////// FIELD CLEAN-UP

/**
 * Functions for the Batch:  lingotek_field_language_data_cleanup_batch_create()
 */

/**
 * Batch API worker for changing the node language setting.
 */
function lingotek_node_source_language_cleanup_batch_worker($nid, $source_language) {

  $loaded_node = lingotek_node_load_default($nid);
  $loaded_node->language = $source_language;
  $loaded_node->lingotek_upload_override = 0; // Set 0 : Ensure that uploading does not occur. Set 1 : Force uploading to occur
  node_save($loaded_node);
}

/**
 * Batch API worker for changing the url alias language setting
 */
function lingotek_node_url_alias_source_language_cleanup_batch_worker($nid, $source_language) {

  $conditions = array('source' => 'node/' . $nid);
  $conditions['language'] = $source_language;
  $source_alias = path_load($conditions);
  if ($source_alias === FALSE) { // if no url alias exists for this node in the source language
    $conditions['language'] = LANGUAGE_NONE;
    $und_alias = path_load($conditions);
    if ($und_alias !== FALSE) { // if a url alias exists for language none (
      $conditions['language'] = $source_language;
      $conditions['alias'] = $und_alias['alias'];
      path_save($conditions);
    }
  }
}

/**
 * Batch API processor for field data language updates.
 */
function lingotek_field_language_data_cleanup_batch_worker($nid, &$context) {

  $process_node = lingotek_node_load_default($nid);
  if ($process_node->nid) {
    lingotek_field_language_data_cleanup_update_node($process_node->nid);
    //$context['message'] = t('Updating field data for node: @node_title', array('@node_title' => $process_node->title));
    $context['message'] = t('Preparing translatable content: @node_title', array('@node_title' => $process_node->title));
  }

  $context['finished'] = 1;
}

/*
 * Batch worker for workbench moderation moderating post-download
 *
 * @param int $nid
 *    Node ID of node to be moderated
 *
 * @param int $options_index
 *    Index for array returned by lingotek_get_workbench_moderation_options()
 *    Specifies the state to which we should moderate
 *
 * @param array $trans_options
 *    Associative array where $key => $value is $transitions_from => $transitions_to
 *    All transitions using lingotek defaults
 */
function lingotek_workbench_moderation_moderate($nid, $options_index, $trans_options = array()) {
  $options = lingotek_get_workbench_moderation_options();
  $option = $options[$options_index];
  $node = lingotek_node_load_default($nid);
  $target_statuses = LingotekSync::getAllTargetStatusNotCurrent($nid);
  if (empty($target_statuses)) { // if not all of the targets have been translated and downloaded
    // if workbench_moderation is not enabled for this node
    if (module_exists('workbench_moderation') && isset($node->workbench_moderation)) {
      if ($options_index == 'no_moderation') {
        return;
      }
      elseif ($options_index == 'increment') {
        $transitions_from = $node->workbench_moderation['current']->state;
        $transitions_to = $trans_options[$transitions_from];
        workbench_moderation_moderate($node, $transitions_to);
      }
      else {
        $states = lingotek_get_workbench_moderation_states();
        $index = 0;
        foreach ($states as $state) {
          if ($index == $options_index) {
            workbench_moderation_moderate($node, $state);
            return;
          }
          $index++;
        }
      }
    }
  }
  return;
}

/**
 * Ensures correct language-specific field data for the specified item.
 * 
 * Logic: Look at each translatable_node_field (Any field marked for lingotek translation management) for the given node.
 * If the field has data in the language 'und' area, and is empty in the language area that this node is, copy the data over.
 * So if this node is marked as English, but there is no data in the English language spots, but there IS in the 'und' spots, move the data to the English locations.
 *
 * @param int $nid
 *   The node ID of the item to be updated.
 *
 * @return bool
 *   TRUE if the specified node's field data was updated. FALSE if no changes were made.
 */
function lingotek_field_language_data_cleanup_update_node($nid) {
  $edited = FALSE;

  $node = lingotek_node_load_default($nid, NULL, TRUE);
  if ($node->nid) {
    if ($node->language != 'und') {
      $translatable_fields = lingotek_translatable_node_fields();
      foreach ($translatable_fields as $field_name) {
        if (!empty($node->{$field_name}['und']) && empty($node->{$field_name}[$node->language])) {
          $node->{$field_name}[$node->language] = $node->{$field_name}['und'];
          $edited = TRUE;
        }
      }
    }

    if ($edited) {
      $node->lingotek_upload_override = 0;
      node_save($node);
    }
  }
  else {
    LingotekLog::error('Attempted to update field data for a non-existent node: @node_id', array('@node_id' => $node->nid));
  }

  return $edited;
}

/**
 * FINISHED CALLBACK:  lingotek_field_language_data_cleanup_batch_create()
 */
function lingotek_field_language_data_cleanup_batch_finished($success, $results, $operations) {
  
}

function lingotek_update_target_progress_batch_create($nids) {
  $document_ids = LingotekSync::getDocIdsFromNodeIds($nids);
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
      $operations[] = array('lingotek_get_and_update_target_progress', array($segment));
    }
    $offset += $offset_interval;
  } while (count($segment) >= $offset_interval);

  $batch['operations'] = $operations;
  $redirect = 'admin/settings/lingotek/manage/list';

  batch_set($batch);
  batch_process($redirect);
}
