<?php

/**
 * @file
 * Provides Rules integration for Kiala Tracking.
 */

/**
 * Implements hook_rules_action_info().
 */
function commerce_kiala_track_rules_action_info() {
  $actions = array();

  $actions['commerce_kiala_track_set_order_tracking'] = array(
    'label' => t('Update an order\'s Kiala tracking number'),
    'parameter' => array(
      'order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
        'restriction' => 'selector',
      ),
      'tracking_number' => array(
        'type' => 'text',
        'label' => t('Tracking Number'),
      ),
    ),
    'group' => t('Commerce Kiala Tracking'),
    'callbacks' => array(
      'execute' => 'commerce_kiala_track_rules_set_order_tracking',
    ),
  );

  return $actions;
}


/**
 * Rules action: set an order tracking number
 */
function commerce_kiala_track_rules_set_order_tracking($order, $tracking_number) {
  if (!empty($tracking_number) && !empty($order->order_id)) {
    $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
    $tracking_object = $order_wrapper->kiala_tracking_object->value();

    if (empty($tracking_object->order_id)) {
      $tracking_object->order_id = $order->order_id;
    }

    $tracking_object->tracking_number = $tracking_number;
    commerce_kiala_track_write_tracking_record($tracking_object);
  }
}
