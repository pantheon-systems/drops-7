<?php

/**
 * @file
 * Content translation management and sync functions.
 */
// -- Node Sync Statuses (Main Node Level) --
define('LINGOTEK_NODE_SYNC_STATUS_CURRENT', 'CURRENT'); // Node and all target languages in sync.
define('LINGOTEK_NODE_SYNC_STATUS_EDITED', 'EDITED');  // Node and one or more target languages out of sync.
define('LINGOTEK_NODE_SYNC_STATUS_PENDING', 'PENDING'); // Node 
// -- Target Sync Statuses (Node Language Level) --
define('LINGOTEK_TARGET_SYNC_STATUS_CURRENT', 'CURRENT'); // Node target language in sync.
define('LINGOTEK_TARGET_SYNC_STATUS_EDITED', 'EDITED'); // Node target language in waiting for the node to be uploaded.
define('LINGOTEK_TARGET_SYNC_STATUS_PENDING', 'PENDING'); // Node target language out of sync.  The source language has been sent for translation, but the translated version has not been downloaded.
define('LINGOTEK_TARGET_SYNC_STATUS_LOCKED', 'LOCKED');  // Node target language is locked, and will NOT be replaced.

/**
 *  Gets the Node sync status.
 * */
function lingotek_get_node_sync_status($node_id) {
  return lingotek_lingonode($node_id, 'node_sync_status');
}

/**
 *  Sets the Node sync status.
 * */
function lingotek_set_node_sync_status($node_id, $node_status) {
  return lingotek_lingonode($node_id, 'node_sync_status', $node_status);
}

/**
 *  Gets the Node Target language status.
 * */
function lingotek_get_target_sync_status($node_id, $lingotek_locale) {
  $key = 'target_sync_status_' . $lingotek_locale;
  return lingotek_lingonode($node_id, $key);
}

/**
 *  Sets the Node Target language to the specified value.
 * */
function lingotek_set_target_sync_status($node_id, $lingotek_locale, $node_status) {
  $key = 'target_sync_status_' . $lingotek_locale;
  return lingotek_lingonode($node_id, $key, $node_status);
}

// ----  Combo Functions

/**
 *  Sets the Node and Target languages to a given value.
 *  Note:  All targets get the SAME status.
 * */
function lingotek_set_node_and_targets_sync_status($node_id, $node_status, $targets_status) {

  // Set the Node to EDITED.
  lingotek_set_node_sync_status($node_id, $node_status);

  // Loop though each target language, and set that target to EDITED.
  $languages = lingotek_get_target_locales();
  foreach ($languages as $lingotek_locale) {
    lingotek_set_target_sync_status($node_id, $lingotek_locale, $targets_status);
  }
}

/**
 *  Gets the nodes current sync status array.
 * */
function lingotek_get_node_and_target_sync_status($node_id) {

  $result = array();

  $result['node_sync_status'] = lingotek_get_node_sync_status($node_id);

  // Loop though each target language, and set that target to EDITED.
  $languages = lingotek_get_target_locales();
  foreach ($languages as $lingotek_locale) {
    $result['targets'][$lingotek_locale] = lingotek_get_target_sync_status($node_id, $lingotek_locale);
  }

  /* -- This will be the sync status array format
    array (
    'node_sync_status' => 'EDITED',
    'targets' =>
    array (
    'es_ES' => 'EDITED',
    'pt_BR' => 'CURRENT',
    ),
    )
   */

  return $result;
}

/**
 *  When a new MT language is added, add a Target status tag to every node.
 * */
function lingotek_add_target_sync_status_to_all_nodes($lingotek_locale) {

  $query = db_select('lingotek', 'l')->fields('l');
  $query->condition('lingokey', 'node_sync_status');
  $result = $query->execute();

  while ($record = $result->fetchAssoc()) {
    // If the Node is CURRENT or PENDING, then we just need to pull down the new translation (because the source will have been uploaded), so set the Node and Target to PENDING.
    if ($record['lingovalue'] == LINGOTEK_NODE_SYNC_STATUS_CURRENT) {
      lingotek_set_target_sync_status($record['nid'], $lingotek_locale, LINGOTEK_TARGET_SYNC_STATUS_PENDING);
    }
    else { // Otherwise, set it to EDITED
      lingotek_set_node_sync_status($record['nid'], LINGOTEK_NODE_SYNC_STATUS_EDITED);
      lingotek_set_target_sync_status($record['nid'], $lingotek_locale, LINGOTEK_TARGET_SYNC_STATUS_EDITED);
    }
  }
}

/**
 *  When a new MT language is added, add a Target status tag to every node.
 * */
function lingotek_delete_target_sync_status_for_all_nodes($lingotek_locale) {

  $key = 'target_sync_status_' . $lingotek_locale;
  db_delete('lingotek')->condition('lingokey', $key)->execute();
}

/**
 * Returns the site callback url.
 * */
