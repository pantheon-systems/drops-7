<?php

/**
 * @file
 * This file provides a way to invite users to a site via drush.
 */

/**
 * Implements hook_drush_command().
 */
function user_external_invite_drush_command() {
  $items = array();
  $items['user-external-invite-send'] = array(
    'description' => 'Invite a user with an email address',
    'arguments' => array(
      'email' => 'Email address to invite',
      'role' => 'Role to grant',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT,
    'options' => array(),
  );
  return $items;
}

/**
 * Drush user-external-invite-send [mail] [role].
 */
function drush_user_external_invite_send_invite() {
  $args = drush_get_arguments();

  // Flipped for easy searching.
  $roles = array_flip(_user_external_invite_invite_roles());

  $mail = $args[1];
  $role = $args[2];

  $error = FALSE;
  if (!valid_email_address($mail)) {
    drush_set_error(dt('Please provide a valid email address.'));
    $error = TRUE;
  }
  if (!isset($roles[$role])) {
    drush_set_error(dt('Please provide a valid role.'));
    $error = TRUE;
  }

  if (!$error) {
    $pending = _user_external_invite_pending_invites($mail);
    if ($pending) {
      drush_set_error(dt('That email address already has a pending invite.'));
    }
    else {
      // Send empty array for addon roles.
      _user_external_invite_add_invite($roles[$role], array(), array($mail), 0);
      drush_log(dt('Invite email sent!'), 'success');
    }
  }
}
