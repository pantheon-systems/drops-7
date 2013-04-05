<?php

/**
 * @file
 * Provides Rules integration for Kiala Pack and Ship.
 */

/**
 * Implements hook_rules_action_info().
 */
function commerce_kiala_ps_rules_action_info() {
  $actions = array();

  $actions['commerce_kiala_ps_rules_ws_ping_create_order'] = array(
    'label' => t('Submit order information to Kiala P&S Web Service'),
    'parameter' => array(
      'order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
        'restriction' => 'selector',
      ),
    ),
    'group' => t('Commerce Kiala P&S'),
    'callbacks' => array(
      'execute' => 'commerce_kiala_ps_rules_ws_ping_create_order',
    ),
  );

  return $actions;
}

/**
 * Rules action: web service createOrder request
 */
function commerce_kiala_ps_rules_ws_ping_create_order($order) {
  if (empty($order->order_id)) {
    return;
  }

  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
  if (!$order_wrapper->kiala_packship_ws_access->value()) {
    return;
  }

  try {
    $response = commerce_kiala_ps_ws_ping_create_order($order);
  }
  catch (Exception $e) {
  }
}
