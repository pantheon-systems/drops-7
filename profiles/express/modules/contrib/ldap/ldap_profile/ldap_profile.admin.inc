<?php

/**
 * @file
 * Administrative page callbacks for the ldap_profile module.
 */


/**
 * form for adding, updating, and deleting a single ldap profile mapping
 *
 * @param <type> $form
 * @param <type> $form_state
 * @return array drupal form array
 */
function ldap_profile_admin_form($form, &$form_state) {
  require_once('LdapProfileConfAdmin.class.php');
  $auth_conf = new LdapProfileConfAdmin();
  return $auth_conf->drupalForm();
}

/**
 * validate handler for the ldap_profile_admin_form
 */
function ldap_profile_admin_form_validate($form, &$form_state) {
  require_once('LdapProfileConfAdmin.class.php');
  $auth_conf = new LdapProfileConfAdmin();
  $errors = $auth_conf->drupalFormValidate($form_state['values']);
  foreach ($errors as $error_name => $error_text) {
    form_set_error($error_name, t(c($error_text)));
  }
}


/**
 * submit handler function for ldap_profile_admin_form
 */
function ldap_profile_admin_form_submit($form, &$form_state) {
  require_once('LdapProfileConfAdmin.class.php');
  $auth_conf = new LdapProfileConfAdmin();
  $auth_conf->drupalFormSubmit($form_state['values']);  // add form data to object and save or create

  if ($auth_conf->hasError == FALSE) {
    drupal_set_message(t('LDAP Profile configuration saved'), 'status');
    drupal_goto(LDAP_SERVERS_MENU_BASE_PATH . '/profile');
  }
  else {
    form_set_error($auth_conf->errorName, $auth_conf->errorMsg);
    $auth_conf->clearError();
  }

}
