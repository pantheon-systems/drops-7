<?php

/**
 * @file
 * Rules integration for the Commerce discount date module.
 */

/**
 * Implements hook_rules_condition_info().
 */
function commerce_discount_date_rules_condition_info() {
  $items = array();

  $items['commerce_discount_date_condition'] = array(
    'label' => t('Check discount dates'),
    'group' => t('Commerce Discount'),
    'parameter' => array(
      'commerce_discount' => array(
        'label' => t('Commerce discount'),
        'type' => 'token',
        'options list' => 'commerce_discount_entity_list',
      ),
    ),
    'base' => 'commerce_discount_date_condition',
  );

  return $items;
}

/**
 * Rules condition: Check discount can be applied.
 */
function commerce_discount_date_condition($discount_name) {
  $wrapper = entity_metadata_wrapper('commerce_discount', $discount_name);
  $time = time();
  return $time >= $wrapper->commerce_discount_date->value->value() && $time <= $wrapper->commerce_discount_date->value2->value();
}

