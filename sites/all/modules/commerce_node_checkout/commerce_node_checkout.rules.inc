<?php
/**
 * @file
 * Provides rules integration for commerce_node_checkout
 */

/**
 * Implements hook_rules_action_info().
 */
function commerce_node_checkout_rules_action_info() {
  $actions = array();
  $actions['commerce_node_checkout_rules_nodes_from_order'] = array(
    'label' => t('Nodes from an order'),
    'parameter' => array(
      'order' => array(
        'type' => 'commerce_order',
        'label' => t('The completed order')
      ),
    ),
    'provides' => array(
      'line_items' => array(
        'type' => 'list<commerce_line_item>',
        'label' => t('Associated Line items'),
        'description' => t('All line items that had associated nodes'),
      ),
      'nodes' => array(
        'type' => 'list<node>',
        'label' => t('Associated Nodes'),
        'description' => t('The associated nodes'),
      ),
    ),
    'group' => t('Commerce Node Checkout'),
  );
  return $actions;
}

/**
 * Implements hook_rules_condition_info().
 */
function commerce_node_checkout_rules_condition_info() {
  $conditions = array();
  $conditions['commerce_node_checkout_rules_order_has_nodes'] = array(
    'label' => t('Order has nodes'),
    'parameter' => array(
      'commerce_order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
      ),
    ),
    'group' => t('Commerce Node Checkout'),
    'callbacks' => array(
      'execute' => 'commerce_node_checkout_rules_order_has_nodes',
    ),
  );
  $conditions['commerce_node_checkout_rules_order_has_only_nodes'] = array(
    'label' => t('Order has only nodes'),
    'parameter' => array(
      'commerce_order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
      ),
    ),
    'group' => t('Commerce Node Checkout'),
    'callbacks' => array(
      'execute' => 'commerce_node_checkout_rules_order_has_only_nodes',
    ),
  );
  return $conditions;
}

/**
 * Rules condition callback to determine if an order contains nodes.
 */
function commerce_node_checkout_rules_order_has_nodes($order) {
  // Iterate the products
  foreach (commerce_node_checkout_get_order_products($order) as $product) {
    // See if this is a node product
    if ($product->type == 'commerce_node_checkout') {
      // Any match means the condition has been met
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Rules condition callback to determine if an order contains nodes.
 */
function commerce_node_checkout_rules_order_has_only_nodes($order) {
  // Iterate the products
  foreach (commerce_node_checkout_get_order_products($order) as $product) {
    // See if this is not a node product
    if ($product->type != 'commerce_node_checkout') {
      // Any match means the condition has been met
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * Rules action callback to extract nodes from an order to be passed
 * along for modification and publication.
 *
 * @param $order
 *   A completed order.
 */
function commerce_node_checkout_rules_nodes_from_order($order) {
  $wrapper = entity_metadata_wrapper('commerce_order', $order);
  $line_items = $nodes = array();
  foreach ($wrapper->commerce_line_items as $delta => $line_item_wrapper) {
    // Load and validate the specified product ID.
    $product = $line_item_wrapper->commerce_product->value();
    if ($product->type != 'commerce_node_checkout') {
      // We don't need this item.
      continue;
    }
    $line_items[] = entity_load_single($line_item_wrapper->type(), $line_item_wrapper->getIdentifier());
    $nodes[] = $line_item_wrapper->commerce_node_checkout_node->value();
  }

  return array('line_items' => $line_items, 'nodes' => $nodes);
}
