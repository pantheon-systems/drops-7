<?php

/**
 * @file
 * Batch API Code
 */

/**
 * The $batch can include the following values. Only 'operations'
 * and 'finished' are required, all others will be set to default values.
 *
 * @param operations
 *   An array of callbacks and arguments for the callbacks.
 *   There can be one callback called one time, one callback
 *   called repeatedly with different arguments, different
 *   callbacks with the same arguments, one callback with no
 *   arguments, etc.
 *
 * @param finished
 *   A callback to be used when the batch finishes.
 *
 * @param title
 *   A title to be displayed to the end user when the batch starts.
 *
 * @param init_message
 *   An initial message to be displayed to the end user when the batch starts.
 *
 * @param progress_message
 *   A progress message for the end user. Placeholders are available.
 *   Placeholders note the progression by operation, i.e. if there are
 *   2 operations, the message will look like:
 *    'Processed 1 out of 2.'
 *    'Processed 2 out of 2.'
 *   Placeholders include:
 *     @current, @remaining, @total and @percentage
 *
 * @param error_message
 *   The error message that will be displayed to the end user if the batch
 *   fails.
 *
 */
function _taxonomy_menu_insert_link_items_batch($vid) {
  $terms = taxonomy_get_tree($vid);
  $menu_name = variable_get(_taxonomy_menu_build_variable('vocab_menu', $vid), FALSE);

  $batch = array(
    'operations' => array(
      array('_taxonomy_menu_insert_link_items_process', array($terms, $menu_name)),
    ),
    'finished' => '_taxonomy_menu_insert_link_items_success',
    'title' => t('Rebuilding Taxonomy Menu'),
    'init_message' => t('The menu items have been deleted, and are about to be regenerated.'),
    'progress_message' => t('Import progress: Completed @current of @total stages.'),
    'redirect' => 'admin/structure/taxonomy',
    'error_message' => t('The Taxonomy Menu rebuild process encountered an error.'),
  );
  batch_set($batch);
  batch_process();
}


/*
 * Insert 10 menu link items
 */
function _taxonomy_menu_insert_link_items_process($terms, $menu_name, &$context) {
  _taxonomy_menu_batch_init_context($context, $start, $end, 10);

  // Loop through $terms to process each term.
  for ($i=$start; $i<count($terms) && $i<$end; $i++) {
    $args = array(
      'term' => $terms[$i],
      'menu_name' => $menu_name,
    );
    $mlid = taxonomy_menu_handler('insert', $args);
  }

  _taxonomy_menu_batch_update_context($context, $end, count($terms), 'Creating Menu Items');
}



/*
 * Set a message stating the menu has been updated
 */
function _taxonomy_menu_insert_link_items_success() {
  // TODO state menu name here.
  drupal_set_message(t('The Taxonomy Menu has been updated.'));
}

/*
 * Initialise the batch context
 * @param array $context Batch context array.
 * @param int $start The item to start on in this pass
 * @param int $end The end item of this pass
 * @param int $items The number of items to process in this pass
 */
function _taxonomy_menu_batch_init_context(&$context, &$start, &$end, $items) {
  // Initialize sandbox the first time through.
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
  }

  $start = $context['sandbox']['progress'];
  $end = $start + $items;
}


/*
 * Update the batch context
 *
 * @param array $context Batch context array.
 * @param int $end The end point of the most recent pass
 * @param int $total The total number of items to process in this batch
 * @param str $msg Message for the progress bar
 */
function _taxonomy_menu_batch_update_context(&$context, $end, $total, $msg) {
  //Update context array
  if ($end > $total) {
    $context['finished'] = 1;
    return;
  }
  $context['message'] = "{$msg}: {$end} of {$total}";
  $context['sandbox']['progress'] = $end;
  $context['finished'] = $end/$total;
}
