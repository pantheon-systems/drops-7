<?php

/**
 * @file
 * LDAP User editing UI.
 *
 */


/**
 * form for adding, updating, and deleting a single ldap authorization mapping
 */
function ldap_user_admin_form($form, &$form_state) {
  $ldap_user_conf = ldap_user_conf('admin');
  $form = $ldap_user_conf->drupalForm();
  return $form;
}


/**
 * validate handler for the ldap_user_admin_form
 */
function ldap_user_admin_form_validate($form, &$form_state) {

  $ldap_user_conf_admin = ldap_user_conf('admin');
  list($errors, $warnings) = $ldap_user_conf_admin->drupalFormValidate($form_state['values'], $form['#storage']);
  foreach ($errors as $error_name => $error_text) {
    form_set_error($error_name, t($error_text));
  }
  foreach ($warnings as $warning_name => $warning_text) {
    drupal_set_message($warning_text, 'warning');
  }
  $form_state['ldap_warnings'] = (boolean)(count($warnings) > 0);
}

/**
 * submit handler function for ldap_user_admin_form
 */
function ldap_user_admin_form_submit($form, &$form_state) {

  $ldap_user_conf = ldap_user_conf('admin');
  $ldap_user_conf->drupalFormSubmit($form_state['values'], $form['#storage']);  // add form data to object and save or create

  if ($ldap_user_conf->hasError == FALSE) {
    drupal_set_message(t('LDAP user configuration saved'), 'status');
    drupal_goto(LDAP_SERVERS_MENU_BASE_PATH . '/user');
  }
  else {
    form_set_error($conf->errorName, $conf->errorMsg);
    $ldap_user_conf->clearError();
  }

}
