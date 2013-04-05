<?php

/**
 * @file
 * Provides support for the Views module.
 */

/**
 * Implements hook_views_data().
 */
function commerce_discount_usage_views_data() {
  $data['commerce_discount']['commerce_discount_usage'] = array(
    'title' => t('Analytics'),
    'help' => t('Show discount usage and usage limit.'),
    'field' => array(
      'handler' => 'commerce_discount_usage_handler_field_commerce_discount_usage',
    ),
    // This is a dummy field, so attach it to the ID column.
    'real field' => 'discount_id',
  );

  return $data;
}
