<?php
/**
 * @file
 * Provides Rules and Inline Conditions integration for commerce_order module.
 *
 * @requirements
 * commerce_order, commerce_price.
 */

/**
 * Implements hook_inline_conditions_info().
 */
function commerce_order_inline_conditions_info() {
  $conditions = array();

  $conditions['commerce_order_compare_order_amount'] = array(
    'label' => t('Amount'),
    'entity type' => 'commerce_order',
    'callbacks' => array(
      'configure' => 'commerce_order_compare_order_amount_configure',
      'build' => 'commerce_order_compare_order_amount_build',
    ),
  );

  $conditions['commerce_order_has_owner'] = array(
    'label' => t('User'),
    'entity type' => 'commerce_order',
    'callbacks' => array(
      'configure' => 'commerce_order_has_owner_configure',
      'build' => 'commerce_order_has_owner_build',
    ),
  );

  $conditions['commerce_order_contains_products'] = array(
    'label' => t('Product(s)'),
    'entity type' => 'commerce_order',
    'callbacks' => array(
      'configure' => 'commerce_order_contains_products_configure',
      'build' => 'commerce_order_contains_products_build',
    ),
  );

  $conditions['commerce_order_has_specific_quantity_products'] = array(
    'label' => t('Product(s) and quantity'),
    'entity type' => 'commerce_order',
    'callbacks' => array(
      'configure' => 'commerce_order_has_specific_quantity_products_configure',
      'build' => 'commerce_order_has_specific_quantity_products_build',
    ),
  );

  if (module_exists('commerce_shipping')) {
    $conditions['commerce_shipping_compare_shipping_service'] = array(
      'label' => t('Shipping service'),
      'entity type' => 'commerce_order',
      'callbacks' => array(
        'configure' => 'commerce_shipping_compare_shipping_service_configure',
        'build' => 'commerce_shipping_rules_line_item_exists',
      ),
    );
  }

  return $conditions;
}


/**
 * Implements hook_inline_conditions_build_alter().
 */
function commerce_order_inline_conditions_build_alter(&$value) {
  switch ($value['condition_name']) {
    case 'commerce_order_contains_products':
    case 'commerce_order_has_specific_quantity_products':
      $entity_ids = array();
      foreach ($value['condition_settings']['products'] as $delta) {
        $entity_ids[] = reset($delta);
      }
      $products = commerce_product_load_multiple($entity_ids);

      $value['condition_settings']['products'] = '';
      foreach ($products as $product) {
        $value['condition_settings']['products'] .= $product->sku;
        if ($product !== end($products)) {
          $value['condition_settings']['products'] .= ', ';
        }
      }
      break;
  }
}

/**
 * Configuration callback for commerce_order_compare_order_amount.
 *
 * @param array $settings
 *   An array of rules condition settings.
 *
 * @return array;
 *   A form element array.
 */
function commerce_order_compare_order_amount_configure($settings) {
  $form = array();

  // Get the default website currency.
  $default_currency = commerce_currency_load(NULL);

  $form['operator'] = array(
    '#type' => 'select',
    '#title' => t('Operator'),
    '#title_display' => 'invisible',
    '#options' => array(
      '<' => t('lower than'),
      '==' => t('equals'),
      '>' => t('greater than'),
    ),
    '#default_value' => !empty($settings['operator']) ? $settings['operator'] : '>=',
  );

  $form['total'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#element_validate' => array('commerce_price_field_widget_validate'),
    '#suffix' => '<div class="condition-instructions">' . t('Enter the minimum order amount to activate the discount.') . '</div>',
  );

  $form['total']['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Order total'),
    '#title_display' => 'invisible',
    '#default_value' => commerce_currency_amount_to_decimal($settings['total']['amount'], $default_currency['code']),
    '#size' => 10,
    '#field_suffix' => $default_currency['code'],
    '#require' => TRUE,
  );

  $form['total']['currency_code'] = array(
    '#type' => 'value',
    '#default_value' => $default_currency['code'],
  );

  return $form;
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
  // Get given total order amount.
  $total_order = $wrapper->commerce_order_total->value();

  // Ensures currency codes match.
  if ($total['currency_code'] != $total_order['currency_code']) {
    return FALSE;
  }

  switch ($operator) {
    case '<':
      return $total_order['amount'] < $total['amount'];

    case '==':
      return $total_order['amount'] == $total['amount'];

    case '>':
      return $total_order['amount'] > $total['amount'];

    default:
      return FALSE;
  }
}

/**
 * Configuration callback for commerce_order_has_owner.
 *
 * @param array $settings
 *   An array of rules condition settings.
 *
 * @return array;
 *   A form element array.
 */
function commerce_order_has_owner_configure($settings) {
  $form = array();

  $form['account'] = array(
    '#type' => 'textfield',
    '#title' => t('Users'),
    '#title_display' => 'invisible',
    '#default_value' => ($account = user_load($settings['account'])) ? $account->name : '',
    '#required' => TRUE,
    '#autocomplete_path' => 'user/autocomplete',
    '#element_validate' => array('_commerce_order_has_owner_validate'),
    '#suffix' => '<div class="condition-instructions">' . t('Discount is active if the selected user is the order owner.') . '</div>',
  );

  return $form;
}


/**
 * Validation callback for a commerce_order_has_owner autocomplete element.
 */
