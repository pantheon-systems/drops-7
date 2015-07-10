<?php

/**
 * @file
 * Rules integration for the Commerce Discount module.
 */

/**
 * Implements hook_rules_condition_info().
 */
function commerce_discount_rules_condition_info() {
  $inline_conditions = inline_conditions_get_info();
  $conditions = array();

  if (module_exists('commerce_order')) {
    $conditions['commerce_order_compare_order_amount'] = array(
      'label' => t('Order amount comparison'),
      'parameter' => array(
        'commerce_order' => array(
          'type' => 'commerce_order',
          'label' => t('Order'),
          'description' => t('The order.'),
          'wrapped' => TRUE,
        ),
        'operator' => array(
          'type' => 'text',
          'label' => t('Operator'),
          'description' => t('The operator used with the order amount value below.'),
          'default value' => '>=',
          'options list' => '_inline_conditions_operator_options',
        ),
        'total' => array(
          'type' => 'commerce_price',
          'label' => t('Order amount'),
          'default value' => '',
          'description' => t('The value to compare against the passed order amount.'),
        ),
      ),
      'module' => 'commerce_discount',
      'group' => t('Commerce Order'),
      'callbacks' => array(
        'execute' => $inline_conditions['commerce_order_compare_order_amount']['callbacks']['build'],
      ),
    );

    $conditions['commerce_order_has_owner'] = array(
      'label' => t('Order owner'),
      'parameter' => array(
        'commerce_order' => array(
          'type' => 'commerce_order',
          'label' => t('Order'),
          'description' => t('The order.'),
          'wrapped' => TRUE,
        ),
        'account' => array(
          'type' => 'user',
          'label' => t('User'),
          'description' => t('User account.'),
        ),
      ),
      'module' => 'commerce_discount',
      'group' => t('Commerce Order'),
      'callbacks' => array(
        'execute' => $inline_conditions['commerce_order_has_owner']['callbacks']['build'],
      ),
    );

    $conditions['commerce_order_contains_products'] = array(
      'label' => t('Order contains products'),
      'parameter' => array(
        'commerce_order' => array(
          'type' => 'commerce_order',
          'label' => t('Order'),
          'description' => t('The order.'),
          'wrapped' => TRUE,
        ),
        'products' => array(
          'type' => 'text',
          'label' => t('Product SKU(s)'),
          'description' => t('Products SKU to look for on the order. Enter a comma-separated list of product SKU(s).'),
        ),
      ),
      'module' => 'commerce_discount',
      'group' => t('Commerce Order'),
      'callbacks' => array(
        'execute' => $inline_conditions['commerce_order_contains_products']['callbacks']['build'],
      ),
    );

    $conditions['commerce_order_has_specific_quantity_products'] = array(
      'label' => t('Order has a specific quantity of products'),
      'parameter' => array(
        'commerce_order' => array(
          'type' => 'commerce_order',
          'label' => t('Order'),
          'description' => t('The order.'),
          'wrapped' => TRUE,
        ),
        'products' => array(
          'type' => 'text',
          'label' => t('Product SKU(s)'),
          'description' => t('Products SKU to look for on the order. Enter a comma-separated list of product SKU(s).'),
        ),
        'operator' => array(
          'type' => 'text',
          'label' => t('Operator'),
          'description' => t('The operator used with the product quantity value below.'),
          'default value' => '>=',
          'options list' => '_inline_conditions_order_operator_options',
        ),
        'quantity' => array(
          'type' => 'integer',
          'label' => t('Quantity'),
          'description' => t('Quantity value to be compared against each selected product(s).'),
        ),
      ),
      'module' => 'commerce_discount',
      'group' => t('Commerce Order'),
      'callbacks' => array(
        'execute' => $inline_conditions['commerce_order_has_specific_quantity_products']['callbacks']['build'],
      ),
    );
  }

  if (module_exists('commerce_product')) {
    $conditions['commerce_product_contains_products'] = array(
      'label' => t('Line item contains a specific product'),
      'parameter' => array(
        'commerce_line_item' => array(
          'type' => 'commerce_line_item',
          'label' => t('Line item'),
          'description' => t('The line item.'),
          'wrapped' => TRUE,
        ),
        'sku' => array(
          'type' => 'text',
          'label' => t('SKU'),
          'description' => t('Enter a comma-separated list of product SKU(s) to compare against the passed product line item.'),
        ),
      ),
      'module' => 'commerce_discount',
      'group' => t('Commerce Line Item'),
      'callbacks' => array(
        'execute' => $inline_conditions['commerce_product_contains_products']['callbacks']['build'],
      ),
    );
    $conditions['commerce_product_has_type'] = array(
      'label' => t('Line item contains a specific product type'),
      'parameter' => array(
        'commerce_line_item' => array(
          'type' => 'commerce_line_item',
          'label' => t('Line item'),
          'description' => t('The line item.'),
          'wrapped' => TRUE,
        ),
        'type' => array(
          'type' => 'text',
          'label' => t('Type'),
          'description' => t('Enter the product type to compare against the passed product line item.'),
          'options list' => 'commerce_product_type_options_list',
        ),
      ),
      'module' => 'commerce_discount',
      'group' => t('Commerce Line Item'),
      'callbacks' => array(
        'execute' => $inline_conditions['commerce_product_has_type']['callbacks']['build'],
      ),
    );
  }

  if (module_exists('taxonomy') && module_exists('commerce_product')) {
    $conditions['commerce_product_has_specified_terms'] = array(
      'label' => t('Line item product contains specific terms ID'),
      'parameter' => array(
        'commerce_line_item' => array(
          'type' => 'commerce_line_item',
          'label' => t('Line item'),
          'description' => t('The line item.'),
          'wrapped' => TRUE,
        ),
        'terms' => array(
          'type' => 'text',
          'label' => t('Terms ID'),
          'description' => t('Enter a comma-separated list of term ID to compare against the passed product line item.'),
        ),
      ),
      'module' => 'commerce_discount',
      'group' => t('Commerce Line Item'),
      'callbacks' => array(
        'execute' => $inline_conditions['commerce_product_has_specified_terms']['callbacks']['build'],
      ),
    );
  }

  return $conditions;
}

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
          'label' => t('Commerce Discount'),
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
        'label' => t('Commerce Discount'),
        'type' => 'token',
        'options list' => 'commerce_discount_entity_list',
      ),
      'shipping_service' => array(
        'label' => t('Shipping service'),
        'type' => 'token',
        'options list' => 'commerce_shipping_service_options_list',
      ),
    ),
    'base' => 'commerce_discount_free_shipping_service',
  );

  $items['commerce_discount_percent_off_shipping_service'] = array(
    'label' => t('% off of shipping'),
    'group' => t('Commerce Discount'),
    'parameter' => array(
      'entity' => array(
        'label' => t('Entity'),
        'type' => 'entity',
        'wrapped' => FALSE,
      ),
      'commerce_discount' => array(
        'label' => t('Commerce Discount'),
        'type' => 'token',
        'options list' => 'commerce_discount_entity_list',
      ),
      'shipping_service' => array(
        'label' => t('Shipping service'),
        'type' => 'token',
        'optional' => TRUE,
        'options list' => 'commerce_shipping_service_options_list',
      ),
    ),
    'base' => 'commerce_discount_percent_off_shipping_service',
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

  $items['commerce_discount_percent_off_shipping'] = array(
    'label' => t('% off of a shipping service'),
    'group' => t('Commerce - discount'),
    'variables' => entity_rules_events_variables('commerce_order', t('Order', array(), array('context' => 'a drupal commerce order'))),
    'access callback' => 'commerce_order_rules_access',
  );

  return $items;
}

