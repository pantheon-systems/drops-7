<?php
/**
 * @file
 * Lingotek Batch Translate Process.
 */


/**
 * Batch Worker Function: Download a Machine Translation
 */
function lingotek_download_node_machine_translation( $nid, $language_code, &$context ) {
  $node = node_load($nid);
  lingotek_download_document($node, $language_code);
  $context['results'][] = 'Download Translated Node Content: ' . $nid;
} // END:  lingotek_machine_translate_node();


/**
 * Batch Finished: Bulk Machine Translation Complete.
 */
function lingotek_bulk_mt_download_batch_finished( $success, $results, $operations ) {

  $messages = array();
  $result_count = count( $results );

  if ( $success ) { // The 'success' (TRUE/FALSE) means no fatal PHP errors were detected. All other error management should be handled using 'results'.
    $messages[] = t( 'The process finished successfully.' );
    $messages[] = format_plural( $result_count, 'One translated node has been downloaded.', '@count translated nodes have been downloaded.' );
  }
  else {
    $messages[] = t( 'The process finished with an errors.  Some content may have been missed.' );
    $messages[] = format_plural( $result_count, 'One translated node has been downloaded.', '@count translated nodes have been downloaded.' );
  }

  $_SESSION['bulk_mt_batch_download_results'] = $messages; // Save data before redirect.

  drupal_goto(LINGOTEK_BASE_URL . '/mt-content-downloaded');

} // END:  lingotek_bulk_mt_batch_finished()


/**
 * Dashboard Confirmation Screen: Machine Translation Download Complete
 * Users are directed here after all their translated content has been downloaded.
 */
function lingotek_bulk_mt_download_batch_complete() {

  $form = array();
  $messages = $_SESSION['bulk_mt_batch_download_results'];

  $form['lingotek_message_1'] = array( '#markup' => '<h3>Your translated content has been downloaded.</h3>' );
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
 * Dashboard Confirmation Screen: Machine Translation Download Complete - Form Submit - Redirects Back to the Lingotek Dashboard.
 */
function lingotek_bulk_mt_download_batch_complete_submit( $form, $form_state ) {
  drupal_goto( LINGOTEK_BASE_URL );
}
