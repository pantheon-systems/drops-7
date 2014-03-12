<?php

/*
 * Implements hook_views_data_alter().
 */
function commerce_discount_usage_views_data_alter(&$data) {
  if (isset($data['commerce_discount'])) {

    // Usage analytics field
    $data['commerce_discount']['commerce_discount_usage'] = array(
      'title' => t('Analytics'),
      'help' => t('Show discount usage and usage limit.'),
      'field' => array(
        'handler' => 'commerce_discount_usage_handler_field_commerce_discount_analytics',
      ),
      'real field' => 'discount_id',
    );

    // Usage relationship
    $data['commerce_discount']['discount_usage'] = array(
      'relationship' => array(
        'title' => t('Discount usage'),
        'label' => t('Discount usage'),
        'help' => t('Relate this discount to its usage statistics.'),
        'handler' => 'views_handler_relationship',
        'base' => 'commerce_discount_usage',
        'base field' => 'discount',
        'field' => 'name',
      ),
    );
  }

  // Order table: discount usage relationship
  if (isset($data['commerce_order'])) {
    $data['commerce_order']['discount_usage'] = array(
      'relationship' => array(
        'title' => t('Discount usage'),
        'label' => t('Discount usage'),
        'help' => t('Relate this order to its discount usage statistics.'),
        'handler' => 'views_handler_relationship',
        'base' => 'commerce_discount_usage',
        'base field' => 'order_id',
        'field' => 'order_id',
      ),
    );
  }

  // User table: discount usage relationship
  $data['users']['discount_usage'] = array(
    'relationship' => array(
      'title' => t('Discount usage'),
      'label' => t('Discount usage'),
      'help' => t('Relate this user to its discount usage statistics.'),
      'handler' => 'views_handler_relationship',
      'base' => 'commerce_discount_usage',
      'base field' => 'mail',
      'field' => 'mail',
    ),
  );
}

/*
 * Implements hook_views_data().
 */
function commerce_discount_usage_views_data() {
  $data = array();
  $data['commerce_discount_usage']['table']['group'] = t('Commerce discount usage');

  // Base table
  $data['commerce_discount_usage']['table']['base'] = array(
    'field' => 'mail',
    'title' => t('Commerce Discount Usage'),
    'help' => t('Usage statistics about discounts.'),
  );

  // Discount name.
  $data['commerce_discount_usage']['discount'] = array(
    'title' => t('Discount'),
    'help' => t('The unique human-readable identifier of the discount.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'relationship' => array(
      'title' => t('Discount'),
      'label' => t('Discount'),
      'help' => t('Relate a usage statistics record to its discount.'),
      'handler' => 'views_handler_relationship',
      'base' => 'commerce_discount',
      'base field' => 'name',
      'field' => 'discount',
    )
  );

  // Customer mail.
  $data['commerce_discount_usage']['mail'] = array(
    'title' => t('User'),
    'help' => t('The email address of the customer who used the discount.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'relationship' => array(
      'title' => t('Customer email'),
      'label' => t('Customer email'),
      'help' => t('Relate a usage statistics record to its user.'),
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'base field' => 'mail',
      'field' => 'mail',
    )
  );

  // Order ID.
  $data['commerce_discount_usage']['order_id'] = array(
    'title' => t('Order ID'),
    'help' => t('The unique internal identifier of the order where the discount was used.'),
    'field' => array(
      'handler' => 'commerce_order_handler_field_order',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'commerce_order_handler_argument_order_order_id',
    ),
    'relationship' => array(
      'title' => t('Order'),
      'label' => t('Order'),
      'help' => t('Relate a usage statistics record to its order.'),
      'handler' => 'views_handler_relationship',
      'base' => 'commerce_order',
      'base field' => 'order_id',
      'field' => 'order_id',
    )
  );

  return $data;
}
