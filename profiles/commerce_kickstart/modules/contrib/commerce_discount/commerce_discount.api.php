<?php

/**
 * @file
 * Hooks provided by the Commerce Discount module.
 */

/**
 * Defines the types of discounts available for creation on the site.
 *
 * Each defined type is a bundle of the commerce_discount entity type, hence
 * able to have its own fields attached.
 *
 * The discount type array structure includes the following keys:
 * - label: a translatable, human-readable discount type label.
 * - event: the machine name of the rules event used to apply a discount of
 *   the defined type.
 * - entity type: The type of entity to which the discount will be applied.
 *
 * @return
 *   An array of discount type arrays keyed by the machine name of the type.
 */
function hook_commerce_discount_type_info() {
  $types = array();
  $types['order_discount'] = array(
    'label' => t('Order Discount'),
    'event' => 'commerce_order_presave',
    'entity type' => 'commerce_order',
  );
  $types['product_discount'] = array(
    'label' => t('Product Discount'),
    'event' => 'commerce_product_calculate_sell_price',
    'entity type' => 'commerce_line_item',
  );

  return $types;
}

/**
 * Defines the types of discount offers available for creation on the site.
 *
 * Each defined type is a bundle of the commerce_discount_offer entity type,
 * hence able to have its own fields attached.
 *
 * The discount offer type array structure includes the following keys:
 * - label: a translatable, human-readable discount offer type label.
 * - action: the Rules function callback used to apply a discount offer
 *   of the defined type to an entity.
 * - entity types: The entity types that this offer handles. Only offers
 *   that support the "entity type" of the selected discount type are shown in
 *   the UI.
 *
 * @return
 *   An array of discount offer type arrays keyed by the machine name of the
 *   type.
 */
function hook_commerce_discount_offer_type_info() {
  $types = array();
  $types['random_amount'] = array(
    'label' => t('Random $ off'),
    'action' => 'foo_random_amount',
    'entity types' => array('commerce_order', 'commerce_line_item'),
  );

  return $types;
}

/**
 * Allow modules alter the rule object, with configuration specifc
 * to commerce discount.
 *
 * @param $rule
 *   The rule configuration entity, passed by reference.
 * @param $commerce_discount
 *   The commerce discount entity.
 */
function hook_commerce_discount_rule_build($rule, $commerce_discount) {
  if ($commerce_discount->name == 'foo') {
    $rule->action('drupal_message', array('message' => 'Discount FOO was applied.'));
  }
}
