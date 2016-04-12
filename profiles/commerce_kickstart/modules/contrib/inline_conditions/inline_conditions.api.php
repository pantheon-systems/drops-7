<?php

/**
 * @file
 * Hooks provided by the Inline Conditions module.
 */

/**
 * Defines inline conditions.
 *
 * Inline Conditions are configured and added through a field attached
 * to the parent entity (discount, shipping rate, etc) based on which the
 * rule is built. When the rule is being built, the module providing the
 * parent entity type calls inline_conditions_build_rule,
 * which in turn calls the build callback of each configured inline condition.
 * Based on the user provided configuration, the build callback adds an
 * actual rules condition to the passed rule.
 *
 * Supported keys:
 * - label: The human readable label, shown in the UI (field widget).
 * - entity type: The type of the entity available to the rule, on which the
 *   condition can operate. This is the main criteria on which conditions are
 *   selected for showing in the field widget.
 * - rule condition name: (Optional) Rule condition machine name. This rule
 *   condition will be use instead of the inline condition name. If this
 *   key is set, the build callback is optional.
 * - parent entity type: (Optional) The type of the parent entity type (that
 *   contains the Inline Conditions field), used to further limit the
 *   availability of the condition (so a condition could choose to be shown
 *   only for Order Discounts, but not Shipping Services, even though both
 *   operate on the same entity type -> commerce_order).
 * - callbacks: An array of callbacks:
 *   - configure: (Optional) Returns a configuration form embedded into the
 *     field widget, and used to configure the inline condition. The following
 *     list of parameters will be passed to the configure callback function:
 *     - condition_settings (Array): an array containing all configured
 *       settings; typically this will match the values of the form elements
 *       defined in the 'configure' callback. It should be transformed into
 *       an array of parameter values as the rules condition needs them.
 *     - instance (Array): The field instance array (which includes the entity
 *       information that is related to the condition, such as the ID).
 *     - delta (Int): The current field delta defined as an integer.
 *   - build: [Do not use if rule condition name is set] Gets the rule and any
 *     settings added by the configure callback, then builds and adds an actual
 *     rules condition to the rule. Also, if the rule condition name key is set,
 *     this parameter is no longer available.
 */
function hook_inline_conditions_info() {
  $conditions = array();
  $conditions['inline_conditions_order_total'] = array(
    'label' => t('Orders over'),
    'entity type' => 'commerce_order',
    //'rule condition name' => 'data_is'
    'callbacks' => array(
      'configure' => 'inline_conditions_order_total_configure',
      'build' => 'inline_conditions_order_total_build',
    ),
  );

  return $conditions;
}

/**
 * Alter the condition info.
 *
 * @param array $conditions
 *   An array of inline conditions.
 *
 * @see hook_inline_conditions_info().
 */
function hook_inline_conditions_info_alter(&$conditions) {
  $conditions['inline_conditions_order_total']['label'] = t('Order total over');
}

/**
 * Alter a field value, just before building a rules condition from it.
 * Specifically: if the structure of the 'condition_settings' array as saved in
 * the field does not exactly match the parameters expected by the corresponding
 * rules condition, this hook allows transforming the former into the latter.
 *
 * @param array $value
 *   A single field value from an "inline_conditions" type field. This means
 *   it typically contains two keys:
 *   - 'condition_name': the condition name as defined in
 *     hook_inline_conditions_info() and hook_rules_condition_info().
 *   - 'condition_settings': an array containing all configured settings;
 *     typically this will match the values of the form elements defined in the
 *     'configure' callback. It should be transformed into an array of parameter
 *     values as the rules condition needs them. (Not including the line item.)
 *     Your 'build' callback will be passed the same parameter values.
 *
 * @see inline_conditions_build().
 */
function hook_inline_conditions_build_alter(&$value) {
  if ($value['condition_name'] == 'commerce_order_contains_products') {
    // 'products' is a text field in the configure form: comma-separated SKUs.
    // It has a #validate function that turns submitted input into form value:
    //     array( array('product_id' => P1), array('product_id' => P2), ...)
    // so that's how it gets stored into 'condition_settings'.

    // Load the products...
    $entity_ids = array();
    foreach ($value['condition_settings']['products'] as $delta) {
      $entity_ids[] = reset($delta);
    }
    $products = commerce_product_load_multiple($entity_ids);

    // ...so we can turn it back into a comma-separated string of SKUs.
    $value['condition_settings']['products'] = '';
    foreach ($products as $product) {
      $value['condition_settings']['products'] .= $product->sku;
      if ($product !== end($products)) {
        $value['condition_settings']['products'] .= ', ';
      }
    }
  }
}
