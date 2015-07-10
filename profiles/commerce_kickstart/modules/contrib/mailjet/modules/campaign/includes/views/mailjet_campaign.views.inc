<?php
/**
 * @file
 * commerce_funds.views.inc
 */

/**
 * Impliments hook_views_data().
 */
function mailjet_campaign_views_data() {
  $data = array();

  $data['mailjet_campaign']['table']['base'] = array(
    'field' => 'id',
    'help' => t('An id referenced by another entity.'),
    'access query tag' => 'mailjet_campaign_access',
  );

  $data['mailjet_campaign']['table']['join'] = array(
    'commerce_order' => array(
      'left_field' => 'order_id',
      'field' => 'order_id',
    ),
  );

  // Campaigns table.
  $data['mailjet_campaign']['campaign_id'] = array(
    'title' => t('Campaign id'),
    'help' => t('The campaign id'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  $data['mailjet_campaign']['order_id'] = array(
    'title' => t('Order id in campaigns'),
    'help' => t('The id of the order to which the campaign belongs.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  'relationship' => array(
      'base' => 'commerce_order', // The name of the table to join with.
      'base field' => 'order_id', // The name of the field on the joined table.
      // 'field' => 'nid' -- see hook_views_data_alter(); not needed here.
      'handler' => 'views_handler_relationship',
      'label' => t('campaigns realationshop with orders'),
      'title' => t('Relation ship with orders'),
      'help' => t('You can join commerce_order table with campaigns.'),
    ),
  );

  $data['mailjet_campaign']['name'] = array(
    'title' => t('Campaign name'),
    'help' => t('The name of the campaign.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  $data['mailjet_campaign']['created'] = array(
    'title' => t('Campaign created date'),
    'help' => t('The created date of the campaign.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
       'handler' => 'views_handler_filter_date',
     ),
  );

  return $data;
}