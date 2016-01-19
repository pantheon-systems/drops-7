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
        'label' => t('Commerce Discount'),
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

  // Note that we add a day to the discount end date. The date field value is
  // set for 12:00:00 AM on the end date, but we want the discount to remain
  // valid through that day.
  return REQUEST_TIME >= $wrapper->commerce_discount_date->value->value() && REQUEST_TIME < $wrapper->commerce_discount_date->value2->value() + 86400;
}
