<?php
/**
 * @file
 * User-facing page callbacks for the LDAP Authentication module.
 */

/**
 * Form constructor for updating the profile.
 * 
 * @see ldap_authentication_profile_update_form_validate().
 * @see ldap_authentication_profile_update_form_submit().
 */
function ldap_authentication_profile_update_form($form, &$form_state) {
  $form['mail'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Email Address'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update Profile'),
  );
  return $form;
}

/**
 * Form validator for updating the profile.
 * 
 * @see ldap_authentication_profile_update_form().
 */
function ldap_authentication_profile_update_form_validate($form, &$form_state) {
  if (!filter_var($form_state['values']['mail'], FILTER_VALIDATE_EMAIL)) {
    form_set_error('mail', t('You must specify a valid email address.'));
  }
  $existing = user_load_by_mail($form_state['values']['mail']);
  if ($existing) {
    form_set_error('mail', t('This email address is already in user.'));
  }
  $auth = ldap_authentication_get_valid_conf();
  $regex = '`' . $auth->templateUsagePromptRegex . '`i';
  if (preg_match($regex, $form_state['values']['mail'])) {
    form_set_error('mail', t('This email address still matches the invalid email template.'));
  }
}

/**
 * Form submit handler for updating the profile.
 * 
 * @see ldap_authentication_profile_update_form().
 */
function ldap_authentication_profile_update_form_submit($form, &$form_state) {
  global $user;
  if (user_save($user, array(
    'mail' => $form_state['values']['mail'],
  ))) {
    // prevents the cached setting from being used again.
    unset($_SESSION['ldap_authentication_template']);
    $form_state['redirect'] = isset($_GET['next']) ? $_GET['next'] : '<front>';
    drupal_set_message(t('Your profile has been updated.'));
  }
}