/**
 * Build callback for commerce_order_compare_order_amount.
 *
 * @param EntityDrupalWrapper $wrapper
 *   The wrapped entity given by the rule.
 * @param string $operator
 *   The comparison operator.
 * @param array $total
 *   A commerce_price type array.
 *
 * @return bool
 *   return true if condition is valid. false otherwise.
 */
function commerce_order_compare_order_amount_build(EntityDrupalWrapper $wrapper, $operator, $total) {
  $total_order = 0;

  // Ensure the discount currency code is the same than the order.
  if ($wrapper->commerce_order_total->currency_code->value() != $total['currency_code']) {
    return FALSE;
  }

  // Get given total order amount.
  foreach ($wrapper->commerce_line_items as $line_item_wrapper) {
    if ($line_item_wrapper->getBundle() != 'commerce_discount') {
      // Convert the line item's total to the order's currency for totalling.
      $component_total = commerce_price_component_total($line_item_wrapper->commerce_total->value());

      // Add the totals.
      $total_order += commerce_currency_convert(
        $component_total['amount'],
        $component_total['currency_code'],
        $total['currency_code']
      );
    }
  }

  switch ($operator) {
    case '<':
      return $total_order < $total['amount'];

    case '==':
      return $total_order == $total['amount'];

    case '>':
      return $total_order > $total['amount'];

    default:
      return FALSE;
  }
}

