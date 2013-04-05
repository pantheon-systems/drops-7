<?php

/**
 * @file
 * Views integration for Commerce Kiala.
 */

/**
 * Implements hook_views_data()
 */
function commerce_kiala_views_data() {
  $data = array();

  // -----------------------------------------------------------------------
  // Line item Kiala Points

  $data['commerce_kiala_line_item_point']['table']['group']  = t('Commerce Kiala');
  $data['commerce_kiala_line_item_point']['table']['base'] = array(
    'field' => 'id',
    'title' => t('Commerce Kiala Line Item Points'),
    'help' => t('Stores kiala point ids associated with a line item.'),
  );

  // For other base tables, explain how we join
  // 'left_field' is the primary key in the referenced table.
  // 'field' is the foreign key in this table.
  $data['commerce_kiala_line_item_point']['table']['join'] = array(
    'commerce_line_item' => array(
      'left_field' => 'line_item_id',
      'field' => 'line_item_id',
    ),
  );


  // Expose Fields

  // Expose the line item ID.
  $data['commerce_kiala_line_item_point']['line_item_id'] = array(
    'title' => t('Line item ID'),
    'help' => t('The unique internal identifier of the line item.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'commerce_line_item_handler_argument_line_item_line_item_id',
      'name field' => 'line_item_label',
      'numeric' => TRUE,
      'validate type' => 'line_item_id',
    ),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'commerce_line_item',
      'field' => 'line_item_id',
      'label' => t('Line Item', array(), array('context' => 'a drupal commerce order')),
    ),
  );


  // Expose the line item ID.
  $data['commerce_kiala_line_item_point']['point_id'] = array(
    'title' => t('Kiala Point ID'),
    'help' => t('The Kiala Points ID selected on the shipping line item.'),
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


  return $data;
}
