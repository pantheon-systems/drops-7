<?php

/**
 * @file
 * Contains forms for the user_external_invite module.
 */

/**
 * Configuration form for the module.
 */
function user_external_invite_settings_form() {
  $form = array();
  $form['#validate'][] = 'user_external_invite_settings_form_validate';

  // Define roles that users can have.
  $user_roles = user_roles();
  $roles = array();
  $excluded_roles = module_invoke_all('user_external_invite_excluded_roles', $roles);
  foreach ($excluded_roles as $role_id) {
    if (isset($user_roles[$role_id])) {
      unset($user_roles[$role_id]);
    }
  }
  $form['user_external_invite_roles'] = array(
    '#type' => 'select',
    '#title' => t('Roles users can be invited to join'),
    '#description' => t('Users with permission to send invites will be able to invite users to join a site with any of these roles.  GRANT WITH CAUTION!'),
    '#options' => $user_roles,
    '#default_value' => variable_get('user_external_invite_roles', NULL),
    '#multiple' => TRUE,
  );

  $form['user_external_invite_default_role'] = array(
    '#title' => t('Default Role to Invite'),
    '#description' => t('Choose the default role you wish to have selected on the invite page.'),
    '#type' => 'radios',
    '#options' => $user_roles,
    '#default_value' => variable_get('user_external_invite_default_role', key($user_roles)),
  );

  // Days invite valid for.
  $form['user_external_invite_days_valid_for'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of days invites are valid'),
    '#description' => t("Invites are set to expire so many days after they are created. If a user hasn't accepted the invite by that time, then you will have to send a new invite to grant that user a role."),
    '#default_value' => variable_get('user_external_invite_days_valid_for', 5),
    '#element_validate' => array('element_validate_number'),
    '#maxlength' => 3,
  );

  // Delete old invites after a certain time.
  $form['user_external_invite_delete_old_invites'] = array(
    '#type' => 'textfield',
    '#title' => t('Invite Deletion'),
    '#description' => t("Invites are deleted during a cron run after they have passed their expire time. Defaults to 30 days (2592000 seconds)."),
    '#default_value' => variable_get('user_external_invite_delete_old_invites', 60*60*24*30),
    '#size' => 60,
    '#element_validate' => array('element_validate_number'),
    '#required' => TRUE,
  );

  // From email address.
  $form['user_external_invite_use_universal_from_email'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send all invites from a single email address'),
    '#description' => t('If this is not configured, invites will be sent using the email address of the user sending the invite.'),
    '#default_value' => variable_get('user_external_invite_use_universal_from_email', FALSE),
  );

  $form['user_external_invite_universal_from_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email address invites are sent from'),
    '#default_value' => variable_get('user_external_invite_universal_from_email', NULL),
    '#maxlength' => 256,
    '#states' => array(
      'visible' => array(
        ':input[name="user_external_invite_use_universal_from_email"]' => array('checked' => TRUE),
      ),
      'required' => array(
        ':input[name="user_external_invite_use_universal_from_email"]' => array('checked' => TRUE),
      ),
    ),
  );

  // Message templates.
  $form['user_external_invite_invite_template'] = array(
    '#title' => t('Invitation Email Template'),
    '#type' => 'textarea',
    '#cols' => 40,
    '#rows' => 5,
    '#default_value' => variable_get('user_external_invite_invite_template'),
    '#description' => t('Message sent to user being invited.'),
  );

  $form['user_external_invite_confirmation_template'] = array(
    '#title' => t('Invitation Confirmation'),
    '#type' => 'textarea',
    '#cols' => 40,
    '#rows' => 5,
    '#default_value' => variable_get('user_external_invite_confirmation_template'),
    '#description' => t('Confirmation message sent to user who initiated the invivation confirming the invitation was sent.'),
  );

  $form['user_external_invite_accepted_template'] = array(
    '#title' => t('Invitation Accepted Email Template'),
    '#type' => 'textarea',
    '#cols' => 40,
    '#rows' => 5,
    '#default_value' => variable_get('user_external_invite_accepted_template'),
    '#description' => t('Message sent to user who initiated the invivation when the user accepts an invite.'),
  );

  $form['user_external_invite_accepted_confirmation_template'] = array(
    '#title' => t('Invitation Accepted Confirmation Email Template'),
    '#type' => 'textarea',
    '#cols' => 40,
    '#rows' => 5,
    '#default_value' => variable_get('user_external_invite_accepted_confirmation_template'),
    '#description' => t('Message sent to user being invited confirming the process was completed.'),
  );

  // @TODO: add warning email about expiring invititations.
  $form['token_help']['content'] = array(
    '#type' => 'markup',
    '#token_types' => 'all',
    '#theme' => 'token_tree',
  );

  return system_settings_form($form);
}

