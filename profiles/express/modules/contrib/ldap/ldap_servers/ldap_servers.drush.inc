<?php

/**
 * @file
 *   LDAP module drush integration.
 */

/**
 * Implements hook_drush_command().
 *
 * @return
 *   An associative array describing your command(s).
 *
 * @see drush_parse_command()
 */
function ldap_servers_drush_command() {
  $items['ldap-servers-set-password'] = array(
    'description' => 'Set the password of an otherwise already configured LDAP server.',
    'aliases' => array('lssp'),
    'arguments' => array(
      'ldap_sid' => 'The configured LDAP server ID.'
    ),
    'options' => array(
      'password' => 'The password to set for the server.'
    ),
    'examples' => array(
      'drush lssp LDAP_SERVER_ID --password="PASSWORD"' => 'Sets the password for LDAP_SERVER_ID. Replace LDAP_SERVER_ID with the "Machine name for this configuration." of
        the server found at /admin/config/people/ldap/servers, and replace PASSWORD with your password.',
    ),
  );
  return $items;
}

/**
 * Callback for the ldap-servers-set-password command.
 *
 * @param string $ldap_sid The server ID for which to set the password.
 * @option string --password Used to provide the password via an option in the Drush command.
 *
 * @return
 *   Error or success message.
 */
function drush_ldap_servers_set_password($ldap_sid = NULL) {
  // Check for the argument.
  if (!isset($ldap_sid)) {
    return drush_set_error(t('The server ID was not included as an argument. Use the "Machine name for this server configuration." found on the edit screen for that server.'));
  }
  // Instantiate the server configuration with the provided sid.
  ldap_servers_module_load_include('php', 'ldap_servers', 'LdapServerAdmin.class');
  $ldap_server = new LdapServerAdmin($ldap_sid);
  if (isset($ldap_server)) {
    // Retrieves the password from the --password option set in the drush command.
    $ldap_server->bindpw = drush_get_option('password');
    if (!isset($ldap_server->bindpw)) {
      return drupal_set_message(dt('No password was provided for @ldap_sid. A password has not been set.', array('@ldap_sid' => $ldap_sid)));
    }
    // Save the server configuration with the password.
    $ldap_server->save('edit');
    // Notify of success.
    return drupal_set_message(dt('Password for @ldap_sid has been set.', array('@ldap_sid' => $ldap_sid)));
  }
  // Provided server ID does not match any of the existing server IDs.
  return drush_set_error(dt('@ldap_sid does not match the server ID of any configured servers.  Use the "Machine name for this server configuration." found on the edit screen for that server.',
    array('@ldap_sid' => $ldap_sid)));
}
