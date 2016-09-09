<?php

/**
 * @file
 * Administrative page callbacks for the ldap_authentication module.
 */


/**
 * form for adding, updating, and deleting a single ldap authorization mapping
 *
 * @param <type> $form
 * @param <type> $form_state
 * @return array drupal form array
 */
function ldap_authentication_admin_form($form, &$form_state) {
    ldap_servers_module_load_include('php', 'ldap_authentication', 'LdapAuthenticationConfAdmin.class');
    $auth_conf = new LdapAuthenticationConfAdmin();
    return $auth_conf->drupalForm();
}


/**
 * validate handler for the ldap_authentication_admin_form
 */
function ldap_authentication_admin_form_validate($form, &$form_state) {

  ldap_servers_module_load_include('php', 'ldap_authentication', 'LdapAuthenticationConfAdmin.class');
  $auth_conf = new LdapAuthenticationConfAdmin();
  $errors = $auth_conf->drupalFormValidate($form_state['values']);
  foreach ($errors as $error_name => $error_text) {
    form_set_error($error_name, t($error_text));
  }

}


/**
 * submit handler function for ldap_authentication_admin_form
 */

function ldap_authentication_admin_form_submit($form, &$form_state) {

  ldap_servers_module_load_include('php', 'ldap_authentication', 'LdapAuthenticationConfAdmin.class');
  $auth_conf = new LdapAuthenticationConfAdmin();
  $auth_conf->drupalFormSubmit($form_state['values']);  // add form data to object and save or create
  if (!$auth_conf->hasEnabledAuthenticationServers()) {
    drupal_set_message(t('No LDAP servers are enabled for authentication,
      so no LDAP Authentication can take place.  This essentially disables
      LDAP Authentication.'), 'warning');
  }
  if ($auth_conf->hasError == FALSE) {
    drupal_set_message(t('LDAP Authentication configuration saved'), 'status');
    drupal_goto(LDAP_SERVERS_MENU_BASE_PATH . '/authentication');
  }
  else {
    form_set_error($auth_conf->errorName, $auth_conf->errorMsg);
    $auth_conf->clearError();
  }

}