/**
 * Form validation handler for user external invite settings form.
 */
function user_external_invite_settings_form_validate($form, &$form_state) {
  // Check if Default Role to Invite is set and is one of the roles that can be invited.
  if (isset($form_state['values']['user_external_invite_default_role'])) {
    if (!in_array($form_state['values']['user_external_invite_default_role'], $form_state['values']['user_external_invite_roles'])) {
      form_set_error('user_external_invite_default_role', t('Default role can only be one of the roles selected to invite.'));
    }
  }

  // Only validate the email if it is being used.
  if ($form_state['values']['user_external_invite_use_universal_from_email']) {
    if (valid_email_address($form_state['values']['user_external_invite_universal_from_email'])) {
      return;
    }
    form_set_error('user_external_invite_universal_from_email', t('Invalid email.'));
  }
}


/**
 * Form to invite a new user as a site owner or content editor.
 */
function user_external_invite_form($form, &$form_state) {

  drupal_set_title(t('Invite New User'));

  $role_options = array();
  $rids = variable_get('user_external_invite_roles', NULL);

  if (empty($rids)) {
    drupal_set_message(t('Your site is not yet configured to invite users. Contact the site administrator to configure the invite feature.'), 'status');
  }
  else {
    foreach ($rids as $rid) {
      $role_options[$rid] = _user_external_invite_role_name_from_rid($rid);
    }
    asort($role_options);

    $form['rid'] = array(
      '#title' => t('Role'),
      '#type' => 'radios',
      '#options' => $role_options,
      '#default_value' => variable_get('user_external_invite_default_role', key($role_options)),
      '#required' => TRUE,
    );
    $form['email'] = array(
      '#title' => t('Email addresses'),
      '#type' => 'textarea',
      '#description' => t('Comma separated list of email addresses that are sent the invitation email.'),
      '#required' => TRUE,
    );

    $form['custom_message'] = array(
      '#title' => t('Custom message'),
      '#type' => 'textarea',
      '#cols' => 40,
      '#rows' => 5,
      '#description' => t('If added, the custom message will be included before the standard template.  Can be left blank.'),
    );

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Send Invites'),
    );
    return $form;  
  }

  
}

/**
 * Validation handler for invite_form.
 *
 * Ensures email addresses are in valid format.
 */
function user_external_invite_form_validate(&$form, &$form_state) {
  $email_value = $form_state['values']['email'];
  // Split on , or line break.
  $emails = preg_split("/[\r\n,\s]+/", $email_value, -1, PREG_SPLIT_NO_EMPTY);
  $error = FALSE;
  foreach ($emails as $mail) {
    if (!valid_email_address($mail)) {
      $error = TRUE;
      form_set_error('email', t('Incorrect email address (%mail). No invites were sent.', array('%mail' => $mail)));
    }
    else {
      $load = _user_external_invite_load_invite($mail);
      if (isset($load['mail'])) {
        form_set_error('email', t('Email address (%mail) has already been invited. No invites were sent.', array('%mail' => $mail)));
      }
    }
  }
  if (!$error) {
    $form_state['values']['parsed_mails'] = $emails;
  }
}

/**
 * Submit handler for invite_form.
 *
 * Add entry to database and send email.
 */
function user_external_invite_form_submit(&$form, &$form_state) {
  global $user;
  _user_external_invite_add_invite($form_state['values']['rid'], $form_state['values']['parsed_mails'], $user->uid, $form_state['values']['custom_message']);
  drupal_set_message(t('Successfully invited new user!'));
}
