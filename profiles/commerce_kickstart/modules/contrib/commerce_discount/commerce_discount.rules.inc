<?php

/**
 * @file
 * Rules integration for the Commerce Discount module.
 */

/**
 * Implements hook_rules_action_info().
 */
function commerce_discount_rules_action_info() {
  $types = commerce_discount_offer_types();
  $items = array();

  foreach (array('fixed_amount', 'percentage', 'free_products') as $type) {
    $items[$types[$type]['action']] = array(
      'label' => $types[$type]['label'],
      'parameter' => array(
        'entity' => array(
          'label' => t('Entity'),
          'type' => 'entity',
          'wrapped' => TRUE,
        ),
        'commerce_discount' => array(
          'label' => t('Commerce discount'),
          'type' => 'token',
          'options list' => 'commerce_discount_entity_list',
        ),
      ),
      'group' => t('Commerce Discount'),
      'base' => $types[$type]['action'],
    );
  }

  $items['commerce_discount_free_shipping_service'] = array(
    'label' => t('Free shipping service'),
    'group' => t('Commerce Discount'),
    'parameter' => array(
      'entity' => array(
        'label' => t('Entity'),
        'type' => 'entity',
        'wrapped' => FALSE,
      ),
      'commerce_discount' => array(
        'label' => t('Commerce discount'),
        'type' => 'token',
        'options list' => 'commerce_discount_entity_list',
      ),
      'shipping_service' => array(
        'label' => t('Shipping service'),
        'type' => 'token',
        'options list' => 'commerce_shipping_service_options_list'
      )
    ),
    'base' => 'commerce_discount_free_shipping_service',
  );

  return $items;
}

/**
 * Implements hook_rules_event_info().
 */
function commerce_discount_rules_event_info() {
  $items = array();

  $items['commerce_discount_order'] = array(
    'label' => t('Apply a discount to a given order'),
    'group' => t('Commerce - discount'),
    'variables' => entity_rules_events_variables('commerce_order', t('Order', array(), array('context' => 'a drupal commerce order'))),
    'access callback' => 'commerce_order_rules_access',
  );

  $items['commerce_discount_free_shipping'] = array(
    'label' => t('Makes a shipping service free'),
    'group' => t('Commerce - discount'),
    'variables' => entity_rules_events_variables('commerce_order', t('Order', array(), array('context' => 'a drupal commerce order'))),
    'access callback' => 'commerce_order_rules_access',
  );

  return $items;
}


/**
 * Rules action: Apply fixed amount discount.
 */
function commerce_discount_fixed_amount(EntityDrupalWrapper $wrapper, $discount_name) {
  $discount_wrapper = entity_metadata_wrapper('commerce_discount', $discount_name);
  $discount_price = $discount_wrapper->commerce_discount_offer->commerce_fixed_amount->value();
  $discount_price['amount'] = -$discount_price['amount'];

  switch ($wrapper->type()) {
    case 'commerce_order':
      // Set reference to the discount.
      // @todo: It doesn't work with the wrapper.
      $order = $wrapper->value();
      $delta = $wrapper->commerce_discounts->count();
      $order->commerce_discounts[LANGUAGE_NONE][$delta]['target_id'] = $discount_wrapper->discount_id->value();

      commerce_discount_add_line_item($wrapper, $discount_name, $discount_price);

      // Updating the commerce order calculate price; For some reason, without
      // the following call to commerce_order_calculate_total() the discount
      // components aren't re-added after they get deleted in
      // commerce_discount_commerce_order_presave().
      commerce_order_calculate_total($order);
      break;

    case 'commerce_line_item':
      // Check whether this discount was already added as a price component.
      $price_data = $wrapper->commerce_unit_price->data->value();

      foreach ($price_data['components'] as $component) {
        if (!empty($component['price']['data']['discount_name']) && $component['price']['data']['discount_name'] == $discount_wrapper->getIdentifier()) {
          return;
        }
      }

      commerce_discount_add_price_component($wrapper, $discount_name, $discount_price);
      break;
  }
}

/**
 * Rules action: Apply percentage discount.
 */
