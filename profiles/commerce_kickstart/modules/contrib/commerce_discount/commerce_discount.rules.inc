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

  $conditions['commerce_discount_compatibility_check'] = array(
    'label' => t('Check discount compatibility'),
    'parameter' => array(
      'commerce_order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
        'description' => t('The order the discount would be applied to.'),
        'wrapped' => TRUE,
      ),
      'commerce_discount' => array(
        'type' => 'token',
        'label' => t('Discount'),
        'description' => t('A discount with a compatibility strategy that requires evaluation.'),
        'options list' => 'commerce_discount_entity_list',
        'wrapped' => TRUE,
      ),
    ),
    'group' => t('Commerce Discount'),
  );

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
          'options list' => '_commerce_discount_operator_options',
        ),
        'total' => array(
          'type' => 'commerce_price',
          'label' => t('Order amount'),
          'default value' => '',
          'description' => t('The value to compare against the passed order amount.'),
        ),
        'line_item_types' => array(
          'type' => 'list<text>',
          'label' => t('Line item types'),
          'description' => t('Select the line item types that should be included in the order total amount. Discount created line items are always excluded.'),
          'default value' => commerce_order_compare_order_amount_options_default(),
          'options list' => 'commerce_order_compare_order_amount_options_list',
          'optional' => TRUE,
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

  // Add a very simple boolean condition to check whether or not a line item was
  // added to an order via a discount. This should likely be accompanied by an
  // action to load that discount and additional conditions to check the name,
  // type, and offer type of the discount.
  $conditions['commerce_discount_line_item_added_by_discount'] = array(
    'label' => t('Line item added by discount'),
    'parameter' => array(
      'commerce_line_item' => array(
        'type' => 'commerce_line_item',
        'label' => t('Line item'),
      ),
    ),
    'group' => t('Commerce Line Item'),
  );

  return $conditions;
}

/**
 * Implements hook_rules_action_info().
 */
