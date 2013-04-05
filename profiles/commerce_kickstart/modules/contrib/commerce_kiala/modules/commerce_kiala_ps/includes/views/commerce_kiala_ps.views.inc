<?php

/**
 * @file
 * Views integration for Commerce Kiala Pack & Ship.
 */

/**
 * Implements hook_views_data()
 */
function commerce_kiala_ps_views_data() {
  $data = array();

  $data['commerce_kiala_ps_ws_log']['table']['group']  = t('Commerce Kiala Pack and Ship Web Service Log');
  $data['commerce_kiala_ps_ws_log']['table']['base'] = array(
    'field' => 'id',
    'title' => t('Commerce Kiala P&S Web Service Log'),
    'help' => t('Stores Kiala Pack & Ship web service logs.'),
  );

  // For other base tables, explain how we join
  // 'left_field' is the primary key in the referenced table.
  // 'field' is the foreign key in this table.
  $data['commerce_kiala_ps_ws_log']['table']['join'] = array(
    'commerce_order' => array(
      'left_field' => 'order_id',
      'field' => 'order_id',
    ),
  );


  // Expose the log ID.
  $data['commerce_kiala_ps_ws_log']['log_id'] = array(
    'title' => t('Log ID'),
    'help' => t('The log ID of the Kiala web service log.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  // Expose the order ID.
  $data['commerce_kiala_ps_ws_log']['order_id'] = array(
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

  // Expose the timestamp
  $data['commerce_kiala_ps_ws_log']['timestamp'] = array(
    'title' => t('Timestamp'),
    'help' => t('The date the log was created.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  // Expose the status.
  $data['commerce_kiala_ps_ws_log']['status'] = array(
    'title' => t('Status'),
    'help' => t('The status of the web service response.'),
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


  // Expose the tracking number
  $data['commerce_kiala_ps_ws_log']['tracking_number'] = array(
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

  // Expose the tracking number
  $data['commerce_kiala_ps_ws_log']['method'] = array(
    'title' => t('Service method'),
    'help' => t('The Kiala web service method for this transaction.'),
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

  // Expose the tracking number
  $data['commerce_kiala_ps_ws_log']['response'] = array(
    'title' => t('Response data'),
    'help' => t('The Kiala web service response data.'),
    'field' => array(
      'handler' => 'views_handler_field_serialized',
      'click sortable' => FALSE,
    ),
  );

  // Operation links
  $data['commerce_kiala_ps_ws_log']['operations'] = array(
    'field' => array(
      'title' => t('Operations links'),
      'help' => t('Display all the available operations links for the web service logs.'),
      'handler' => 'commerce_kiala_ps_handler_field_ws_log_operations',
    ),
  );

  return $data;
}
