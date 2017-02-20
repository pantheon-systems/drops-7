<?php
/**
 * @file
 * Rules integration for commerce physical.
 */

/**
 * Implements hook_rules_condition_info().
 */
function commerce_physical_rules_condition_info() {

  $conditions = array();

  $conditions['commerce_physical_rules_order_weight_comparison'] = array(
    'label' => t("Order total weight comparison"),
    'parameter' => array(
      'commerce_order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
      ),
      'operator' => array(
        'type' => 'text',
        'label' => t('Operator'),
        'description' => t('The comparison operator.'),
        'options list' => 'commerce_numeric_comparison_operator_options_list',
        'restriction' => 'input',
      ),
      'value' => array(
        'type' => 'decimal',
        'label' => t('Value'),
      ),
      'unit' => array(
        'type' => 'text',
        'label' => t('Weight unit'),
        'options list' => 'physical_weight_unit_options',
        'restriction' => 'input',
        'default value' => 'kg',
      ),
    ),
    'group' => t('Commerce Physical'),
  );

  $conditions['commerce_physical_rules_order_max_dimension_comparison'] = array(
    'label' => t("Maximum product dimension comparison"),
    'parameter' => array(
      'commerce_order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
      ),
      'operator' => array(
        'type' => 'text',
        'label' => t('Operator'),
        'description' => t('The comparison operator.'),
        'options list' => 'commerce_numeric_comparison_operator_options_list',
        'restriction' => 'input',
      ),
      'value' => array(
        'type' => 'decimal',
        'label' => t('Value'),
      ),
      'unit' => array(
        'type' => 'text',
        'label' => t('Dimension unit'),
        'options list' => 'physical_dimension_unit_options',
        'restriction' => 'input',
        'default value' => 'cm',
      ),
    ),
    'group' => t('Commerce Physical'),
  );

  $conditions['commerce_physical_rules_order_is_shippable'] = array(
    'label' => t("The order contains shippable products"),
    'parameter' => array(
      'commerce_order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
      ),
    ),
    'group' => t('Commerce Physical'),
  );

  $conditions['commerce_physical_rules_line_item_is_shippable'] = array(
    'label' => t("The line item is a shippable product"),
    'parameter' => array(
      'commerce_line_item' => array(
        'type' => 'commerce_line_item',
        'label' => t('Line Item'),
      ),
    ),
    'group' => t('Commerce Physical'),
  );

  return $conditions;
}


/**
 * Fetches the max. dimension in the order and compares it with a given value.
 *
 * Calculates the maximum single dimension of any product in the order and
 * performs a comparison on it.
 */
function commerce_physical_rules_order_max_dimension_comparison($order, $operator, $value, $unit) {

  $max_dimension = 0;

  $dimension_keys = array(
    'length',
    'width',
    'height',
  );

  // Get the dimensions of every product in the order.
  foreach (commerce_physical_order_dimensions($order, $unit) as $dimension) {

    // Check each of length / width / height.
    foreach ($dimension_keys as $dimension_key) {

      // If this dimension's bigger than the current max, it's the new max.
      if ($dimension[$dimension_key] > $max_dimension) {
        $max_dimension = $dimension[$dimension_key];
      }
    }
  }

  switch ($operator) {
    case '<':
      return $max_dimension < $value;

    case '<=':
      return $max_dimension <= $value;

    case '==':
      return $max_dimension == $value;

    case '>=':
      return $max_dimension >= $value;

    case '>':
      return $max_dimension > $value;
  }

  return FALSE;
}

/**
 * Calculates the order's total weight and performs a comparison on it.
 */
function commerce_physical_rules_order_weight_comparison($order, $operator, $value, $unit) {

  $order_weight = commerce_physical_order_weight($order, $unit);

  switch ($operator) {
    case '<':
      return $order_weight['weight'] < $value;

    case '<=':
      return $order_weight['weight'] <= $value;

    case '==':
      return $order_weight['weight'] == $value;

    case '>=':
      return $order_weight['weight'] >= $value;

    case '>':
      return $order_weight['weight'] > $value;
  }

  return FALSE;
}

/**
 * Rules condition: check if the order contains shippable products.
 */
function commerce_physical_rules_order_is_shippable($order) {
  return commerce_physical_order_shippable($order);
}

/**
 * Rules condition: check if the line item is a shippable product.
 */
function commerce_physical_rules_line_item_is_shippable($line_item) {
  return commerce_physical_line_item_shippable($line_item);
}