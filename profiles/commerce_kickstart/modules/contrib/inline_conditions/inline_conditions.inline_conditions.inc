<?php

/**
 * Implements hook_inline_conditions_info().
 */
function inline_conditions_inline_conditions_info() {
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

    $conditions['commerce_order_has_specific_quantity_products'] = array(
      'label' => t('Product(s) and quantity'),
      'entity type' => 'commerce_order',
      'callbacks' => array(
        'configure' => 'commerce_order_has_specific_quantity_products_configure',
        'build' => 'commerce_order_has_specific_quantity_products_build',
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
function inline_conditions_inline_conditions_build_alter(&$value) {
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
    '#suffix' => '<div class="condition-instructions">' . t('The discount is active only if the total order amount matches the condition above.') . '</div>',
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

  if (!array_key_exists($element['#value'], $shipping_services)) {
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

  if (!empty($settings)) {
    foreach ($settings['sku'] as $delta => $product_id) {
      $product = commerce_product_load(reset($product_id));
      $default_value .= $product->sku;
      if ($product_id !== end($settings['sku'])) {
        $default_value .= ', ';
      }
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

  $default_value = '';
  if (!empty($settings['terms'])) {
    foreach ($settings['terms'] as $delta => $term) {
      $default_value .= taxonomy_term_load($term['target_id'])->name . ' (' . $term['target_id'] . ')';
      if ($term != end($settings['terms'])) {
        $default_value .= ', ';
      }
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
