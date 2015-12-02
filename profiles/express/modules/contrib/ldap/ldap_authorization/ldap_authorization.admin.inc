<?php
// $Id: ldap_authorization.admin.inc,v 1.1.4.2 2011/02/08 06:01:00 johnbarclay Exp $

/**
 * @file
 * Administrative page callbacks for the ldap_authorization module.  Consumer configuration form and index.
 */


/**
 * index of ldap authorization configurations
 *
 * @return string html table
 */

function ldap_authorizations_admin_index() {

  $consumers = ldap_authorization_get_consumers(NULL, TRUE, FALSE);
  if (!is_array($consumers) || count($consumers) == 0) {
    drupal_set_message(t('No authorization consumer modules are enabled.  Enable
    LDAP Authorization Drupal Roles, OG LDAP, or another LDAP Authorization consuming module'), 'warning');
  }

  $servers = ldap_servers_get_servers(NULL, 'enabled');
  if (count($servers) == 0) {
    return t('ldap authorization can not be set up until ldap servers are configured.') . '  ' .
    l(t('Add LDAP Server'), 'admin/config/people/ldap/servers/add');
  }

  foreach ($consumers as $consumer_type => $consumer) {
    $consumers[$consumer_type] = ldap_authorization_get_consumer_object($consumer_type);
  }
  return theme('ldap_authorization_admin_index', array('consumers' => $consumers));

}



/**
 * form for adding, updating, and deleting a single ldap authorization configuration
 *
 * @param form array $form
 * @param form state array $form_state
 * @param string $op (add, edit, or delete)
 * @param string $consumer_type e.g. drupal_roles, og_group, etc.  Only needed for adds
 * @return drupal form array
 */

function ldap_authorization_admin_form($form, &$form_state, $consumer_type, $op = NULL) {
  ldap_server_module_load_include('php', 'ldap_authorization', 'LdapAuthorizationConsumerConfAdmin.class');
  $consumer = ldap_authorization_get_consumer_object($consumer_type);

  if ($op == 'add' && is_object($consumer->consumerConf) && $consumer->consumerConf->inDatabase) {
    drupal_set_message(t('Only one configuration is allowed per consumer type.
      Configuration already exists for the cosumer type %consumer_type.  Please edit that configuration.',
      array('%consumer_type' => $consumer_type)), 'warning');
    drupal_goto(LDAP_SERVERS_MENU_BASE_PATH . '/authorization');
  }

  if (($op == 'edit' || $op == 'delete') && !is_object($consumer->consumerConf)) {
    drupal_set_message(t('Bad LDAP Authorization Configuration URL.'), 'error');
    drupal_goto(LDAP_SERVERS_MENU_BASE_PATH . '/authorization');
  }

  $servers = ldap_servers_get_servers(NULL, 'enabled');
  if (count($servers) == 0) {
    drupal_set_message(t('No ldap servers configured.  Please configure a server before an ldap authorization.'), 'error');
    drupal_goto('admin/config/people/ldap/authorization');
  }


  $new = ($op == 'add');
  $consumer_conf_admin = new LdapAuthorizationConsumerConfAdmin($consumer, $new);

  foreach ($servers as $sid => $server) {
    $server_options[$sid] = $server->name;
  }
  return $consumer_conf_admin->drupalForm($server_options, $op);

}


/**
 * validate handler for the ldap_authorization_admin_form
 */

function ldap_authorization_admin_form_validate($form, &$form_state) {

  list($consumer, $op, $op_past, $new)  = _ldap_authorization_admin_parse_form($form, $form_state);
  $values = $form_state['values'];
  ldap_server_module_load_include('php', 'ldap_authorization', 'LdapAuthorizationConsumerConfAdmin.class');
  $consumer_conf_admin = new LdapAuthorizationConsumerConfAdmin($consumer, $new);

  $errors = $consumer_conf_admin->drupalFormValidate($op, $values);
  foreach ($errors as $error_name => $error_text) {
    $error_text = check_plain($error_text);
    form_set_error($error_name, t($error_text));
  }

}


/**
 * submit handler function for ldap_authorization_admin_form
 */

function ldap_authorization_admin_form_submit($form, &$form_state) {
  list($consumer, $op, $op_past_tense, $new)  = _ldap_authorization_admin_parse_form($form, $form_state);
  $values = $form_state['values'];
  ldap_server_module_load_include('php', 'ldap_authorization', 'LdapAuthorizationConsumerConfAdmin.class');
  $consumer_conf = new LdapAuthorizationConsumerConfAdmin($consumer, $new);
  $consumer_conf->drupalFormSubmit($op, $values);  // add form data to object and save or create

  if ($consumer_conf->hasError == FALSE) {
    drupal_set_message(t('LDAP Authorization %name !verb', array('!verb' => $op_past_tense, '%name' => $consumer->name)), 'status');
    drupal_goto(LDAP_SERVERS_MENU_BASE_PATH . '/authorization');
  }

  form_set_error($consumer_conf->errorName, $consumer_conf->errorMsg);
  $consumer_conf->clearError();

}

/**
 * helper function for parsing ldap authorization config form
 */

function _ldap_authorization_admin_parse_form($form, &$form_state) {
  $op = drupal_strtolower($form_state['clicked_button']['#value']);
  $values = $form_state['values'];

  if ($values['consumer_type']) {
    $consumer_type = $values['consumer_type'];
    $consumer = ldap_authorization_get_consumer_object($consumer_type);
  }
  else {
    return FALSE;
  }

  switch ($op) {
    case 'add':
      $op_past_tense = 'Added';
      $new = TRUE;
    break;

    case 'save':
    case 'update':
    case 'edit':
      $op_past_tense = 'Updated';
      $new = FALSE;
    break;

    case 'delete':
      $op_past_tense = 'Deleted';
      $new = FALSE;
    break;
  }

  return array($consumer, $op, $op_past_tense, $new);

}