function commerce_discount_percentage(EntityDrupalWrapper $wrapper, $discount_name) {
  $discount_wrapper = entity_metadata_wrapper('commerce_discount', $discount_name);
  $rate = $discount_wrapper->commerce_discount_offer->commerce_percentage->value();
  // Get the line item types to apply the discount to.
  $line_item_types = variable_get('commerce_discount_line_item_types', array('product' => 'product'));

  if ($rate > 1) {
    $rate = $rate / 100;
  }

  switch ($wrapper->type()) {
    case 'commerce_order':
      if (!$wrapper->commerce_line_items->value()) {
        return;
      }

      // Set reference to the discount.
      // @todo: It doesn't work with the wrapper.
      $order = $wrapper->value();
      $delta = $wrapper->commerce_discounts->count();
      $order->commerce_discounts[LANGUAGE_NONE][$delta]['target_id'] = $discount_wrapper->discount_id->value();

      $calculated_discount = 0;
      // Loop the line items of the order and calculate the total discount.
      foreach ($wrapper->commerce_line_items as $line_item_wrapper) {
        if (!empty($line_item_types[$line_item_wrapper->type->value()])) {
          $line_item_total = commerce_price_wrapper_value($line_item_wrapper, 'commerce_total', TRUE);
          $calculated_discount += $line_item_total['amount'] * $rate;
        }
      }

      if ($calculated_discount) {
        $discount_amount = array(
          'amount' => $calculated_discount * -1,
          'currency_code' => $wrapper->commerce_order_total->currency_code->value(),
        );
        commerce_discount_add_line_item($wrapper, $discount_name, $discount_amount);

        // Updating the commerce order calculate price; For some reason, without
        // the following call to commerce_order_calculate_total() the discount
        // components aren't re-added after they get deleted in
        // commerce_discount_commerce_order_presave().
        commerce_order_calculate_total($order);
      }
      break;

    case 'commerce_line_item':
      // Check if the line item is configured in the settings to apply the
      // discount.
      if (empty($line_item_types[$wrapper->type->value()])) {
        return;
      }

      // Check whether this discount was already added as a price component.
      $price_data = $wrapper->commerce_unit_price->data->value();
      foreach ($price_data['components'] as $component) {
        if (!empty($component['price']['data']['discount_name']) && $component['price']['data']['discount_name'] == $discount_name) {
          return;
        }
      }

      $unit_price = commerce_price_wrapper_value($wrapper, 'commerce_unit_price', TRUE);
      $calculated_discount = $unit_price['amount'] * $rate * -1;

      $discount_amount = array(
        'amount' => $calculated_discount,
        'currency_code' => $unit_price['currency_code'],
      );
      commerce_discount_add_price_component($wrapper, $discount_name, $discount_amount);
      break;
  }
}

/**
 * Rules action: Apply free shipping discount.
 *
 * @param object $order
 *   Wrapped commerce_order entity type.
 * @param string $discount_name
 *   The name of the discount.
 * @param string $service_name
 *   The shipping service machine-name.
 */
function commerce_discount_free_shipping_service($order, $discount_name, $service_name) {
  if (isset($order->shipping_rates[$service_name])) {
    $wrapper_line_item = entity_metadata_wrapper('commerce_line_item', $order->shipping_rates[$service_name]);

    // Exit if the rule has already been processed.
    if ($wrapper_line_item->commerce_unit_price->amount->value() == 0) {
      return;
    }

    $wrapper_line_item->commerce_unit_price->amount = 0;

    $shipping_service = commerce_shipping_service_load($service_name);

    // Updating the data component.
    $wrapper_line_item->commerce_unit_price->data = array();
    $wrapper_line_item->commerce_unit_price->data = commerce_price_component_add(
      $wrapper_line_item->commerce_unit_price->value(),
      $shipping_service['price_component'],
      $wrapper_line_item->commerce_unit_price->value(),
      TRUE,
      FALSE
    );
  }
}

/**
 * Rules action: Apply free products discount.
 *
 * @param EntityDrupalWrapper $wrapper
 *   Wrapped commerce_order entity type.
 * @param string $discount_name
 *   The name of the discount.
 */