/**
 * Build callback for commerce_order_has_owner.
 *
 * @param EntityDrupalWrapper $wrapper
 *   The wrapped entity given by the rule.
 * @param array $account
 *   A fully loaded drupal user.
 *
 * @return bool
 *   Returns true if condition is valid. false otherwise.
 */
function commerce_order_has_owner_build(EntityDrupalWrapper $wrapper, $account) {
  if (isset($account->uid)) {
    // If current logged user matches the discount related users.
    return $account->uid == $wrapper->uid->value();
  }

  return FALSE;
}

/**
 * Build callback for commerce_order_contains_products.
 *
 * @param EntityDrupalWrapper $wrapper
 *   The wrapped entity given by the rule
 * @param string $products
 *   A list of products SKU.
 *
 * @return bool
 *   Returns True if condition is valid. False otherwise.
 */
function commerce_order_contains_products_build(EntityDrupalWrapper $wrapper, $products) {

  $products_sku = explode(', ', (string) $products);

  foreach ($wrapper->commerce_line_items as $wrapper_line_item) {
    // Ensures the passed line item is a product.
    if (in_array('commerce_product', array_keys($wrapper_line_item->getPropertyInfo()))) {
      if (($key = array_search($wrapper_line_item->commerce_product->sku->value(), $products_sku)) !== FALSE) {
        unset($products_sku[$key]);
      }
    }
  }

  return empty($products_sku);
}

/**
 * Build callback for commerce_product_has_type.
 *
 * @param EntityDrupalWrapper $wrapper
 *   Wrapped entity type given by the rule.
 * @param string $type
 *   Product type returned by rule condition.
 *
 * @return bool
 *   True if condition is valid. false otherwise.
 */
function commerce_product_has_type_build(EntityDrupalWrapper $wrapper, $type) {
  // Only for Line items with Product reference field.
  if (in_array('commerce_product', array_keys($wrapper->getPropertyInfo()))) {
    return ($wrapper->commerce_product->type->value() == $type);
  }
  return FALSE;
}

/**
 * Build callback for inline_conditions_product_quantity.
 *
 * Checks if every order line item match the quantity comparison defined in the
 * rule settings.
 *
 * @param EntityDrupalWrapper $wrapper
 *   Wrapped entity given by the rule.
 * @param string $products
 *   A list of products SKU.
 * @param string $operator
 *   String operator used to compare product quantity.
 * @param int $quantity
 *   Product quantity.
 *
 * @return bool
 *   True if the condition is valid. False otherwise.
 */
