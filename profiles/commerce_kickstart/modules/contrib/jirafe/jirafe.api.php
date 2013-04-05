<?php

/**
 * @file
 * This file contains documentation for hooks in Jirafe module. It contains no
 * working PHP code.
 */

/**
 * Allows modules to alter Jirafe order data mapping.
 *
 * @param $types
 *   Component types for order data.
 */
function hook_jirafe_component_types_alter(&$types) {
  // Removes tax mapping
  $types['tax_rate'] = '';
  $types['tax'] = array();
}

/**
 * Allows modules to alter resources.
 *
 * @param $resources
 *   Compiled resources.
 * @param $config
 *   Jirafe configuration.
 */
function hook_jirafe_resources_alter(&$resources, $config) {
  // Changes first user name.
  $resources['users'][0]['first_name'] = 'John';
}

/**
 * Allows modules to change current site information.
 *
 * @param $current_site
 *   Current site information array.
 * @param $config
 *   Jirafe configuration.
 */
function hook_jirafe_current_site_alter(&$current_site, $config) {
  // Adds timestamp to current site information
  $current_site['timestamp'] = time();
}

/**
 * Allows modules to change line item in order.
 *
 * @param $jirafe_product
 *   Array with product information to be reported to Jirafe.
 * @param $line_item
 *   Commerce line item.
 */
function hook_jirafe_commerce_order_line_item(&$jirafe_product, $line_item) {
  // Call all reported products as 'Ni'
  $jirafe_product['name'] = 'Ni';
}

/**
 * Allows module to change data to be reported to Jirafe.
 *
 * @param $data
 *   Array to be reported to Jirafe.
 * @param $order
 *   Comerce order.
 */
function hook_jirafe_commerce_order_data_alter(&$data, $order) {
  // Removes order reporting.
  $data = array();
}

/**
 * Modifies or removes tracking tag.
 *
 * This alter hook allows other modules to modify or completely remove tracking tag.
 *
 * @param $data
 *   Tracking data.
 * @param $page
 *   Complete page.
 */
function hook_jirafe_tracking_data_alter(&$data, $page) {
  // Remove tracking on Friday 13th.
  if (date('l jS') == 'Friday 13th') {
    $data = FALSE;
  }
  else {
    // Track our product.
    $data['product'] = array(
      'sku' => '123',
      'name' => 'Product 123',
      'price' => '10.00',
      'categories' => array(
        'Items',
      ),
    );
    // Or track our category.
    $data['category'] = array(
      'name' => 'Items',
    );
  }
}
