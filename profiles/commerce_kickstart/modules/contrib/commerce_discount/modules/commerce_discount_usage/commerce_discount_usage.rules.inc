<?php

/**
 * @file
 * Rules integration for the Commerce discount usage module.
 */

/**
 * Implements hook_rules_condition_info().
 */
function commerce_discount_usage_rules_condition_info() {
  $items = array();

  $items['commerce_discount_usage_condition'] = array(
    'label' => t('Check discount max usage'),
    'group' => t('Commerce Discount'),
    'parameter' => array(
      'commerce_discount' => array(
        'label' => t('Commerce discount'),
        'type' => 'token',
        'options list' => 'commerce_discount_entity_list',
      ),
    ),
    'base' => 'commerce_discount_usage_condition',
  );

  return $items;
}

/**
 * Rules condition: Check discount can be applied.
 */
function commerce_discount_usage_condition($discount_name) {
  $wrapper = entity_metadata_wrapper('commerce_discount', $discount_name);
  $uses = $wrapper->commerce_discount_uses->value() ? $wrapper->commerce_discount_uses->value() : 0;
  return $uses < $wrapper->commerce_discount_max_uses->value();
}

