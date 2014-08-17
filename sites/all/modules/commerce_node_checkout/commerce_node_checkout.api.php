<?php
/**
 * @file
 * Provides example hooks defined by the module.
 */

/**
 * Implements hook_commerce_node_checkout_line_item_alter
 *
 * Allows modules to interact with the line item to be added to the cart
 * before it is saved. Modules can't use hook_commerce_line_item_alter or
 * other entity alter functions because they don't have the $node object or
 * the associated (selected) product in scope which this hook does.
 *
 * @param object $line_item
 *   The commerce line item about to be added to the cart
 * @param object $product
 *   The commerce product option selection by the node author
 * @param object $node
 *   The node object created by the author
 */
function hook_commerce_node_checkout_line_item_alter(&$line_item, $product, $node) {
  // Load the interval field from the product.
  $interval = field_get_items('commerce_product', $product, 'commerce_node_checkout_expire');
  if ($interval && !empty($interval['0'])) {
    // We have an expiry field - apply it.
    $due_date_obj = new DateObject('now');
    if (interval_apply_interval($due_date_obj, $interval)) {
      // Update the expiry field on the line item.
      $line_item->commerce_node_checkout_expires[LANGUAGE_NONE][0]['value'] = $due_date_obj->format('U');
    }
  }
}
