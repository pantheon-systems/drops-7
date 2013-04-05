<?php
/**
 * @file
 * Lingotek Translation Sync Process.
 */


/**
 * Upload Batch Worker Function: Upload Node for Translation
 */
function lingotek_mt_sync_upload_node( $nid, &$context ) {

  watchdog( 'ltk_upload_node', 'node: @node_id', array( '@node_id' => $nid ), WATCHDOG_DEBUG );

  $api = LingotekApi::instance();
  $node = node_load( $nid );

  // Push this node for translation.
  if ($existing_document = lingotek_lingonode($node->nid, 'document_id')) {
    // Update an existing Lingotek Document.
    $api->updateContentDocument($node);
  }
  else {
    // Create a new Lingotek Document.
    $api->addContentDocument($node, TRUE);
  }
  
  $context['results'][] = 'Upload Node: ' . $nid;
} // END:  lingotek_machine_translate_node();


/**
 * Download Batch Worker Function: Download Translated Node Content
 */
function lingotek_mt_sync_download_node_target( $nid, $lingotek_locale, &$context ) {

  watchdog( 'ltk_download_target', 'node: @node_id (@language)', array( '@node_id' => $nid, '@language' => $lingotek_locale ), WATCHDOG_DEBUG );

  $node = node_load($nid);
  lingotek_download_document($node, $lingotek_locale);
  $context['results'][] = t('Download Target Translation: Node #@nid (@langcode)', array('@nid'=>$nid,'@langcode'=>$lingotek_locale));
}


/**
 * Sets and clears session sync flags.
 */
function lingotek_mt_sync_set_status( $status, &$context ) {

  switch ($status) {
    case 'set':
      $_SESSION['lingotek_sync_in_progress'] = 'syncing';
      break;
    case 'clear':
      if ( isset( $_SESSION['lingotek_sync_in_progress'] ) ) {
        unset($_SESSION['lingotek_sync_in_progress']);
      }
      break;
  }

}
