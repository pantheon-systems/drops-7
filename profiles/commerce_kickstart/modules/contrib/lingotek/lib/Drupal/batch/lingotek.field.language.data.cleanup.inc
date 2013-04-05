<?php
/**
 * @file
 * Functions for the Batch:  lingotek_field_language_data_cleanup_batch_create()
 */




/**
 * Batch API worker for changing the node language setting.
 */
function lingotek_node_source_language_cleanup_batch_worker( $nid, $source_language ) {

  $loaded_node = node_load( $nid );
  $loaded_node->language = $source_language;
  $loaded_node->lingotek_upload_override = 0;// Set 0 : Ensure that uploading does not occur. Set 1 : Force uploading to occur
  node_save($loaded_node);
}


/**
 * Batch API processor for field data language updates.
 */
function lingotek_field_language_data_cleanup_batch_worker( $nid, &$context ) {

  $process_node = node_load($nid);
  if ($process_node->nid) {
    lingotek_field_language_data_cleanup_update_node($process_node->nid);
    $context['message'] = t('Updating field data for node: @node_title', array('@node_title' => $process_node->title));
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
    watchdog('lingotek', 'Attempted to update field data for a non-existent node: @node_id',
      array('@node_id' => $node_id), WATCHDOG_ERROR);
  }

  return $edited;
}


/**
 * FINISHED CALLBACK:  lingotek_field_language_data_cleanup_batch_create()
 */
function lingotek_field_language_data_cleanup_batch_finished($success, $results, $operations) {
}
