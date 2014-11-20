<?php

/**
 * Implements hook_views_data().
 */
function commerce_addressbook_views_data() {
  $data = array();
  $data['commerce_addressbook_defaults']['table']['group'] = t('Addressbook');
  $data['commerce_addressbook_defaults']['table']['join'] = array(
    'commerce_customer_profile' => array(
      'left_field' => 'profile_id',
      'field' => 'profile_id',
    ),
    'users' => array(
      'left_field' => 'uid',
      'field'      => 'uid',
    ),
  );
  $data['commerce_addressbook_defaults']['profile_id'] = array(
    'title' => t('Profile ID'),
    'help' => t('The customer profile id'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'commerce_customer_profile',
      'base field' => 'profile_id',
      'label' => t('Profile'),
    ),
  );
  return $data;
}
