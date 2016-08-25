<?php
/**
 * @file
 * cu_ldap.strongarm.inc
 */

/**
 * Implements hook_strongarm().
 */
function cu_ldap_strongarm() {
  $export = array();

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ldap_authentication_conf';
  $strongarm->value = array(
    'sids' => array(
      'directory' => 'directory',
    ),
    'authenticationMode' => 2,
    'loginConflictResolve' => 2,
    'acctCreation' => 4,
    'loginUIUsernameTxt' => NULL,
    'loginUIPasswordTxt' => NULL,
    'ldapUserHelpLinkUrl' => NULL,
    'ldapUserHelpLinkText' => 'Logon Help',
    'emailOption' => 3,
    'emailUpdate' => 1,
    'allowOnlyIfTextInDn' => array(),
    'excludeIfTextInDn' => array(),
    'allowTestPhp' => '',
    'excludeIfNoAuthorizations' => NULL,
    'ssoRemoteUserStripDomainName' => NULL,
    'seamlessLogin' => NULL,
    'ldapImplementation' => NULL,
    'cookieExpire' => NULL,
  );
  $export['ldap_authentication_conf'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ldap_servers_encryption';
  $strongarm->value = '10';
  $export['ldap_servers_encryption'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ldap_servers_require_ssl_for_credentails';
  $strongarm->value = 1;
  $export['ldap_servers_require_ssl_for_credentails'] = $strongarm;

  return $export;
}