function commerce_discount_free_products(EntityDrupalWrapper $wrapper, $discount_name) {
  $discount_wrapper = entity_metadata_wrapper('commerce_discount', $discount_name);
  if (!$products = $discount_wrapper->commerce_discount_offer->commerce_free_products->value()) {
    return;
  }

  // Set reference to the discount.
  $order = $wrapper->value();
  $delta = $wrapper->commerce_discounts->count();
  $order->commerce_discounts[LANGUAGE_NONE][$delta]['target_id'] = $discount_wrapper->discount_id->value();

  // Loop on products and add each product to order line items.
  foreach ($products as $product) {
    //TODO: Add display_path property to line item data context in order to expose product link in shopping cart.
    $data = array('context' => array('commerce_discount_offer' => 'free_products', 'discount_name' => $discount_name));
    $line_item = commerce_product_line_item_new($product, 1, $order->order_id, $data, 'product_discount');

    $wrapper_line_item = entity_metadata_wrapper('commerce_line_item', $line_item);

    // Getting the product price and negate it for the discount component price.
    $product_unit_price = commerce_price_wrapper_value(entity_metadata_wrapper('commerce_product', $product), 'commerce_price');
    $discount_amount = array(
      'amount' => -$product_unit_price['amount'],
      'currency_code' => $product_unit_price['currency_code'],
    );

    commerce_discount_add_price_component($wrapper_line_item, $discount_name, $discount_amount);

    commerce_line_item_save($line_item);

    // Add the free product to order's line items.
    $wrapper->commerce_line_items[] = $line_item;
  }

  // Updating the commerce order calculate price; For some reason, without
  // the following call to commerce_order_calculate_total() the discount
  // components aren't re-added after they get deleted in
  // commerce_discount_commerce_order_presave().
  commerce_order_calculate_total($order);
}

/**
 * Creates a discount line item on the provided order.
 *
 * @param EntityDrupalWrapper $order_wrapper
 *   The wrapped order entity.
 * @param string $discount_name
 *   The name of the discount being applied.
 * @param array $discount_amount
 *   The discount amount price array (amount, currency_code).
 */
function commerce_discount_add_line_item(EntityDrupalWrapper $order_wrapper, $discount_name, $discount_amount) {
  // Create a new line item.
  $discount_line_item = entity_create('commerce_line_item', array(
    'type' => 'commerce_discount',
    'order_id' => $order_wrapper->order_id->value(),
    'quantity' => 1,
  ));
  $discount_line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $discount_line_item);

  // Initialize the line item unit price.
  $discount_line_item_wrapper->commerce_unit_price->amount = 0;
  $discount_line_item_wrapper->commerce_unit_price->currency_code = $discount_amount['currency_code'];

  // Reset the data array of the line item total field to only include a
  // base price component, set the currency code from the order.
  $base_price = array(
    'amount' => 0,
    'currency_code' => $discount_amount['currency_code'],
    'data' => array(),
  );
  $discount_line_item_wrapper->commerce_unit_price->data = commerce_price_component_add($base_price, 'base_price', $base_price, TRUE);

  // Add the actual discount price component.
  commerce_discount_add_price_component($discount_line_item_wrapper, $discount_name, $discount_amount);

  // Save the line item and add it to the order.
  $discount_line_item_wrapper->save();
  $order_wrapper->commerce_line_items[] = $discount_line_item_wrapper;
}

/**
 * Adds a discount price component to the provided line item.
 *
 * @param EntityDrupalWrapper $line_item_wrapper
 *   The wrapped line item entity.
 * @param string $discount_name
 *   The name of the discount being applied.
 * @param array $discount_amount
 *   The discount amount price array (amount, currency_code).
 */
function commerce_discount_add_price_component(EntityDrupalWrapper $line_item_wrapper, $discount_name, $discount_amount) {
  $unit_price = commerce_price_wrapper_value($line_item_wrapper, 'commerce_unit_price', TRUE);
  $discount_wrapper = entity_metadata_wrapper('commerce_discount', $discount_name);
  $component_title = $discount_wrapper->component_title->value();
  $current_amount = $unit_price['amount'];
  // Currencies don't match, abort.
  if ($discount_amount['currency_code'] != $unit_price['currency_code']) {
    return;
  }

  // Calculate the updated amount and create a price array representing the
  // difference between it and the current amount.
  $updated_amount = commerce_round(COMMERCE_ROUND_HALF_UP, $current_amount + $discount_amount['amount']);

  $difference = array(
    'amount' => $discount_amount['amount'],
    'currency_code' => $discount_amount['currency_code'],
    'data' => array(
      'discount_name' => $discount_name,
      'discount_component_title' => empty($component_title) ? 'discount' : $component_title,
    ),
  );

  // Set the amount of the unit price and add the difference as a component.
  $line_item_wrapper->commerce_unit_price->amount = $updated_amount;

  $line_item_wrapper->commerce_unit_price->data = commerce_price_component_add(
    $line_item_wrapper->commerce_unit_price->value(),
    empty($component_title) ? 'discount' : $component_title,
    $difference,
    TRUE
  );
}
