<?php
/**
 * @file
 */

/**
 * Implements hook_views_data().
 * 
 * Describes the two custom handlers to Views.
 */
function views_boxes_views_data() {
  $data['views_boxes']['table']['group']  = t('Boxes');
  $data['views_boxes']['table']['join'] = array(
    '#boxes' => array(),
  );
  $data['views']['limit'] = array(
    'title' => t('Limit'),
    'help' => t('Limit the number of records returned.'),
    'argument' => array(
      'handler' => 'views_boxes_handler_argument_limit',
    ),
  );
  $data['views']['sort'] = array(
    'title' => t('Sort'),
    'help' => t('Limit which items can but used for sort.'),
    'filter' => array(
      'handler' => 'views_boxes_handler_filter_sort',
    ),
  );
  
  return $data;
}
