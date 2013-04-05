<?php

/**
 * @file
 * Rules integration for shipping.
 *
 * @addtogroup rules
 * @{
 */

/**
 * Implements hook_rules_event_info().
 */
function commerce_shipping_rules_event_info() {
  $events = array();

  $events['commerce_shipping_collect_rates'] = array(
    'label' => t('Collecting shipping rates for an order'),
    'group' => t('Commerce Shipping'),
    'variables' => array(
      'commerce_order' => array(
        'label' => t('Order', array(), array('context' => 'a drupal commerce order')),
        'type' => 'commerce_order',
        'skip save' => TRUE,
      ),
    ),
    'access callback' => 'commerce_order_rules_access',
  );

  // Include the Line Item module's Rules integration to reuse its access callback.
  module_load_include('inc', 'commerce_line_item', 'commerce_line_item.rules');

  $events['commerce_shipping_calculate_rate'] = array(
    'label' => t('Calculating a shipping rate'),
    'group' => t('Commerce Shipping'),
    'variables' => array(
      'commerce_line_item' => array(
        'label' => t('Shipping line item'),
        'type' => 'commerce_line_item',
        'skip save' => TRUE,
      ),
    ),
    'access callback' => 'commerce_line_item_rules_access',
  );

  return $events;
}

/**
 * Implements hook_rules_condition_info().
 */
function commerce_shipping_rules_condition_info() {
  $conditions = array();

  $conditions['commerce_shipping_compare_shipping_service'] = array(
    'label' => t('Shipping line item exists'),
    'parameter' => array(
      'commerce_order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
        'description' => t('The order whose line items should be checked for an existing shipping line item.'),
      ),
      'service' => array(
        'type' => 'text',
        'label' => t('Shipping service'),
        'options list' => 'commerce_shipping_service_rules_options_list',
        'description' => t('Limit the search amongst the shipping line items to a particular shipping service.'),
      ),
    ),
    'group' => t('Commerce Shipping'),
    'callbacks' => array(
      'execute' => 'commerce_shipping_rules_line_item_exists',
    ),
  );

  return $conditions;
}

/**
 * Returns a list of shipping services available for existence conditions.
 */
function commerce_shipping_service_rules_options_list() {
  // Return an options array with an "Any" option prefixed to the normal list of
  // shipping service options.
  return array_merge(array('-any-' => t('Any')), commerce_shipping_service_options_list());
}

/**
 * Checks an order for the existence of a shipping line item.
 *
 * @param $order
 *   The order to check for a shipping line item.
 * @param $service
 *   The machine-name of a particular shipping service to search for; if '-any-'
 *   the condition returns TRUE for any found shipping line item.
 */
function commerce_shipping_rules_line_item_exists($order, $service) {
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);

  // Loop over all the line items on the order.
  foreach ($order_wrapper->commerce_line_items as $delta => $line_item_wrapper) {
    // If the current line item is a shipping line item and the condition either
    // doesn't care about the service or the service name matches, return TRUE.
    if ($line_item_wrapper->type->value() == 'shipping' &&
      ($service == '-any-' || $line_item_wrapper->commerce_shipping_service->value() == $service)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 * Implements hook_rules_action_info().
 */
function commerce_shipping_rules_action_info() {
  $actions = array();

  if (count(commerce_shipping_services()) > 0) {
    $actions['commerce_shipping_service_rate_order'] = array(
      'label' => t('Add a rate for a shipping service to an order'),
      'parameter' => array(
        'shipping_service_name' => array(
          'type' => 'text',
          'label' => t('Shipping service'),
          'options list' => 'commerce_shipping_service_options_list',
        ),
        'commerce_order' => array(
          'type' => 'commerce_order',
          'label' => t('Order'),
        ),
      ),
      'group' => t('Commerce Shipping'),
    );
  }

  if (count(commerce_shipping_methods()) > 0) {
    $actions['commerce_shipping_method_collect_rates'] = array(
      'label' => t('Collect rates for a shipping method'),
      'parameter' => array(
        'shipping_method_name' => array(
          'type' => 'text',
          'label' => t('Shipping method'),
          'options list' => 'commerce_shipping_method_options_list',
        ),
        'commerce_order' => array(
          'type' => 'commerce_order',
          'label' => t('Order'),
        ),
      ),
      'group' => t('Commerce Shipping'),
    );
  }

  if (count(commerce_shipping_methods()) > 0) {
    $actions['commerce_shipping_rate_apply'] = array(
      'label' => t('Apply shipping rate to an order'),
      'parameter' => array(
        'commerce_order' => array(
          'type' => 'commerce_order',
          'label' => t('Order'),
          'save' => TRUE,
        ),
        'shipping_service_name' => array(
          'type' => 'text',
          'label' => t('Shipping service'),
          'options list' => 'commerce_shipping_service_options_list',
          'optional' => TRUE,
          'description' => t('If left empty, the first available shipping service will be applied.'),
        ),
      ),
      'group' => t('Commerce Shipping'),
    );
  }

  $actions['commerce_shipping_delete_shipping_line_items'] = array(
    'label' => t('Delete all shipping line items from an order'),
    'parameter' => array(
      'commerce_order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
      ),
    ),
    'group' => t('Commerce Shipping'),
  );

  return $actions;
}

/**
 * Action: Apply a shipping rate to an order.
 */
function commerce_shipping_rate_apply($order, $service_name = NULL) {

  if (isset($service_name) && !isset($order->shipping_rates[$service_name])) {
    // Make the chosen service available to the order.
    commerce_shipping_service_rate_order($service_name, $order);
  }
  elseif (!isset($service_name)) {
    if (empty($order->shipping_rates)) {
      // No available rate.
      return;
    }
    $service_name = key($order->shipping_rates);
  }


  // Delete any existing shipping line items from the order.
  commerce_shipping_delete_shipping_line_items($order, TRUE);

  // Extract the unit price from the calculated rate.
  $rate_line_item = $order->shipping_rates[$service_name];
  $rate_line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $rate_line_item);
  $unit_price = $rate_line_item_wrapper->commerce_unit_price->value();

  // Create a new shipping line item with the calculated rate from the form.
  $line_item = commerce_shipping_line_item_new($service_name, $unit_price, $order->order_id, $rate_line_item->data, $rate_line_item->type);

  // Save and add the line item to the order.
  commerce_shipping_add_shipping_line_item($line_item, $order, TRUE);
}

/**
 * @}
 */
