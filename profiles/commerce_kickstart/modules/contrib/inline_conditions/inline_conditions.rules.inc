<?php

/**
 * Implements hook_rules_condition_info().
 */
function inline_conditions_rules_condition_info() {
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
          'options list' => '_inline_conditions_order_operator_options',
        ),
        'total' => array(
          'type' => 'commerce_price',
          'label' => t('Order amount'),
          'default value' => '',
          'description' => t('The value to compare against the passed order amount.'),
        ),
      ),
      'module' => 'inline_conditions',
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
      'module' => 'inline_conditions',
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
      'module' => 'inline_conditions',
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
      'module' => 'inline_conditions',
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
      'module' => 'inline_conditions',
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
      'module' => 'inline_conditions',
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
      'module' => 'inline_conditions',
      'group' => t('Commerce Line Item'),
      'callbacks' => array(
        'execute' => $inline_conditions['commerce_product_has_specified_terms']['callbacks']['build'],
      ),
    );
  }

  return $conditions;
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
 * Options list callback for condition commerce_order_compare_order_amount.
 */
function _inline_conditions_order_operator_options() {
  return array(
    '<' => t('lower than'),
    '==' => t('equals'),
    '>' => t('greater than'),
  );
}
