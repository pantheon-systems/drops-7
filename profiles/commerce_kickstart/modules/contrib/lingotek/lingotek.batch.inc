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
 * @param bool $front
 * If this batch was NOT created from a form_submit() handler, then pass in TRUE
 */
function lingotek_field_language_data_cleanup_batch_create($front = FALSE) {
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

    batch_set($batch);
    if ($front) { // If this batch was NOT created from a form_submit() handler, do this to initiate the batch.
      batch_process('<front>'); // Needed if not inside a form _submit handler.  Setting redirect in batch_process.
    }
  } // END:  if operations
}

/**
 * Batch Create - Sync:  Uploads new and changed documents for translation and Downloads translated documents.
 *
 * Creates the batch operations array.  Downloads first, then uploads.
 */
function lingotek_sync_batch_create($download_targets = array(), $upload_nids = array(), $download_targets_incomplete = array()) {

  $upload = !empty($upload_nids);
  $download = !empty($download_targets);
  $download_incomplete = !empty($download_targets_incomplete);

  // Grab the Nodes that need to be Downloaded & Uploaded.  These are batch operation arrays.
  $download_commands = $download ? lingotek_get_sync_download_batch_elements($download_targets, LingotekSync::STATUS_CURRENT) : array();
  $download_commands_inc = $download_incomplete ? lingotek_get_sync_download_batch_elements($download_targets_incomplete, LingotekSync::STATUS_PENDING) : array();
  $upload_commands = $upload ? lingotek_get_sync_upload_batch_elements($upload_nids) : array();

  $operations = array();
  $operations[] = array('lingotek_mt_sync_set_status', array('set'));
  $operations = array_merge($operations, $download_commands, $download_commands_inc, $upload_commands);
  $operations[] = array('lingotek_mt_sync_set_status', array('clear'));

  // Where to send the user after the batch has processed. If redirect_url GET param exists, then use it
  $redirect = (isset($_GET['redirect_url']) && strlen($_GET['redirect_url'])) ? $_GET['redirect_url'] : LINGOTEK_MENU_LANG_BASE_URL;

  if (count($operations) > 0) {

    // Note, the first step of the batch process sets a session variable that tracks that we are in a sync state.
    // The Node update hook uses that so it knows NOT to reupload the content we just recieved.
    // The last step of the sync process clears the sync flag.   $_SESSION['lingotek_sync_in_progress']
    // As a backup (in case there is an error and the batch doesnt complete successfully) there is a backup on the lingotek_dashboard() that clears the sync status flag.
    $batch = array(
      'title' => t('Syncing Content Translations with Lingotek'),
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

function lingotek_sync_batch_create_old() {

  // Grab the Nodes that need to be Downloaded & Uploaded.  These are batch operation arrays.
  $download_commands = lingotek_get_sync_download_batch_elements();
  $upload_commands = lingotek_get_sync_upload_batch_elements();

  // Important: Until we add the call to get target statuses, and can work off that, do the upload SECOND. (so we dont try to download what we just uploaded.)
  $operations = array();
  $operations[] = array('lingotek_mt_sync_set_status', array('set'));
  $operations = array_merge($operations, $download_commands, $upload_commands);
  $operations[] = array('lingotek_mt_sync_set_status', array('clear'));


  // Where to send the user after the batch has processed. If redirect_url GET param exists, then use it
  $redirect = (isset($_GET['redirect_url']) && strlen($_GET['redirect_url'])) ? $_GET['redirect_url'] : LINGOTEK_MENU_LANG_BASE_URL;

  if (count($operations) > 0) {

    // Note, the first step of the batch process sets a session variable that tracks that we are in a sync state.
    // The Node update hook uses that so it knows NOT to reupload the content we just recieved.
    // The last step of the sync process clears the sync flag.   $_SESSION['lingotek_sync_in_progress']
    // As a backup (in case there is an error and the batch doesnt complete successfully) there is a backup on the lingotek_dashboard() that clears the sync status flag.
    $batch = array(
      'title' => t('Syncing Content Translations with Lingotek'),
      'operations' => $operations,
      'file' => drupal_get_path('module', 'lingotek') . '/lib/Drupal/batch/lingotek.mt.batch.sync.inc',
    );

    batch_set($batch);
    batch_process($redirect); // Needed if not inside a form _submit handler.  Setting redirect in batch_process.
  }
  else {
    $options = strpos($redirect, '//') !== FALSE ? array('external' => TRUE) : array();
    drupal_goto($redirect, $options);
  }
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

      $key = 'target_sync_status_' . $lingotek_locale;
      $query = db_select('lingotek', 'l')->fields('l');
      $query->condition('lingokey', $key);
      $query->condition('lingovalue', LingotekSync::STATUS_PENDING);

      $result = $query->execute();
      while ($record = $result->fetchAssoc()) {
        $operations[] = array('lingotek_sync_download_node_target', array($record['nid'], $lingotek_locale, $sync_success_target));
      }
    }
  }
  elseif (is_array($download_targets)) {
    foreach ($download_targets as $download_target) {
      $nid = LingotekSync::getNodeIdFromDocId($download_target->document_id);
      $lingotek_locale = $download_target->locale;
      $operations[] = array('lingotek_sync_download_node_target', array($nid, $lingotek_locale, $sync_success_target));
    }
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

  foreach (lingotek_get_content_types(TRUE) as $type) {

    $nodes = node_load_multiple(array(), array('type' => $type));

    foreach ($nodes as $node) {
      if (!in_array($node->nid, $existing_nids)) {
        // Add content nodes to lingotek table, to indicate that they are machine translatable nodes
        $operations[] = array('LingotekSync::setNodeStatus', array($node->nid, LingotekSync::STATUS_EDITED));
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
 * Batch Create: Lingotek Reset Content
 */
function lingotek_batch_reset_content() {
  $doc_ids = LingotekSync::getAllLocalDocIds();
  $api = LingotekApi::instance();
  $operations = array();

  /*
    //one at a time
    foreach ($doc_ids as $doc_id) {
    $operations[] = array('lingotek_batch_reset_content_worker', array($api, $doc_id));
    }
   */

  // all at once
  $operations[] = array('lingotek_batch_reset_content_worker', array($api, $doc_ids));


  $operations[] = array('LingotekSync::disassociateAllNodes', array());
  drupal_set_message(t('All local translations have been disassociated from Lingotek.'));

  $operations[] = array('lingotek_cleanup_utility', array(FALSE)); // identify translatable content
  drupal_set_message(t('Translation management settings for all nodes have been set to the defaults.'));

  $batch = array(
    'title' => t('Resetting Translations'),
    'operations' => $operations,
  );

  batch_set($batch);
}

function lingotek_batch_reset_content_worker($api, $doc_id) {
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

  $api = LingotekApi::instance();
  $node = node_load($nid);

  // Push this node for translation.
  if ($existing_document = lingotek_lingonode($node->nid, 'document_id')) {
    // Update an existing Lingotek Document.
    $result = $api->updateContentDocument($node);
  }
  else {
    // Create a new Lingotek Document.
    $result = $api->addContentDocument($node, TRUE);
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

/**
 * Download Batch Worker Function: Download Translated Node Content
 */
function lingotek_sync_download_node_target($nid, $lingotek_locale, $sync_success_status, &$context) {

  LingotekLog::trace('download node: @node_id (@language)', array('@node_id' => $nid, '@language' => $lingotek_locale));

  $node = node_load($nid);
  lingotek_download_document($node, $lingotek_locale, $sync_success_status);
  $result = TRUE; // assume TRUE until the above lingotek_download_document call is updated to return the result

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
  //$context['results'][] = t('Download Target Translation: Node #@nid (@langcode)', array('@nid' => $nid, '@langcode' => $lingotek_locale));
}

/**
 * Sets and clears session sync flags.
 */
function lingotek_mt_sync_set_status($status, &$context) {

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

  $loaded_node = node_load($nid);
  $loaded_node->language = $source_language;
  $loaded_node->lingotek_upload_override = 0; // Set 0 : Ensure that uploading does not occur. Set 1 : Force uploading to occur
  node_save($loaded_node);
}

/**
 * Batch API processor for field data language updates.
 */
function lingotek_field_language_data_cleanup_batch_worker($nid, &$context) {

  $process_node = node_load($nid);
  if ($process_node->nid) {
    lingotek_field_language_data_cleanup_update_node($process_node->nid);
    //$context['message'] = t('Updating field data for node: @node_title', array('@node_title' => $process_node->title));
    $context['message'] = t('Preparing translatable content: @node_title', array('@node_title' => $process_node->title));
  }

  $context['finished'] = 1;
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

  $node = node_load($nid, NULL, TRUE);
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

