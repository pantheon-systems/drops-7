<?php
/**
 * @file
 * Lingotek Batch Translate Process.
 */


/**
 * Batch Worker Function: Machine Translate a Node.
 */
function lingotek_machine_translate_node( $nid, &$context ) {

  watchdog( 'ltk_upload', 'nide: @node_id', array( '@node_id' => $nid ), WATCHDOG_DEBUG );

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
 * Batch Finished: Bulk Machine Translation Complete.
 */
function lingotek_bulk_mt_upload_batch_finished( $success, $results, $operations ) {

  $messages = array();
  $result_count = count( $results );

  if ( $success ) { // The 'success' (TRUE/FALSE) means no fatal PHP errors were detected. All other error management should be handled using 'results'.
    $messages[] = t( 'The process finished successfully.' );
    $messages[] = format_plural( $result_count, 'One node has been queued for translation.', '@count nodes have been queued for translation.' );
  }
  else {
    $messages[] = t( 'The process finished with an errors.  Some content may have been missed.' );
    $messages[] = format_plural( $result_count, 'One node queued for translation.', '@count nodes queued for translation.' );
  }

  $_SESSION['bulk_mt_batch_results'] = $messages; // Save data before redirect.

  drupal_goto(LINGOTEK_BASE_URL . '/mt-content-queued');

} // END:  lingotek_bulk_mt_batch_finished()


/**
 * Dashboard Confirmation Screen: Machine Translation Complete
 * Users are directed here after all their content has been queued for translation.
 */
function lingotek_bulk_mt_upload_batch_complete() {

  $form = array();

  $messages = $_SESSION['bulk_mt_batch_results'];

  $form['lingotek_message_1'] = array( '#markup' => '<h3>Your content has been queued for translation.</h3>' );
  $form['lingotek_message_2'] = array( '#markup' => '<div>&nbsp;</div>' );
  $form['lingotek_message_3'] = array( '#markup' => '<div><strong>Details:</strong></div>' );

  $form['lingotek_message_4'] = array( '#markup' => '<blockquote>' );

  $cnt = 1;
  foreach ( $messages as $message ) {
    $form['lingotek_message_details_' . $cnt] = array( '#markup' => '<div>' . $message . '</div>' );
    $cnt++;
  }

  $form['lingotek_message_5'] = array( '#markup' => '</blockquote>' );

  $form['lingotek_button_spacer'] = array( '#markup' => '<div>&nbsp;</div>' );
  $form[ 'submit' ] = array(
    '#type' => 'submit', 
    '#value' => t( 'Continue' )
  );

  return $form;

} // END:  lingotek_bulk_mt_batch_complete()


/**
 * Dashboard: Machine Translation Complete - Form Submit - Redirects Back to the Lingotek Dashboard.
 */
function lingotek_bulk_mt_upload_batch_complete_submit( $form, $form_state ) {
  drupal_goto(LINGOTEK_BASE_URL);
} // END:  lingotek_bulk_mt_batch_complete_submit
