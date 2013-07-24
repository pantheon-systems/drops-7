<?php
/**
 * @file
 * Provides Rules and Inline Conditions integration for commerce_product module.
 */

/**
 * Implements hook_inline_conditions_info().
 */
function commerce_product_inline_conditions_info() {
  $conditions = array();

  $conditions['commerce_product_contains_products'] = array(
    'label' => t('Product(s)'),
    'entity type' => 'commerce_line_item',
    'callbacks' => array(
      'configure' => 'commerce_product_contains_products_configure',
      'build' => 'commerce_product_contains_products_build',
    ),
  );

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
function commerce_product_inline_conditions_build_alter(&$value) {
  switch ($value['condition_name']) {
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
    '#autocomplete_path' => 'commerce_product/autocomplete/commerce_product/0/0',
    '#element_validate' => array('commerce_product_reference_autocomplete_validate'),
    '#suffix' => '<div class="condition-instructions">' . t('The discount is active if the line item is a product with the selected SKU.') . '</div>',
  );

  return $form;
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
    '#default_value' => $default_value,
    '#autocomplete_path' => 'inline_conditions/autocomplete/taxonomy_term/1/0',
    '#element_validate' => array('_inline_conditions_autocomplete_validate'),
    '#suffix' => '<div class="condition-instructions">' . t('The discount is active if the product has the selected term(s).') . '</div>',
  );

  return $form;
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

  if ($wrapper->getBundle() == 'product') {
    // Fetch all the fields name of taxonomy_term type for the current
    // product type.
    $term_fields = array();
    foreach ($wrapper->commerce_product->getPropertyInfo() as $field_name => $property) {
      if (preg_match('/taxonomy_term/', $property['type'])) {
        $term_fields[] = $field_name;
      }
    }

    foreach ($term_fields as $term_field) {
      // If the wrapped field is an instance of EntityListWrapper class, that
      // means that field contains multiple values.
      if ($wrapper->commerce_product->$term_field instanceof EntityListWrapper) {
        foreach ($wrapper->commerce_product->$term_field as $wrapper_term) {
          if (($key = array_search($wrapper_term->getIdentifier(), $terms_name)) !== FALSE) {
            unset($terms_name[$key]);
          }
        }
      }
      else {
        $term = $wrapper->commerce_product->$term_field->value();
        if (($key = array_search($term->tid, $terms_name)) !== FALSE) {
          unset($terms_name[$key]);
        }
      }
    }
  }

  return empty($terms_name);
}

/**
 * Implements hook_rules_condition_info_alter().
 *
 * Adds new rule conditions to commerce_line_item entity type.
 */
function commerce_product_rules_condition_info_alter(&$conditions) {
  $inline_conditions = inline_conditions_get_info();

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
    'module' => 'commerce_line_item',
    'group' => t('Commerce Line Item'),
    'callbacks' => array(
      'execute' => $inline_conditions['commerce_product_contains_products']['callbacks']['build'],
    ),
  );

  if (module_exists('taxonomy')) {
    $conditions['commerce_product_has_specified_terms'] = array(
      'label' => t('Line item contains specific terms ID'),
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
      'module' => 'commerce_line_item',
      'group' => t('Commerce Line Item'),
      'callbacks' => array(
        'execute' => $inline_conditions['commerce_product_has_specified_terms']['callbacks']['build'],
      ),
    );
  }
}
