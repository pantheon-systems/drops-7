<?php

/**
 * @file
 * Rules integration for Commerce Addressbook.
 *
 * @addtogroup rules
 * @{
 */

/**
 * Implements hook_rules_event_info().
 */
function commerce_addressbook_rules_event_info() {
  $events = array();

  $events['commerce_addressbook_set_default'] = array(
    'label' => t('Customer profile default changed'),
    'group' => t('Commerce Addressbook'),
    'variables' => array(
      'commerce_customer_profile' => array(
        'type' => 'commerce_customer_profile',
        'label' => t('Customer profile'),
      ),
    ),
  );

  return $events;
}

/**
 * Implement hook_rules_condition_info()
 */
function commerce_addressbook_rules_condition_info() {
  return array(
    'commerce_addressbook_profile_is_default' => array(
      'label' => t('Customer profile is the default'),
      'parameter' => array(
        'customer_profile' => array(
          'label' => t('Customer Profile'),
          'type' => 'commerce_customer_profile'
        ),
      ),
      'group' => t('Commerce Addressbook'),
      'callbacks' => array(
        'execute' => 'commerce_addressbook_rules_profile_is_default',
      ),
    ),
  );
}

/**
 * Rules condition: checks if profile is user's default customer profile.
 *
 * @param $customer_profile
 *    The customer profile to check.
 *
 * @return bool
 *    TRUE or FALSE if profile IDs match.
 */
function commerce_addressbook_rules_profile_is_default($customer_profile) {
  return commerce_addressbook_get_default_profile_id($customer_profile->uid,$customer_profile->type) == $customer_profile->profile_id;
}

/**
 * @}
 */