function commerce_discount_rules_action_info() {
  $types = commerce_discount_offer_types();
  $items = array();

  foreach ($types as $info) {
    $items[$info['action']] = array(
      'label' => $info['label'],
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
      'base' => $info['action'],
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

  $items['commerce_discount_shipping_upgrade'] = array(
    'label' => t('Shipping service upgrade'),
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
    ),
    'base' => 'commerce_discount_shipping_upgrade',
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
    'group' => t('Commerce Discount'),
    'variables' => entity_rules_events_variables('commerce_order', t('Order', array(), array('context' => 'a drupal commerce order'))),
    'access callback' => 'commerce_order_rules_access',
  );

  $items['commerce_discount_free_shipping'] = array(
    'label' => t('Makes a shipping service free'),
    'group' => t('Commerce Discount'),
    'variables' => entity_rules_events_variables('commerce_order', t('Order', array(), array('context' => 'a drupal commerce order'))),
    'access callback' => 'commerce_order_rules_access',
  );

  $items['commerce_discount_percent_off_shipping'] = array(
    'label' => t('% off of a shipping service'),
    'group' => t('Commerce Discount'),
    'variables' => entity_rules_events_variables('commerce_order', t('Order', array(), array('context' => 'a drupal commerce order'))),
    'access callback' => 'commerce_order_rules_access',
  );

  return $items;
}

/**
 * Rules callback: executes the "Check discount compatibility" condition.
 *
 * @param \EntityDrupalWrapper $order_wrapper
 *   The order the discount would be applied to.
 * @param \EntityDrupalWrapper $discount_wrapper
 *   The discount whose compatibility needs to be checked.
 */
function commerce_discount_compatibility_check(EntityDrupalWrapper $order_wrapper, $discount_name) {
  // Ensure the discount we're loading still exists.
  if (!$discount = entity_load_single('commerce_discount', $discount_name)) {
    return FALSE;
  }

  // Get the strategy from the compatibility strategy field.
  $strategy = commerce_discount_get_compatibility_strategy($discount);

  // Determine which discounts have been applied to the order thus far.
  $order_total = $order_wrapper->commerce_order_total->value();
  $applied_discounts = commerce_discount_get_discounts_applied_to_price($order_total);

  // Ensure none of them indicate they are incompatible with the current one.
  foreach ($applied_discounts as $applied_discount_id => $applied_discount_name) {
    // Determine the strategy of the existing discount.
    $existing_discount = entity_load_single('commerce_discount', $applied_discount_name);
    $existing_strategy = commerce_discount_get_compatibility_strategy($existing_discount);

    // If the discount is compatible with any other discount, continue on.
    if ($existing_strategy == 'any') {
      continue;
    }

    // If the discount is not compatible with any other discount, return FALSE.
    if ($existing_strategy == 'none') {
      return FALSE;
    }

    // Get the selected discount value from the existing discount.
    $existing_selection = commerce_discount_get_compatibility_selection($existing_discount);

    // If the discount is incompatible with select discounts and the current
    // discount was selected, return FALSE.
    if ($existing_strategy == 'except' && in_array($discount->discount_id, $existing_selection)) {
      return FALSE;
    }

    // If the discount is only compatible with select discounts and the current
    // discount was not selected, return FALSE.
    if ($existing_strategy == 'only' && !in_array($discount->discount_id, $existing_selection)) {
      return FALSE;
    }
  }

  // If the discount is not compatible with any other discount and we found
  // applied discounts, return FALSE.
  if ($strategy == 'none' && count($applied_discounts) > 0) {
    return FALSE;
  }

  // Get the selected discount value from the disocunt being evaluated.
  $selection = commerce_discount_get_compatibility_selection($discount);

  // If the discount is not compatible with selected discounts, ensure they
  // are not on the order.
  if ($strategy == 'except') {
    foreach ($applied_discounts as $applied_discount_id => $applied_discount_name) {
      if (in_array($applied_discount_id, $selection)) {
        return FALSE;
      }
    }
  }

  // If the discount is only compatible with selected discounts, ensure no other
  // discounts are on the order.
  if ($strategy == 'only') {
    foreach ($applied_discounts as $applied_discount_id => $applied_discount_name) {
      if (!in_array($applied_discount_id, $selection)) {
        return FALSE;
      }
    }
  }

  // Return TRUE if nothing has disqualified compatibility yet. For example, if
  // the discount is compatible with any other discount and no other discounts
  // on the order specifies an incompatibility with this discount or if this
  // discount were incompatible with other discounts but none have been applied
  // to the order yet.
  return TRUE;
}

/**
 * Returns an options list for commerce_order_compare_order_amount line item types.
 */
function commerce_order_compare_order_amount_options_list() {
  return array_diff_key(commerce_line_item_type_options_list(), drupal_map_assoc(array('commerce_discount', 'product_discount')));
}

/**
 * Returns a default value array for commerce_order_compare_order_amount line
 * item types.
 */
function commerce_order_compare_order_amount_options_default() {
  return drupal_map_assoc(array_keys(commerce_order_compare_order_amount_options_list()));
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
function commerce_order_compare_order_amount_build(EntityDrupalWrapper $wrapper, $operator, $total, $line_item_types) {
  $total_order = 0;

  // Ensure the discount currency code is the same than the order.
  if ($wrapper->commerce_order_total->currency_code->value() != $total['currency_code']) {
    return FALSE;
  }

  // If $line_item_types is not an array, then we need to total all line item
  // types to preserve backwards compatibility.
  if (!is_array($line_item_types)) {
    $line_item_types = commerce_order_compare_order_amount_options_default();
  }

  // Get given total order amount.
  foreach ($wrapper->commerce_line_items as $line_item_wrapper) {
    if (in_array($line_item_wrapper->getBundle(), $line_item_types)) {
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

    case '<=':
      return $total_order <= $total['amount'];

    case '==':
      return $total_order == $total['amount'];

    case '>':
      return $total_order > $total['amount'];

    case '>=':
      return $total_order >= $total['amount'];

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
 *   The wrapped entity given by the rule.
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
 * Condition callback: looks to see if the line item was added by a discount.
 */
function commerce_discount_line_item_added_by_discount($line_item) {
  return !empty($line_item->data['discount_name']);
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
      if (!isset($wrapper->commerce_discounts)) {
        return;
      }

      // Set reference to the discount.
      // @todo: It doesn't work with the wrapper.
      $order = $wrapper->value();

      // If the discount will bring the order to less than zero, set the
      // discount amount so that it stops at zero.
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
      if (!isset($wrapper->commerce_discounts) || !$wrapper->commerce_line_items->value()) {
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

        // Modify the existing discount line item or add a new line item
        // if that fails.
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
      if (empty($line_item_types[$wrapper->getBundle()])) {
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
 */
function commerce_discount_free_shipping_service($order, $discount_name) {
  // If the order hasn't had any shipping rates calculated for it, exit now as
  // there's nothing to discount.
  if (empty($order->shipping_rates)) {
    return;
  }

  // Load the discount to find the free shipping service and strategy.
  $discount = entity_load_single('commerce_discount', $discount_name);
  $discount_wrapper = entity_metadata_wrapper('commerce_discount', $discount);

  try {
    $free_service = $discount_wrapper->commerce_discount_offer->commerce_free_shipping->raw();
    $free_shipping_strategy = commerce_discount_get_free_shipping_strategy($discount);
  }
  catch (Exception $e) {
    watchdog('commerce_discount', 'Free shipping configuration issue detected on discount %name.', array('%name' => $discount_wrapper->label()));
    return;
  }

  // If a shipping service is specified, ensure it exists.
  if (!empty($free_service)) {
    if (empty($order->shipping_rates[$free_service])) {
      return;
    }

    // Exit if the shipping service is already free.
    $free_service_wrapper = entity_metadata_wrapper('commerce_line_item', $order->shipping_rates[$free_service]);

    if ($free_service_wrapper->commerce_unit_price->amount->value() == 0) {
      return;
    }

    // Update the free shipping service line item's rate to be free using the
    // service's price component type but a price array that will ensure the
    // discount title gets swapped in for the price component when the rate is
    // formatted accordingly.
    $discount_amount = $free_service_wrapper->commerce_unit_price->amount->value();
    $free_service_wrapper->commerce_unit_price->amount = 0;
    $order->shipping_rates[$free_service]->data['shipping_service']['description'] .= ' <span class="shipping-discount-text">' . check_plain($discount->component_title) . '</span>';

    $shipping_service = commerce_shipping_service_load($free_service);

    $discount_price = array(
      'amount' => -$discount_amount,
      'currency_code' => $free_service_wrapper->commerce_unit_price->currency_code->value(),
      'data' => array(
        'discount_component_title' => check_plain($discount->component_title),
      ),
    );

    $free_service_wrapper->commerce_unit_price->data = commerce_price_component_add(
      $free_service_wrapper->commerce_unit_price->value(),
      $shipping_service['price_component'],
      $discount_price,
      TRUE,
      FALSE
    );

    // If the free shipping discount strategy requires us to discount all
    // other shipping services an equivalent amount...
    if ($free_shipping_strategy == 'discount_all') {
      foreach ($order->shipping_rates as $service_name => $line_item) {
        // Skip the free shipping service.
        if ($service_name == $free_service) {
          continue;
        }

        // Apply the discount, ensuring the rate does not discount below 0,
        // but do not use the message again to reduce spam on the form.
        $line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $line_item);

        $service_discount_amount = min($discount_amount, $line_item_wrapper->commerce_unit_price->amount->value());
        $line_item_wrapper->commerce_unit_price->amount = $line_item_wrapper->commerce_unit_price->amount->value() - $service_discount_amount;

        $shipping_service = commerce_shipping_service_load($service_name);

        $discount_price = array(
          'amount' => -$service_discount_amount,
          'currency_code' => $line_item_wrapper->commerce_unit_price->currency_code->value(),
          'data' => array(
            'discount_component_title' => check_plain($discount->component_title),
          ),
        );

        $line_item_wrapper->commerce_unit_price->data = commerce_price_component_add(
          $line_item_wrapper->commerce_unit_price->value(),
          $shipping_service['price_component'],
          $discount_price,
          TRUE,
          FALSE
        );
      }
    }
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

  // Get the discount component title for later usage.
  $discount_label = check_plain($discount_wrapper->component_title->value());

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
  else {
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
 * Rules action: apply a shipping service upgrade discount.
 *
 * @param object $order
 *   The order object whose discounts are being applied.
 * @param string $discount_name
 *   The name of the shipping service upgrade discount.
 */
function commerce_discount_shipping_upgrade($order, $discount_name) {
  // If the order hasn't had any shipping rates calculated for it, exit now as
  // there's nothing to discount.
  if (empty($order->shipping_rates)) {
    return;
  }

  // Load the discount to find the target and source services.
  $discount = entity_load_single('commerce_discount', $discount_name);
  $discount_wrapper = entity_metadata_wrapper('commerce_discount', $discount);

  try {
    $target_service = $discount_wrapper->commerce_discount_offer->commerce_shipping_upgrade_target->raw();
    $source_service = $discount_wrapper->commerce_discount_offer->commerce_shipping_upgrade_source->raw();
  }
  catch (Exception $e) {
    watchdog('commerce_discount', 'Shipping service upgrade configuration issue detected on discount %name.', array('%name' => $discount_wrapper->label()));
    return;
  }

  // Exit now if we either could not determine a target or source service or
  // if they were not calculated for this order.
  if (empty($target_service) || empty($order->shipping_rates[$target_service]) ||
    empty($source_service) || empty($order->shipping_rates[$source_service])) {
    return;
  }

  // Get the calculated rates from both items.
  $target_line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $order->shipping_rates[$target_service]);
  $source_line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $order->shipping_rates[$source_service]);

  // If the prices are already the same, exit now.
  if ($target_line_item_wrapper->commerce_unit_price->amount->value() == $source_line_item_wrapper->commerce_unit_price->amount->value()) {
    return;
  }

  // Otherwise, calculate the difference between the source and the target.
  $difference = $source_line_item_wrapper->commerce_unit_price->amount->value() - $target_line_item_wrapper->commerce_unit_price->amount->value();

  // Update the target shipping service line item's rate to match the source
  // line item's rate using the price component type of the target rate but
  // a price array that will ensure the discount title gets swapped in for the
  // price component when the rate is formatted accordingly.
  $target_line_item_wrapper->commerce_unit_price->amount = $source_line_item_wrapper->commerce_unit_price->amount->value();
  $order->shipping_rates[$target_service]->data['shipping_service']['description'] .= ' <span class="shipping-discount-text">' . check_plain($discount->component_title) . '</span>';

  $shipping_service = commerce_shipping_service_load($target_service);

  $discount_price = array(
    'amount' => $difference,
    'currency_code' => $target_line_item_wrapper->commerce_unit_price->currency_code->value(),
    'data' => array(
      'discount_component_title' => check_plain($discount->component_title),
    ),
  );

  $target_line_item_wrapper->commerce_unit_price->data = commerce_price_component_add(
    $target_line_item_wrapper->commerce_unit_price->value(),
    $shipping_service['price_component'],
    $discount_price,
    TRUE,
    FALSE
  );
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
  if (!isset($wrapper->commerce_discounts)) {
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
 * @param array $data
 *   Optional. An associative array of parameters to include in the line item's
 *   data array along with the discount name.
 */
function commerce_discount_add_line_item(EntityDrupalWrapper $order_wrapper, $discount_name, $discount_amount, $data = array()) {
  // Create a new line item.
  $values = array(
    'type' => 'commerce_discount',
    'order_id' => $order_wrapper->order_id->value(),
    'quantity' => 1,
    // Flag the line item.
    'data' => array('discount_name' => $discount_name) + $data,
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
  $discount_amount['amount'] = commerce_round(COMMERCE_ROUND_HALF_UP, $discount_amount['amount']);

  $updated_amount = $current_amount + $discount_amount['amount'];

  $difference = array(
    'amount' => $discount_amount['amount'],
    'currency_code' => $discount_amount['currency_code'],
    'data' => array(
      'discount_name' => $discount_name,
      'discount_component_title' => empty($component_title) ? t('Discount') : $component_title,
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
 * Update commerce_total without saving line item.
 *
 * To have the order total refreshed without saving the line item.
 * Taken from CommerceLineItemEntityController::save().
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

/**
 * Options list callback for condition.
 */
function _commerce_discount_operator_options() {
  return array(
    '<' => t('less than'),
    '<=' => t('less than or equal to'),
    '==' => t('equals'),
    '>' => t('greater than'),
    '>=' => t('greater than or equal to'),
  );
}