function commerce_order_has_specific_quantity_products_build(EntityDrupalWrapper $wrapper, $products, $operator, $quantity) {
  $products_sku = explode(', ', (string) $products);
  // Loop on order line items to check if each product has the quantity
  // specified in the rule settings.
  foreach ($wrapper->commerce_line_items as $wrapper_line_item) {
    if (in_array('commerce_product', array_keys($wrapper_line_item->getPropertyInfo()))) {
      if (($key = array_search($wrapper_line_item->commerce_product->sku->value(), $products_sku)) !== FALSE) {
        // At this point, we are sure that the current product is in the order.
        // If this product line item doesn't meet the quantity comparison, the
        // condition will return false.
        switch ($operator) {
          case '<':
            if ($wrapper_line_item->quantity->value() < $quantity) {
              unset($products_sku[$key]);
            }
            else {
              return FALSE;
            }
            break;

          case '==':
            if ($wrapper_line_item->quantity->value() == $quantity) {
              unset($products_sku[$key]);
            }
            else {
              return FALSE;
            }
            break;

          case '>':
            if ($wrapper_line_item->quantity->value() > $quantity) {
              unset($products_sku[$key]);
            }
            else {
              return FALSE;
            }
            break;

        }
      }
    }
  }

  return empty($products_sku);
}

/**
 * Build callback for commerce_product_contains_products.
 *
 * @param EntityDrupalWrapper $wrapper
 *   Wrapped entity type given by the rule.
 * @param string $sku
 *   Product sku(s) returned by rule condition.
 *
 * @return bool
 *   True if condition is valid. false otherwise.
 */
function commerce_product_contains_products_build(EntityDrupalWrapper $wrapper, $sku) {
  // Only for Line items with Product reference field.
  if (in_array('commerce_product', array_keys($wrapper->getPropertyInfo()))) {
    return in_array($wrapper->commerce_product->sku->value(), array_map('trim', explode(',', $sku)));
  }

  return FALSE;
}

/**
 * Build callback for commerce_product_has_specified_terms on product.
 *
 * @param EntityDrupalWrapper $wrapper
 *   Wrapped entity type given by the rule.
 * @param array $terms
 *   Values for the condition settings.
 *
 * @return bool
 *   True is condition is valid. false otherwise.
 */
