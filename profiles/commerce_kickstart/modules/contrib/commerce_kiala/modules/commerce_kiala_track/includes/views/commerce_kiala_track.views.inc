<?php

/**
 * @file
 * Views integration for Commerce Kiala.
 */

/**
 * Implements hook_views_data()
 */
function commerce_kiala_track_views_data() {
  $data = array();

  // -----------------------------------------------------------------------
  // Tracking object data

  $data['commerce_kiala_tracking']['table']['group']  = t('Commerce Kiala Tracking');
  $data['commerce_kiala_tracking']['table']['base'] = array(
    'field' => 'id',
    'title' => t('Commerce Kiala Order Tracking'),
    'help' => t('Stores kiala tracking information associated with an order.'),
  );

  // For other base tables, explain how we join
  // 'left_field' is the primary key in the referenced table.
  // 'field' is the foreign key in this table.
  $data['commerce_kiala_tracking']['table']['join'] = array(
    'commerce_order' => array(
      'left_field' => 'order_id',
      'field' => 'order_id',
    ),
  );


  // Expose Fields

  // Expose the order ID.
  $data['commerce_kiala_tracking']['order_id'] = array(
    'title' => t('Order ID'),
    'help' => t('The unique internal identifier of the order.'),
    'field' => array(
      'handler' => 'commerce_order_handler_field_order',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'commerce_order_handler_argument_order_order_id',
      'name field' => 'order_label',
      'numeric' => TRUE,
      'validate type' => 'order_id',
    ),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'commerce_order',
      'field' => 'order_id',
      'label' => t('Order', array(), array('context' => 'a drupal commerce order')),
    ),
  );


  // Expose the tracking number
  $data['commerce_kiala_tracking']['tracking_number'] = array(
    'title' => t('Kiala Tracking Number'),
    'help' => t('The Kiala tracking number selected on the order.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Tracking link
  $data['commerce_kiala_tracking']['tracking_link'] = array(
    'title' => t('Kiala tracking information link'),
    'help' => t('External link to kiala tracking information.'),
    'field' => array(
      'handler' => 'commerce_kiala_track_views_handler_tracking_link_field',
      'click sortable' => TRUE,
      'real field' => 'tracking_number'
    ),
  );

  // Tracking iFrame popup
  $data['commerce_kiala_tracking']['tracking_iframe'] = array(
    'title' => t('Kiala tracking information iFrame link'),
    'help' => t('Link to open kiala tracking information in an iFrame.'),
    'field' => array(
      'handler' => 'commerce_kiala_track_views_handler_tracking_iframe_field',
      'click sortable' => TRUE,
      'real field' => 'tracking_number'
    ),
  );


  return $data;
}
