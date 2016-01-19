<?php

/**
 * @file
 * Provides Inline Conditions integration for the Commerce Discount module.
 */

/**
 * Implements hook_inline_conditions_info().
 */
function commerce_discount_inline_conditions_info() {
  $conditions = array();

  if (module_exists('commerce_order')) {
    $conditions['commerce_order_compare_order_amount'] = array(
      'label' => t('Total amount'),
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

    $conditions['commerce_product_has_owner'] = array(
      'label' => t('User'),
      'entity type' => 'commerce_line_item',
      'rule condition name' => 'data_is',
      'callbacks' => array(
        'configure' => 'commerce_product_has_owner_configure',
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

    $conditions['commerce_order_compare_address'] = array(
      'label' => t('Address'),
      'entity type' => 'commerce_order',
      'callbacks' => array(
        'configure' => 'commerce_order_compare_address_configure',
        'build' => 'commerce_order_rules_compare_address',
      ),
    );
  }

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

  if (module_exists('commerce_product')) {
    $conditions['commerce_product_contains_products'] = array(
      'label' => t('Product(s)'),
      'entity type' => 'commerce_line_item',
      'callbacks' => array(
        'configure' => 'commerce_product_contains_products_configure',
        'build' => 'commerce_product_contains_products_build',
      ),
    );
    $conditions['commerce_product_has_type'] = array(
      'label' => t('Product type'),
      'entity type' => 'commerce_line_item',
      'callbacks' => array(
        'configure' => 'commerce_product_has_type_configure',
        'build' => 'commerce_product_has_type_build',
      ),
    );
  }

  if (module_exists('taxonomy')) {
    $conditions['commerce_product_has_specified_terms'] = array(
      'label' => t('Product attributes'),
      'entity type' => 'commerce_line_item',
      'callbacks' => array(
        'configure' => 'commerce_product_has_specified_terms_configure',
        'build' => 'commerce_product_has_specified_terms_build',
      ),
    );
  }

  return $conditions;
}

/**
 * Implements hook_inline_conditions_build_alter().
 */
function commerce_discount_inline_conditions_build_alter(&$value) {
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

    case 'commerce_product_has_specified_terms':
      $terms = $value['condition_settings']['terms'];

      $value['condition_settings']['terms'] = '';
      foreach ($terms as $term) {
        $value['condition_settings']['terms'] .= reset($term);
        if ($term !== end($terms)) {
          $value['condition_settings']['terms'] .= ', ';
        }
      }
      break;

    case 'commerce_product_contains_products':
      $entity_ids = array();
      foreach ($value['condition_settings']['sku'] as $delta) {
        $entity_ids[] = reset($delta);
      }
      $products = commerce_product_load_multiple($entity_ids);

      $value['condition_settings']['sku'] = '';
      foreach ($products as $product) {
        $value['condition_settings']['sku'] .= $product->sku;
        if ($product !== end($products)) {
          $value['condition_settings']['sku'] .= ', ';
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

  module_load_include('inc', 'commerce_discount', 'commerce_discount.rules');

  // Ensure we have default values for the condition settings.
  $settings += array(
    'operator' => '>=',
    'total' => array('amount' => 0),
    'line_item_types' => commerce_order_compare_order_amount_options_default(),
  );

  // Get the default website currency.
  $default_currency = commerce_currency_load(NULL);

  $form['line_item_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Included line item types'),
    '#title_display' => 'invisible',
    '#options' => commerce_order_compare_order_amount_options_list(),
    '#default_value' => $settings['line_item_types'],
    '#multiple' => TRUE,
  );

  $form['operator'] = array(
    '#type' => 'select',
    '#title' => t('Operator'),
    '#title_display' => 'invisible',
    '#options' => _commerce_discount_operator_options(),
    '#default_value' => $settings['operator'],
  );

  $form['total'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#element_validate' => array('commerce_price_field_widget_validate'),
    '#suffix' => '<div class="condition-instructions">' . t('The discount is active only if the total of the selected line items on the order passes the comparison condition.<br />(Other order / product discount line items are not taken into account in the comparison process.)') . '</div>',
  );

  $form['total']['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Order subtotal'),
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
 * Configuration callback for commerce_order_has_owner.
 *
 * @param array $settings
 *   An array of rules condition settings.
 *
 * @return array
 *   A form element array.
 */
function commerce_order_has_owner_configure($settings) {
  $form = array();

  // Ensure we've default settings set.
  $settings += array(
    'account' => 0,
  );

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
 * Configuration callback for commerce_product_has_owner.
 *
 * @param array $settings
 *   An array of rules condition settings.
 *
 * @return array
 *   A form element array.
 */
function commerce_product_has_owner_configure($settings) {
  $form = array();

  // Ensure we've default settings set.
  $settings += array(
    'value' => 0,
  );

  // The bellow values (data:select & op) are added because they are required by
  // rule condition.
  $form['data:select'] = array(
    '#type' => 'textfield',
    '#access' => FALSE,
    '#default_value' => 'site:current-user:uid',
  );

  $form['op'] = array(
    '#type' => 'textfield',
    '#access' => FALSE,
    '#default_value' => '==',
  );

  $form['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Users'),
    '#title_display' => 'invisible',
    '#default_value' => isset($settings['value']) ? user_load($settings['value'])->name : '',
    '#required' => TRUE,
    '#autocomplete_path' => 'user/autocomplete',
    '#element_validate' => array('_commerce_order_has_owner_validate'),
    '#suffix' => '<div class="condition-instructions">' . t('Discount is active if the selected users are currently logged.') . '</div>',
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
 * Configuration callback for commerce_order_contains_products.
 *
 * @param array $settings
 *   Values for the form element.
 *
 * @return array
 *   Return a form element.
 */
function commerce_order_contains_products_configure($settings) {
  // Ensure we've default settings set.
  $settings += array(
    'products' => array(),
  );

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
    '#maxlength' => 4096,
    '#autocomplete_path' => 'commerce_product/autocomplete/commerce_product/0/0',
    '#element_validate' => array('commerce_product_reference_autocomplete_validate'),
    '#suffix' => '<div class="condition-instructions">' . t('Select products when ordered make discount active.') . '</div>',
    '#attributes' => array('placeholder' => array(t('enter product name'))),
  );

  return $form;
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
  // Ensure we've default settings set.
  $settings += array(
    'products' => array(),
    'operator' => '==',
    'quantity' => '',
  );

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
    '#maxlength' => 4096,
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
    '#suffix' => '<div class="condition-instructions">' . t("The discount is active if the order's product(s) match the condition above.") . '</div>',
  );

  return $form;
}

/**
 * Configuration callback for commerce_order_compare_address_configure.
 *
 * @param array $settings
 *   An array of rules condition settings.
 *
 * @return array;
 *   A form element array.
 */
function commerce_order_compare_address_configure($settings) {
  $form = array();

  module_load_include('inc', 'commerce_order', 'commerce_order.rules');

  // Need a list of all address fields on customer profile bundles.
  $form['address_field'] = array(
    '#type' => 'select',
    '#multiple' => FALSE,
    '#options' => commerce_order_address_field_options_list(),
    '#title' => t('Address field'),
    '#title_display' => 'invisible',
    '#default_value' => !empty($settings['address_field']) ? $settings['address_field'] : FALSE,
    '#require' => TRUE,
  );

  // Need a list of address component options to compare.
  $form['address_component'] = array(
    '#type' => 'select',
    '#multiple' => FALSE,
    '#options' => commerce_order_address_component_options_list(),
    '#title' => t('Address component'),
    '#title_display' => 'invisible',
    '#default_value' => !empty($settings['address_component']) ? $settings['address_component'] : FALSE,
    '#require' => TRUE,
  );

  $form['operator'] = array(
    '#type' => 'select',
    '#multiple' => FALSE,
    '#options' => commerce_order_address_comparison_operator_options_list(),
    '#title' => t('Operator'),
    '#title_display' => 'invisible',
    '#default_value' => !empty($settings['operator']) ? $settings['operator'] : FALSE,
    '#require' => TRUE,
  );

  $form['value'] = array(
    '#type' => 'textarea',
    '#title' => t('Value'),
    '#title_display' => 'invisible',
    '#rows' => 3,
    '#default_value' => !empty($settings['value']) ? $settings['value'] : FALSE,
    '#require' => TRUE,
    '#suffix' => '<div class="condition-instructions">' . t('The discount is active if the order address component selected matches the entered value. For the entered value bear in mind that addresses using select lists for various components may use a value different from the option you select. For example, countries are selected by name, but the value is the two letter abbreviation. For comparisons with multiple possible values, place separate values on new lines.') . '</div>',
  );

  return $form;
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
    '#suffix' => '<div class="condition-instructions">' . t('The discount is active if the order shipping service match the selected one above.') . '</div>',
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

  if (!empty($shipping_services) && $element['#value'] !== FALSE && !array_key_exists($element['#value'], $shipping_services)) {
    form_set_error(implode('][', $element['#array_parents']), t('A correct shipping service must be selected'));
  }
}

/**
 * Configuration callback for commerce_product_contains_products.
 *
 * @param array $settings
 *   Values for the form element.
 *
 * @return array
 *   Return a form element.
 */
function commerce_product_contains_products_configure($settings) {
  $form = array();
  $default_value = '';

  // Ensure we've default settings set.
  $settings += array(
    'sku' => array(),
  );

  foreach ($settings['sku'] as $delta => $product_id) {
    $product = commerce_product_load(reset($product_id));
    $default_value .= $product->sku;
    if ($product_id !== end($settings['sku'])) {
      $default_value .= ', ';
    }
  }

  $form['sku'] = array(
    '#type' => 'textfield',
    '#title' => t('Product title'),
    '#title_display' => 'invisible',
    '#default_value' => $default_value,
    '#required' => TRUE,
    '#maxlength' => 4096,
    '#autocomplete_path' => 'commerce_product/autocomplete/commerce_product/0/0',
    '#element_validate' => array('commerce_product_reference_autocomplete_validate'),
    '#suffix' => '<div class="condition-instructions">' . t('The discount is active if the line item is a product with the selected SKU.') . '</div>',
  );

  return $form;
}

/**
 * Configuration callback for commerce_product_has_type.
 *
 * @param array $settings
 *   Values for the form element.
 *
 * @return array
 *   Return a form element.
 */
function commerce_product_has_type_configure($settings) {
  $form = array();

  $form['type'] = array(
    '#type' => 'select',
    '#multiple' => FALSE,
    '#options' => commerce_product_type_options_list(),
    '#title' => t('Product type'),
    '#title_display' => 'invisible',
    '#default_value' => !empty($settings['type']) ? $settings['type'] : FALSE,
    '#require' => TRUE,
    '#element_validate' => array('_commerce_product_has_type_validate'),
    '#suffix' => '<div class="condition-instructions">' . t('The discount is active if the product has the type selected above.') . '</div>',
  );

  return $form;
}

/**
 * Element validation callback.
 *
 * Ensures passed element is a valid commerce product type.
 */
function _commerce_product_has_type_validate($element, &$form_state, $form) {
  $product_types = commerce_product_types();

  if (!array_key_exists($element['#value'], $product_types)) {
    form_set_error(implode('][', $element['#array_parents']), t('A correct product type must be selected'));
  }
}

/**
 * Configuration callback for commerce_product_has_specified_terms on product.
 *
 * @param array $settings
 *   Values for the form element.
 *
 * @return array
 *   Return a form element.
 */
function commerce_product_has_specified_terms_configure($settings) {
  $form = array();

  // Ensure we've default settings set.
  $settings += array(
    'terms' => array(),
  );

  $default_value = '';
  foreach ($settings['terms'] as $delta => $term) {
    $default_value .= taxonomy_term_load($term['target_id'])->name . ' (' . $term['target_id'] . ')';
    if ($term != end($settings['terms'])) {
      $default_value .= ', ';
    }
  }

  $form['terms'] = array(
    '#type' => 'textfield',
    '#title' => t('Terms'),
    '#title_display' => 'invisible',
    '#required' => TRUE,
    '#maxlength' => 4096,
    '#default_value' => $default_value,
    '#autocomplete_path' => 'inline_conditions/autocomplete/taxonomy_term/1/0',
    '#element_validate' => array('_inline_conditions_autocomplete_validate'),
    '#suffix' => '<div class="condition-instructions">' . t('The discount is active if the product has the selected term(s).') . '</div>',
  );

  return $form;
}