function commerce_product_has_specified_terms_build(EntityDrupalWrapper $wrapper, $terms) {
  $terms_name = explode(', ', $terms);

  if (in_array('commerce_product', array_keys($wrapper->getPropertyInfo()))) {
    // Fetch all the fields name of taxonomy_term type for the passed entity.
    foreach ($wrapper->commerce_product->getPropertyInfo() as $field_name => $property) {
      if (preg_match('/taxonomy_term/', $property['type'])) {
        // If the wrapped field is an instance of EntityListWrapper class, that
        // means that field contains multiple values.
        if ($wrapper->commerce_product->$field_name instanceof EntityListWrapper) {
          foreach ($wrapper->commerce_product->$field_name as $wrapper_term) {
            if (($key = array_search($wrapper_term->getIdentifier(), $terms_name)) !== FALSE) {
              unset($terms_name[$key]);
            }
          }
        }
        elseif ($term = $wrapper->commerce_product->$field_name->value()) {
          if (($key = array_search($term->tid, $terms_name)) !== FALSE) {
            unset($terms_name[$key]);
          }
        }
      }
    }
  }

  return empty($terms_name);
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

      // Exit if the wrapper doesn't have a commerce_discounts property.
      if (!$wrapper->__isset('commerce_discounts')) {
        return;
      }

      // Set reference to the discount.
      // @todo: It doesn't work with the wrapper.
      $order = $wrapper->value();

      // If the discount will bring the order to less than zero, set the discount
      // amount so that it stops at zero.
      $order_amount = $wrapper->commerce_order_total->amount->value();
      if (-$discount_price['amount'] > $order_amount) {
        $discount_price['amount'] = -$order_amount;
      }

      $delta = $wrapper->commerce_discounts->count();
      $order->commerce_discounts[LANGUAGE_NONE][$delta]['target_id'] = $discount_wrapper->discount_id->value();

      // Modify the existing discount line item or add a new one if that fails.
      if (!commerce_discount_set_existing_line_item_price($wrapper, $discount_name, $discount_price)) {
        commerce_discount_add_line_item($wrapper, $discount_name, $discount_price);
      }

      // Update the total order price, for the next rules condition (if any).
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

      // Do not allow negative line item totals.
      $line_item_amount = $wrapper->commerce_unit_price->amount->value();
      if (-$discount_price['amount'] > $line_item_amount) {
        $discount_price['amount'] = -$line_item_amount;
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

      // Exit if there are no line items or the wrapper doesn't contain
      // the commerce_discounts property.
      if (!$wrapper->commerce_line_items->value() || !$wrapper->__isset('commerce_discounts')) {
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

        // Modify the existing discount line item or add a new one if that fails.
        if (!commerce_discount_set_existing_line_item_price($wrapper, $discount_name, $discount_amount)) {
          commerce_discount_add_line_item($wrapper, $discount_name, $discount_amount);
        }

        // Update the total order price, for the next rules condition (if any).
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
 * Rules action: Apply percent off of shipping discount.
 *
 * @param object $order
 *   Wrapped commerce_order entity type.
 * @param string $discount_name
 *   The name of the discount.
 */
function commerce_discount_percent_off_shipping_service($order, $discount_name) {


  // Load the discount wrapper, which has all the discount information in it.
  $discount_wrapper = entity_metadata_wrapper('commerce_discount', $discount_name);

  // Load the shipping service, if one was selected.
  $shipping_service_to_discount = $discount_wrapper->commerce_discount_offer->commerce_percent_off_ship_serv->value();

  // Get the discount label for later usage.
  $discount_label = $discount_wrapper->value()->label;

  // Determine the discount multiplifer based on submitted value.
  $discount_value = $discount_wrapper->commerce_discount_offer->commerce_percent_off_shipping->value();
  if ($discount_value > 1 && $discount_value <= 100) {
    $discount_multiplier = (100 - $discount_value) / 100;
  }

  // Bring in the $conf global for stashing what shipping services were altered.
  $conf['discounted_shipping_services'] = array();

  // Determine whether to affect a singular service, or all services.
  if (isset($shipping_service_to_discount)) {
    // Add singular shipping service to $conf global array.
    $order->shipping_rates[$shipping_service_to_discount]->data['shipping_service']['description'] .= ' <span class="shipping-discount-text">' . $discount_label . '</span>';

    // Instantiate the line item wrapper.
    $shipping_line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $order->shipping_rates[$shipping_service_to_discount]);

    // Calculate the correct value to discount the line item.
    $discounted_shipping_value = $shipping_line_item_wrapper->commerce_unit_price->amount->value() * $discount_multiplier;

    // The following applies the shipping discount to the line item.
    $shipping_line_item_wrapper->commerce_unit_price->amount = $discounted_shipping_value;
    $shipping_service = commerce_shipping_service_load($discount_name);

    // Updating the data component. Effectively updates line item price!
    $shipping_line_item_wrapper->commerce_unit_price->data = array();
    $shipping_line_item_wrapper->commerce_unit_price->data = commerce_price_component_add(
      $shipping_line_item_wrapper->commerce_unit_price->value(),
      $shipping_service['price_component'],
      $shipping_line_item_wrapper->commerce_unit_price->value(),
      TRUE,
      FALSE
    );
  }
  else{
    foreach ($order->shipping_rates as $shipping_service => $shipping_object) {
      // Add singular shipping service to $conf global array.
      $order->shipping_rates[$shipping_service]->data['shipping_service']['description'] .= ' <span class="shipping-discount-text">' . $discount_label . '</span>';

      // Instantiate the line item wrapper.
      $shipping_line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $order->shipping_rates[$shipping_service]);

      // Calculate the correct value to discount the line item.
      $discounted_shipping_value = $shipping_line_item_wrapper->commerce_unit_price->amount->value() * $discount_multiplier;

      // The following applies the shipping discount to the line item.
      $shipping_line_item_wrapper->commerce_unit_price->amount = $discounted_shipping_value;

      $shipping_service = commerce_shipping_service_load($discount_name);

      // Updating the data component. Effectively updates line item price!
      $shipping_line_item_wrapper->commerce_unit_price->data = array();
      $shipping_line_item_wrapper->commerce_unit_price->data = commerce_price_component_add(
        $shipping_line_item_wrapper->commerce_unit_price->value(),
        $shipping_service['price_component'],
        $shipping_line_item_wrapper->commerce_unit_price->value(),
        TRUE,
        FALSE
      );
    }
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

  // Exit if the wrapper doesn't have a commerce_discounts property.
  if (!$wrapper->__isset('commerce_discounts')) {
    return;
  }

  // Set reference to the discount.
  $order = $wrapper->value();
  $delta = $wrapper->commerce_discounts->count();
  $order->commerce_discounts[LANGUAGE_NONE][$delta]['target_id'] = $discount_wrapper->discount_id->value();

  // Loop on products and add each product to order line items.
  foreach ($products as $product) {
    $context = array(
      'commerce_discount_offer' => 'free_products',
      'discount_name' => $discount_name,
    );

    // Look for a product display that references this project. This is
    // imprecise but covers the vastly most common use case.
    $node_instances = field_info_instances('node');

    foreach ($node_instances as $bundle => $instances) {
      foreach ($instances as $instance) {
        $field = field_info_field($instance['field_name']);

        if ($field['type'] == 'commerce_product_reference') {
          $query = new EntityFieldQuery();
          $results = $query
            ->entityCondition('entity_type', 'node')
            ->fieldCondition($instance['field_name'], 'product_id', $product->product_id)
            ->propertyCondition('status', 1)
            ->execute();

          if (!empty($results['node'])) {
            $nids = array_keys($results['node']);

            // Just take the first one.
            $node = node_load(reset($nids));
            $uri = entity_uri('node', $node);

            // Build the context array in order to attach the display path on
            // product level.
            $context += array(
              'entity_id' => $node->nid,
              'entity_type' => 'node',
              'display_path' => $uri['path'],
            );

            // If we have found something, no need to continue.
            break;
          }
        }
      }
    }

    // This covers use cases where the above logic is insufficient.
    $discount = $discount_wrapper->value();
    drupal_alter('commerce_discount_free_product_context', $context, $product, $discount);

    $data = array('context' => $context);
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

  // Update the total order price, for the next rules condition (if any).
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
  $values = array(
    'type' => 'commerce_discount',
    'order_id' => $order_wrapper->order_id->value(),
    'quantity' => 1,
    // Flag the line item.
    'data' => array('discount_name' => $discount_name),
  );
  $discount_line_item = entity_create('commerce_line_item', $values);
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

  // Add the discount price component.
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
    'amount' => commerce_round(COMMERCE_ROUND_HALF_UP, $discount_amount['amount']),
    'currency_code' => $discount_amount['currency_code'],
    'data' => array(
      'discount_name' => $discount_name,
      'discount_component_title' => empty($component_title) ? 'discount' : $component_title,
    ),
  );

  // Set the new unit price.
  $line_item_wrapper->commerce_unit_price->amount = $updated_amount;
  // Add the discount amount as a price component.
  $price = $line_item_wrapper->commerce_unit_price->value();
  $type = empty($component_title) ? 'discount' : check_plain('discount|' . $discount_name);
  $line_item_wrapper->commerce_unit_price->data = commerce_price_component_add($price, $type, $difference, TRUE, TRUE);

  // Update the line item total.
  _commerce_discount_update_commerce_total($line_item_wrapper);
}

/**
 * Sets a discount price component to the provided line item.
 *
 * @param EntityDrupalWrapper $line_item_wrapper
 *   The wrapped line item entity.
 * @param string $discount_name
 *   The name of the discount being applied.
 * @param array $discount_amount
 *   The discount amount price array (amount, currency_code).
 */
function commerce_discount_set_price_component(EntityDrupalWrapper $line_item_wrapper, $discount_name, $discount_amount) {
  $unit_price = commerce_price_wrapper_value($line_item_wrapper, 'commerce_unit_price', TRUE);
  $discount_wrapper = entity_metadata_wrapper('commerce_discount', $discount_name);
  $component_title = $discount_wrapper->component_title->value();
  // Currencies don't match, abort.
  if ($discount_amount['currency_code'] != $unit_price['currency_code']) {
    return;
  }

  $discount_amount['data'] = array(
    'discount_name' => $discount_name,
    'discount_component_title' => empty($component_title) ? 'discount' : $component_title,
  );

  // Set the new unit price.
  $line_item_wrapper->commerce_unit_price->amount = $discount_amount['amount'];

    // Add the discount amount as a price component.
  $price = $line_item_wrapper->commerce_unit_price->value();
  $type = empty($component_title) ? 'discount' : check_plain('discount|' . $discount_name);
  $line_item_wrapper->commerce_unit_price->data = commerce_price_component_add($price, $type, $discount_amount, TRUE, TRUE);

  // Update the line item total.
  _commerce_discount_update_commerce_total($line_item_wrapper);
}

/**
 * Update commerce_total so that the order total can be refreshed without
 * saving the line item. Taken from CommerceLineItemEntityController::save().
 */
function _commerce_discount_update_commerce_total($line_item_wrapper) {
  $quantity = $line_item_wrapper->quantity->value();

  // Update the total of the line item based on the quantity and unit price.
  $unit_price = commerce_price_wrapper_value($line_item_wrapper, 'commerce_unit_price', TRUE);

  $line_item_wrapper->commerce_total->amount = $quantity * $unit_price['amount'];
  $line_item_wrapper->commerce_total->currency_code = $unit_price['currency_code'];

  // Add the components multiplied by the quantity to the data array.
  if (empty($unit_price['data']['components'])) {
    $unit_price['data']['components'] = array();
  }
  else {
    foreach ($unit_price['data']['components'] as $key => &$component) {
      $component['price']['amount'] *= $quantity;
    }
  }

  // Set the updated data array to the total price.
  $line_item_wrapper->commerce_total->data = $unit_price['data'];
  // Reset the cache because we aren't saving it.
  entity_get_controller('commerce_line_item')->resetCache(array($line_item_wrapper->getIdentifier()));
}

/**
 * Updates the unit price of an existing discount line item.
 *
 * Non-discount line items are ignored.
 *
 * @param EntityDrupalWrapper $order_wrapper
 *   The wrapped order entity.
 * @param string $discount_name
 *   The name of the discount being applied.
 * @param array $discount_price
 *   The discount amount price array (amount, currency_code).
 *
 * @return bool
 *   TRUE if an existing line item was successfully modified, FALSE otherwise.
 */
function commerce_discount_set_existing_line_item_price(EntityDrupalWrapper $order_wrapper, $discount_name, $discount_price) {
  $modified_existing = FALSE;
  foreach ($order_wrapper->commerce_line_items as $line_item_wrapper) {
    if ($line_item_wrapper->getBundle() == 'commerce_discount') {
      // Add the discount component price if the line item was originally
      // added by discount module.
      $line_item = $line_item_wrapper->value();
      if (isset($line_item->data['discount_name']) && $line_item->data['discount_name'] == $discount_name) {
        commerce_discount_set_price_component($line_item_wrapper, $discount_name, $discount_price);
        $line_item_wrapper->save();
        $modified_existing = TRUE;
      }
    }
  }

  return $modified_existing;
}
