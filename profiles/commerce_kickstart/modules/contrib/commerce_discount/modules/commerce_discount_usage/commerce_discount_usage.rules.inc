<?php

/**
 * @file
 * Rules integration for the Commerce Discount Usage module.
 */

/**
 * Implements hook_rules_condition_info().
 */
function commerce_discount_usage_rules_condition_info() {
  // Max usage per person.
  $conditions['commerce_discount_usage_max_usage_per_person'] = array(
    'label' => t('Max usage per person'),
    'parameter' => array(
      'order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
      ),
      'commerce_discount' => array(
        'label' => t('Commerce Discount'),
        'type' => 'token',
        'options list' => 'commerce_discount_entity_list',
      ),
      'usage' => array(
        'type' => 'integer',
        'label' => t('Maximum usage per customer'),
        'description' => t('Enter the maximum number of times a specific person (as identified by email) may use this discount. Leave blank for unlimited.'),
      ),
    ),
    'group' => t('Commerce Discount'),
  );

  // Max usage.
  $conditions['commerce_discount_usage_max_usage'] = array(
    'label' => t('Max usage'),
    'parameter' => array(
      'order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
      ),
      'commerce_discount' => array(
        'label' => t('Commerce Discount'),
        'type' => 'token',
        'options list' => 'commerce_discount_entity_list',
      ),
      'usage' => array(
        'type' => 'integer',
        'label' => t('Maximum overall usage'),
        'description' => t('Enter the maximum number of times this discount may be used on the site, by anyone. Leave blank for unlimited.'),
      ),
    ),
    'group' => t('Commerce Discount'),
  );

  return $conditions;
}

/**
 * Rules condition callback: evaluate maximum usage per-person of a discount.
 */
function commerce_discount_usage_max_usage_per_person($order, $discount_name) {
  $discount_wrapper = entity_metadata_wrapper('commerce_discount', $discount_name);
  $per_person_limit = $discount_wrapper->discount_usage_per_person->value();

  // Nothing to count if the order does not have an email.
  if (!$per_person_limit || !$order->mail) {
    return TRUE;
  }

  // Find other orders owned by same person that have same discount.
  $usage = commerce_discount_usage_get_usage_by_mail($discount_name, $order->mail, $order->order_id);

  return $usage < $per_person_limit;
}

/**
 * Rules condition callback: evaluate maximum usage of a discount.
 */
function commerce_discount_usage_max_usage($order, $discount_name) {
  $discount_wrapper = entity_metadata_wrapper('commerce_discount', $discount_name);
  $limit = $discount_wrapper->discount_usage_limit->value();

  if (!$limit) {
    return TRUE;
  }

  // Find other orders that have same discount.
  $usage = commerce_discount_usage_get_usage($discount_name, $order->order_id);

  return $usage < $limit;
}
