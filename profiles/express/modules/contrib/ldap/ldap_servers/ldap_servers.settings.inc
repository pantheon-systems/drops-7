<?php
// $Id: ldap_servers.settings.inc,v 1.3.2.1 2011/02/08 06:01:00 johnbarclay Exp $

/**
 * @file
 * admin interface for general ldap api settings
 *
 */

function ldap_servers_settings() {
  ldap_server_module_load_include('inc', 'ldap_servers', 'ldap_servers.functions');

  if (! ldap_servers_ldap_extension_loaded()) {
    drupal_set_message(t('PHP LDAP Extension is not loaded.'), "warning");
  }

  $https_approaches = array();
  $https_approaches[] = t('Use secure pages or secure login module to redirect to SSL (https)');
  $https_approaches[] = t('Run entire site with SSL (https)');
  $https_approaches[] = t('Remove logon block and redirect all /user page to https via webserver redirect');

  $form['#title'] = "Configure LDAP Preferences";
  $form['ssl'] = array('#type' => 'fieldset', '#title' => t('Require HTTPS on Credential Pages'));
  $form['ssl']['ldap_servers_require_ssl_for_credentails'] = array(
    '#type' => 'checkbox',
    '#title' => t('If checked, modules using LDAP will not allow credentials to
      be entered on or submitted to HTTP pages, only HTTPS. This option should be used with an
      approach to get all logon forms to be https, such as:') .
      theme('item_list', array('items' => $https_approaches)),
    '#default_value' => variable_get('ldap_servers_require_ssl_for_credentails', 1),
  );

  $options = ldap_servers_encrypt_types('encrypt');

   /**  when this is changed, need to decrypt and possibly encrypt pwd in newly selected format
    *   ... thus default needs to be "No Encryption" to avoid confusion.
    */

  $form['previous_encryption'] = array('#type' => 'hidden',  '#default_value' => variable_get('ldap_servers_encryption', LDAP_SERVERS_ENC_TYPE_CLEARTEXT));
  $form['encryption'] = array('#type' => 'fieldset', '#title' => t('Encryption'));
  $form['encryption']['ldap_servers_encryption'] = array(
    '#type' => 'select',
    '#options' => $options,
    '#title' => t('Encrypt Stored LDAP Passwords?'),
    '#default_value' => variable_get('ldap_servers_encryption', LDAP_SERVERS_ENC_TYPE_CLEARTEXT),
    '#description' => t('With encryption, passwords will be stored in encrypted form.
    This is two way encryption because the actual password needs to used to bind to LDAP.
    So it offers minimal defense if someone gets in the filespace.  It mainly helps avoid the accidental
    discovery of a clear text password.'),
    );


    // $options will be empty if server does not support mcrypt.
  // Disable the form field and explain this to the user.
  if (empty($options)) {
    $form['encryption']['ldap_servers_encryption']['#options'] = array(LDAP_SERVERS_ENC_TYPE_CLEARTEXT => t('Not available.'));
    $form['encryption']['ldap_servers_encryption']['#disabled'] = TRUE;
    $form['encryption']['ldap_servers_encryption']['#description'] .= ' <strong>' . t('Encryption is not supported on this web server.') . '</strong>';
  }

  $form = system_settings_form($form);
  array_unshift($form['#submit'], 'ldap_servers_settings_submit');  // needs to be first
  return $form;
}

function ldap_servers_settings_submit($form, &$form_state) {
  if ($form_state['submitted']) {
    $new_encyption = $form_state['values']['ldap_servers_encryption'];
    $old_encyption = $form_state['values']['previous_encryption'];

    // use db instead of functions to avoid classes encryption and decryption
    if ($new_encyption != $old_encyption) {
      $servers = db_query("SELECT sid, bindpw FROM {ldap_servers} WHERE bindpw is not NULL AND bindpw <> ''")->fetchAllAssoc('sid');
      foreach ($servers as $sid => $server) {
        $decrypted_bind_pwd = ldap_servers_decrypt($server->bindpw, $old_encyption);
        $rencrypted = ldap_servers_encrypt($decrypted_bind_pwd, $new_encyption);
        db_query("UPDATE {ldap_servers} SET bindpw = :bindpw WHERE sid = :sid", array(':bindpw' => $rencrypted, ':sid' => $sid));
      }
    }
  }
}
