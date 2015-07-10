<?php

/**
 * @file
 * Defines Views integration for Commerce Shipping.
 */


/**
 * Implements hook_views_data_alter()
 */
function commerce_shipping_views_data_alter(&$data) {
  // Add a relationship to the order table to join to a representative shipping
  // line item (in this case, the one with the lowest line item ID) through the
  // commerce_line_items field data table.
  $data['commerce_order']['shipping_line_item_representative']['relationship'] = array(
    'title' => t('Representative shipping line item'),
    'label'  => t('Representative shipping line item'),
    'help' => t('Obtains a single representative shipping line item for each order.'),
    'handler' => 'commerce_shipping_handler_relationship_shipping_line_item_representative',
    'base'   => 'commerce_line_item',
    'base field'  => 'line_item_id',
    'relationship table' => 'commerce_order',
    'relationship field' => 'commerce_order.order_id',
  );
}