function _commerce_order_has_owner_validate($element, &$form_state, $form) {
  // Try to fetch user.
  $user = user_load_by_name($element['#value']);

  if (!empty($user)) {
    $value = $user->uid;
    form_set_value($element, $value, $form_state);
  }
  else {
    form_set_error(implode('][', $element['#array_parents']), t('Enter a correct username.'));
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
 * Configuration callback for commerce_order_contains_products.
 *
 * @param array $settings
 *   Values for the form element.
 *
 * @return array
 *   Return a form element.
 */
function commerce_order_contains_products_configure($settings) {
  // Get values from $settings.
  $default_value = '';
  foreach ($settings['products'] as $delta => $product_id) {
    $product = commerce_product_load(reset($product_id));
    $default_value .= $product->sku;
    if ($product_id !== end($settings['products'])) {
      $default_value .= ', ';
    }
  }

  $form = array();

  $form['products'] = array(
    '#type' => 'textfield',
    '#title' => t('SKUs'),
    '#title_display' => 'invisible',
    '#default_value' => $default_value,
    '#required' => TRUE,
    '#autocomplete_path' => 'commerce_product/autocomplete/commerce_product/0/0',
    '#element_validate' => array('commerce_product_reference_autocomplete_validate'),
    '#suffix' => '<div class="condition-instructions">' . t('Select products when ordered make discount active.') . '</div>',
    '#attributes' => array('placeholder' => array(t('enter product name'))),
  );

  return $form;
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

  // Compare discount product ids with thoses of given order.
  foreach ($wrapper->commerce_line_items as $wrapper_line_item) {
    // Ensures that the type of current line item is product.
    if ($wrapper_line_item->getBundle() == 'product') {
      if (($key = array_search($wrapper_line_item->commerce_product->sku->value(), $products_sku)) !== FALSE) {
        unset($products_sku[$key]);
      }
    }
  }

  return empty($products_sku);
}

/**
 * Configure callback for commerce_order_has_specific_quantity_products.
 *
 * @param array $settings
 *   Values for the form element.
 *
 * @return array
 *   Return the form element to display.
 */
function commerce_order_has_specific_quantity_products_configure($settings) {
  // Get product IDs from $settings.
  $default_value = '';
  foreach ($settings['products'] as $delta => $product_id) {
    $product = commerce_product_load(reset($product_id));
    $default_value .= $product->sku;
    if ($product_id !== end($settings['products'])) {
      $default_value .= ', ';
    }
  }

  $form = array();

  $form['products'] = array(
    '#type' => 'textfield',
    '#title' => t('Products'),
    '#title_display' => 'invisible',
    '#default_value' => $default_value,
    '#required' => TRUE,
    '#autocomplete_path' => 'commerce_product/autocomplete/commerce_product/0/0',
    '#element_validate' => array('commerce_product_reference_autocomplete_validate'),
    '#attributes' => array('placeholder' => array(t('enter product name'))),
  );

  $form['operator'] = array(
    '#type' => 'select',
    '#title' => t('Operator'),
    '#title_display' => 'invisible',
    '#options' => array(
      '<' => t('quantity lower than'),
      '==' => t('quantity equals'),
      '>' => t('quantity greater than'),
    ),
    '#default_value' => !empty($settings['operator']) ? $settings['operator'] : '==',
  );

  $form['quantity'] = array(
    '#type' => 'textfield',
    '#title' => t('Quantity'),
    '#title_display' => 'invisible',
    '#default_value' => !empty($settings['quantity']) ? $settings['quantity'] : '',
    '#size' => 5,
    '#required' => TRUE,
    '#element_validate' => array('element_validate_integer'),
    '#suffix' => '<div class="condition-instructions">' . t('Enter product(s) and the quantity to be checked.') . '</div>',
  );

  return $form;
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
    if ($wrapper_line_item->getBundle() == 'product') {
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
 * Configuration callback for commerce_shipping_compare_shipping_service.
 *
 * @param array $settings
 *   An array of rules condition settings.
 *
 * @return array;
 *   A form element array.
 */
function commerce_shipping_compare_shipping_service_configure($settings) {
  $form = array();

  $form['service'] = array(
    '#type' => 'select',
    '#multiple' => FALSE,
    '#options' => commerce_shipping_service_options_list(),
    '#title' => t('Shipping service'),
    '#title_display' => 'invisible',
    '#default_value' => !empty($settings['service']) ? $settings['service'] : FALSE,
    '#require' => TRUE,
    '#element_validate' => array('_commerce_shipping_compare_shipping_service_validate'),
    '#suffix' => '<div class="condition-instructions">' . t('The discount will be enable only if the shipping service is selected by customer.') . '</div>',
  );

  return $form;
}

/**
 * Element validation callback.
 *
 * Ensures passed element is a valid commerce shipping service.
 */
function _commerce_shipping_compare_shipping_service_validate($element, &$form_state, $form) {
  $shipping_services = commerce_shipping_services();

  if (!array_key_exists($element['#value'], $shipping_services)) {
    form_set_error(implode('][', $element['#array_parents']), t('A correct shipping service must be selected'));
  }
}

/**
 * Implements hook_rules_condition_info_alter().
 *
 * Adds new rule conditions to commerce_order entity type.
 */
function commerce_order_rules_condition_info_alter(&$conditions) {
  $inline_conditions = inline_conditions_get_info();

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
    'module' => 'commerce_order',
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
    'module' => 'commerce_order',
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
    'module' => 'commerce_order',
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
    'module' => 'commerce_order',
    'group' => t('Commerce Order'),
    'callbacks' => array(
      'execute' => $inline_conditions['commerce_order_has_specific_quantity_products']['callbacks']['build'],
    ),
  );
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
