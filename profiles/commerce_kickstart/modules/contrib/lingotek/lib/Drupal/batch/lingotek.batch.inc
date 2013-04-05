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

  if ( isset( $nodes1[$entity_type] ) ) {
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

  if ( isset( $nodes2[$entity_type] ) ) {
    foreach ($nodes2[$entity_type] as $node2) {
      $operations[] = array('lingotek_field_language_data_cleanup_batch_worker', array($node2->nid));
    }
  }

  if (count($operations) > 0) {

    $batch = array(
      'title' => t('Lingotek Field Language Updater'),
      'operations' => $operations,
      'finished' => 'lingotek_field_language_data_cleanup_batch_finished',
      'file' => drupal_get_path('module', 'lingotek') . '/lib/Drupal/batch/lingotek.field.language.data.cleanup.inc',
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
function lingotek_sync_batch_create( ) {

  // Grab the Nodes that need to be Downloaded & Uploaded.  These are batch operation arrays.
  $download_commands = lingotek_get_sync_download_batch_elements();
  $upload_commands = lingotek_get_sync_upload_batch_elements();

  // Important: Until we add the call to get target statuses, and can work off that, do the upload SECOND. (so we dont try to download what we just uploaded.)
  $operations = array();
  $operations[] = array( 'lingotek_mt_sync_set_status', array( 'set' ) );
  $operations = array_merge( $operations, $download_commands, $upload_commands );
  $operations[] = array( 'lingotek_mt_sync_set_status', array( 'clear' ) );

  
  // Where to send the user after the batch has processed. If redirect_url GET param exists, then use it
  $redirect = (isset($_GET['redirect_url']) && strlen($_GET['redirect_url'])) ? $_GET['redirect_url'] : LINGOTEK_BASE_URL;

  if ( count( $operations ) > 0 ) {

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
function lingotek_get_sync_upload_batch_elements( ) {

  $operations = array();

  // Grab nodes that are currently marked 'EDITED'
  $query = db_select( 'lingotek', 'l' )->fields( 'l' );
  $query->condition( 'lingokey', 'node_sync_status' );
  $query->condition( 'lingovalue', LINGOTEK_NODE_SYNC_STATUS_EDITED );

  $result = $query->execute();

  while ($record = $result->fetchAssoc()) {
    $operations[] = array( 'lingotek_mt_sync_upload_node', array( $record['nid'] ) );
  }

  return $operations;

}


/**
 * Sync - Download Batch Elements:  Creates the batch elements for nodes/documents that need to be downloaded.
 */
function lingotek_get_sync_download_batch_elements( ) {

  $operations = array();

  $target_locales = lingotek_get_target_locales();
  foreach ($target_locales as $lingotek_locale) {

      $key = 'target_sync_status_' . $lingotek_locale;
      $query = db_select( 'lingotek', 'l' )->fields( 'l' );
      $query->condition( 'lingokey', $key );
      $query->condition( 'lingovalue', LINGOTEK_TARGET_SYNC_STATUS_PENDING );
    
      $result = $query->execute();
      while ( $record = $result->fetchAssoc() ) {
        $operations[] = array( 'lingotek_mt_sync_download_node_target', array( $record['nid'], $lingotek_locale ) );
      }
  } // END:  Target Languages Loop

  return $operations;

}

/**
 * Batch Create: Lingotek Identify Content - create informative lingonode data (in lingotek table) for pre-existing content 
 */
function lingotek_batch_identify_content($front = FALSE) {

  $result = db_query('SELECT DISTINCT nid FROM {lingotek}');
  $existing_nids = $result->fetchCol();
  $operations = array();

  foreach (lingotek_get_content_types(TRUE) as $type) {

    $nodes = node_load_multiple(array(), array('type' => $type));

    foreach ($nodes as $node) {
      if (!in_array($node->nid, $existing_nids)) {
        // Add content nodes to lingotek table, to indicate that they are machine translatable nodes
        $operations[] = array('lingotek_set_node_sync_status', array($node->nid, LINGOTEK_NODE_SYNC_STATUS_EDITED));
      };
    }
  }

  $batch = array(
    'title' => t('Identifying Translatable Content'),
    'operations' => $operations,
    'finished' => 'lingotek_batch_identify_content_finished'
  );

  batch_set($batch);
  if ($front) {
    batch_process('<front>'); // Needed if not inside a form _submit handler.  Setting redirect in batch_process.
  }
}











/**
 *
 * Old Upload / Download Batch Functions -- Replaced with the combined Sync batch methods.
 *
 */


/**
 * Batch Create - Upload:  Create a Bulk Machine Translate UPLOAD Batch.
 */
function lingotek_bulk_mt_upload_batch_create( ) {

  $operations = array();
  $bundles = lingotek_translatable_node_types(); // The nodes we translate.

  // Grab nodes that are currently marked 'EDITED'
  $query = db_select( 'lingotek', 'l' )->fields( 'l' );
  $query->condition( 'lingokey', 'node_sync_status' );
  $query->condition( 'lingovalue', LINGOTEK_NODE_SYNC_STATUS_EDITED );

  $result = $query->execute();

  while ($record = $result->fetchAssoc()) {
    $operations[] = array( 'lingotek_machine_translate_node', array( $record['nid'] ) );
  }

  $batch = array(
    'title' => t('Sending Content to Lingotek for Translation.'),
    'operations' => $operations,
    'finished' => 'lingotek_bulk_mt_upload_batch_finished',
    'file' => drupal_get_path('module', 'lingotek') . '/lib/Drupal/batch/lingotek.mt.batch.upload.inc',
  );

  batch_set($batch);
  batch_process('<front>'); // Needed if not inside a form _submit handler.  Setting redirect in batch_process.

} // END:  lingotek_bulk_mt_upload_batch_create()


/**
 * Batch Create - Download:  Create a Bulk Machine Translate DOWNLOAD Batch.
 */
function lingotek_bulk_mt_download_batch_create($lingotek_locale) {

  $operations = array();

  if ($lingotek_locale) {

    $drupal_language_code =  Lingotek::convertLingotek2Drupal($lingotek_locale);
    // NOTE!!!   lingotek_download_document() does its OWN drupal to lingotek lookup.  So pass in the DRUPAL language code
    // Grab nodes that are currently marked 'PENDING'
    $query = db_select('lingotek', 'l')->fields('l');
    $query->condition('lingokey', 'node_sync_status');
    $query->condition('lingovalue', LINGOTEK_TARGET_SYNC_STATUS_PENDING);

    $result = $query->execute();

    while ($record = $result->fetchAssoc()) {
      $operations[] = array('lingotek_download_node_machine_translation', array($record['nid'], $drupal_language_code));
    }

    $batch = array(
      'title' => t('Downloading Lingotek Translated Content'),
      'operations' => $operations,
      'finished' => 'lingotek_bulk_mt_download_batch_finished',
      'file' => drupal_get_path('module', 'lingotek') . '/lib/Drupal/batch/lingotek.mt.batch.download.inc',
    );

    batch_set($batch);
    batch_process('<front>'); // Needed if not inside a form _submit handler.  Setting redirect in batch_process.
  } // END:  if language_code
}


/**
 * Batch Create:  Bulk Machine Translate Upload.
 */
function lingotek_dashboard_mt_upload() {

  lingotek_bulk_mt_upload_batch_create();
}


/**
 * Batch Create:  Bulk Machine Translate Download.
 */
function lingotek_dashboard_mt_download() {

  if (isset($_GET['code'])) {

    lingotek_bulk_mt_download_batch_create($_GET['code']);
  } // If Language Code
}