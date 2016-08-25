<?php

/**
 * @file
 * Views integration for the user_external_invite module.
 */


/**
 * Implements hook_views_data().
 *
 * @return array
 */
function user_external_invite_views_data() {

  $data['user_external_invite']['table']['group'] = t('User External Invite');

  $data['user_external_invite']['table']['base'] = array(
    'field' => 'id', // This is the identifier field for the view.
    'title' => t('User External Invite'),
    'help' => t('Table to handle user invites.'),
  );

  // Defining id field.
  $data['user_external_invite']['id'] = array(
    'title' => t('Invite ID'),
    'help' => t('Primary key for user invites.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Defining mail field.
  $data['user_external_invite']['mail'] = array(
    'title' => t('Email Address'),
    'help' => t('Email address where invite is sent.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );


  // Defining rid field.
  $data['user_external_invite']['rid'] = array(
    'title' => t('Role ID'),
    'help' => t('Role ID assigned to user invite.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Defining expire field.
  $data['user_external_invite']['expire'] = array(
    'title' => t('Expiration of Invite'),
    'help' => t('Timestamp of when an invite is set to expire if not activated.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  // Defining status field.
  $data['user_external_invite']['status'] = array(
    'title' => t('Invite Status'),
    'help' => t('Current status of user invite.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Defining uid field.
  $data['user_external_invite']['uid'] = array(
    'title' => t('Invite UID'),
    'help' => t('UID the invite will be associated to when granted.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Defining message field.
  $data['user_external_invite']['message'] = array(
    'title' => t('Invite Message'),
    'help' => t('Custom message sent to user being invited.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  dpm($data);
  return $data;
}
