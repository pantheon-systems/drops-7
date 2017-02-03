<?php

/**
 * @file
 * Views hooks implementations.
 */

/**
 * Implements hook_views_data_alter().
 */
function draggableviews_views_data_alter(&$data) {

  $data['draggableviews_structure']['weight'] = array(
    'title' => t('Weight'),
    'group' => t('Draggableviews'),
    'field' => array(
      'help' => t('Display the weight value.'),
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'help' => t('Sort entities by the draggableviews weight table field.'),
      'handler' => 'draggableviews_handler_sort',
    ),
    'filter' => array(
       'help' => t('Filter by the draggableviews weight value (Native handler only).'),
      'handler' => 'views_handler_filter_numeric',
    ),
  );
  $data['draggableviews_structure']['parent'] = array(
    'title' => t('Parent'),
    'help' => t('The parent entity id.'),
    'group' => t('Draggableviews'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
    ),
    'filter' => array(
      'help' => t('Filter by the draggableviews parent\'s entity id (Native handler only).'),
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  foreach (entity_get_info() as $entity_type => $info) {
    if (isset($info['base table']) && isset($data[$info['base table']]['table'])) {
      $data[$info['base table']]['draggableviews'] = array(
        'title' => $data[$info['base table']]['table']['group'],
        'group' => t('Draggableviews'),
        'help' => t('Provide a draggable functionality.'),
        'real field' => $info['entity keys']['id'],
        'field' => array(
          'handler' => 'draggableviews_handler_field_draggable',
          'click sortable' => FALSE,
        ),
      );
      // Explain to every entity how to join with draggableviews structure table.
      $data['draggableviews_structure']['table']['join'][$info['base table']] = array(
        'handler' => 'draggableviews_join_handler',
        'left_table' => $info['base table'], // Because this is a direct link it could be left out.
        'left_field' => $info['entity keys']['id'],
        'field' => 'entity_id',
      );
    }
  }
}

/**
 * Implements hook_views_post_execute().
 */
function draggableviews_views_post_execute(&$view) {
  if (isset($view->field['draggableviews'])) {
    // Move draggableviews field to last column
    // otherwise tabledrag.js doesn't work.
    $draggable_field = $view->field['draggableviews'];
    unset($view->field['draggableviews']);
    $view->field['draggableviews'] = $draggable_field;
  }
}