function lingotek_get_notifications_url() {
  global $base_url;
  return $base_url . '/' . LINGOTEK_NOTIFICATIONS_URL;
}

/**
 * Registers the site translation notfication callback.
 * This URL will be called when a document translation is complete, and can be downloaded.
 *
 * Format:  ?doc_id={document_id}&target_code={target_language}&project_id={project_id}
 * */
function lingotek_notifications() {

  $document_id = ( isset($_GET['doc_id']) ) ? $_GET['doc_id'] : NULL;
  $lingotek_locale = ( isset($_GET['target_code']) ) ? $_GET['target_code'] : NULL;
  $project_id = ( isset($_GET['project_id']) ) ? $_GET['project_id'] : NULL;

  if (!isset($document_id) || !isset($lingotek_locale)) {
    return drupal_json_output(array("message" => "Missing Required Parameter(s).  Required: doc_id, target_code"));
  }

  // Adding a delay in the update.  Without the delay all the different language updates hit at once, causing node lock issues as multiple languages try to update the same node at once.
  $min = 0;
  $max = 3;
  $sleep = rand($min, $max);
  sleep($sleep);

  include_once('lib/Drupal/batch/lingotek.mt.batch.sync.inc');
  $context = '';

  $target_drupal_language_code = Lingotek::convertLingotek2Drupal($lingotek_locale);
  $nid = lingotek_get_node_id_from_document_id($document_id);

  watchdog('lingotek_callback', '
      node: @node_id
      <br /><strong>doc:</strong> @document_id
      <br /><strong>code:</strong> @language_code
      <br /><strong>project:</strong> @project_id', array(
    '@node_id' => $nid,
    '@document_id' => $document_id,
    '@language_code' => $lingotek_locale,
    '@project_id' => $project_id
      ), WATCHDOG_DEBUG);

  if (!$nid) {
    // Look for and sync a comment if one is associated with the passed Lingotek Document ID.
    if (class_exists('LingotekComment')) {

      $source_language = lingotek_get_source_language(); //TO-DO: use the source_language of the comment?
      if ($comment = LingotekComment::loadByLingotekDocumentId($document_id, $source_language, $project_id)) {
        $replacements = array(
          '@id' => $comment->id,
          '@document' => $document_id,
          '@language_code' => $lingotek_locale,
          '@project_id' => $project_id,
        );

        if ($comment->updateLocalContent()) {
          if (variable_get('lingotek_api_debug', FALSE)) {
            watchdog('lingotek_debug', 'Updated local content for <strong>comment</strong> @id based on hit
                from external API for document: @document, language code @language_code, project ID: @project_id', $replacements, WATCHDOG_DEBUG);
          }
        }
        else {
          if (variable_get('lingotek_api_debug', FALSE)) {
            watchdog('lingotek_debug', 'Unable to update local content for <strong>comment</strong> @id based on hit
                from external API for document: @document, language code @language_code, project ID: @project_id', $replacements, WATCHDOG_ERROR);
          }
        }
      }
    }
    else {
      watchdog('lingotek', 'LingotekComment class not found.
          Please clear the Drupal cache to refresh the autoload registry', array(), WATCHDOG_ERROR);
    }
  }
  else {
    $node = node_load($nid);
    $source_language = $node->language;
    $node_setting = lingotek_lingonode($nid, 'sync_method');
    $auto_download = $node_setting !== FALSE ? $node_setting : variable_get('lingotek_sync', TRUE);

    if ($auto_download) { // download only when automatic download is enabled
      lingotek_mt_sync_download_node_target($nid, $lingotek_locale, $context);
    }
  }

  $found = $nid || (isset($comment) && $comment) ? TRUE : FALSE;

  //print_r($comment);
  //Response
  $response = $found ? array_merge($_GET, array(
        'target_drupal_language_code' => $target_drupal_language_code,
        'source_language' => $source_language,
        'type' => isset($comment) ? 'comment' : 'node',
        'id' => isset($comment) ? $comment->cid : $nid,
        'found' => $found,
        'download' => $found && isset($comment) ? TRUE : (isset($auto_download) && $auto_download == TRUE)
      )) : array_merge($_GET, array(
        'found' => $found
      ));

  drupal_json_output($response);
}

/**
 * Finds the Node ID when given a Document ID.  (Deprecate)
 * Returns FALSE if not found.
 * */
function lingotek_get_node_id_from_document_id($document_id) {

  $found = FALSE;
  $source_language = lingotek_get_source_language();
  $key = 'document_id';

  $query = db_select('lingotek', 'l')->fields('l');
  $query->condition('lingokey', $key);
  $query->condition('lingovalue', $document_id);
  $result = $query->execute();

  if ($record = $result->fetchAssoc()) {
    $found = $record['nid'];
  }

  return $found;
}

/**
 * The main function responsible for syncing node/document translation.
 * 
 * */
function lingotek_sync() {

  lingotek_sync_batch_create();
}
