<?php

/**
 * @file
 *
 */

function ldap_test_ldap_authentication_data() {

  $conf['default'] = array(
      'sids' => array('activedirectory1' => 'activedirectory1'),
      'authenticationMode' => LDAP_AUTHENTICATION_MIXED,
      'emailOption' => LDAP_AUTHENTICATION_EMAIL_FIELD_DISABLE,
      'emailUpdate' => LDAP_AUTHENTICATION_EMAIL_UPDATE_ON_LDAP_CHANGE_DISABLE,
      'loginUIUsernameTxt' => 'Hogwarts Username',
      'loginUIPasswordTxt' => 'Hogwards LDAP Password',
      'ldapUserHelpLinkUrl' => 'https://passwords.hogwarts.edu/',
      'ldapUserHelpLinkText' => 'Hogwarts IT Password Support Page',
      'allowOnlyIfTextInDn' =>  NULL,
      'excludeIfTextInDn' =>  NULL,
      'allowTestPhp' =>  NULL,
      'excludeIfNoAuthorizations' =>  LDAP_AUTHENTICATION_EXCL_IF_NO_AUTHZ_DEFAULT,
      'ssoEnabled' =>  FALSE,
    );

  $conf['MixedModeUserLogon'] = $conf['default'];

  $conf['MixedModeUserLogon3'] = $conf['default'];
  $conf['MixedModeUserLogon3']['sids'] = array('activedirectory1' => 'activedirectory1');

  $conf['ExclusiveModeUserLogon'] = $conf['default'];
  $conf['ExclusiveModeUserLogon']['authenticationMode'] = LDAP_AUTHENTICATION_EXCLUSIVE;

  $conf['SSOUserLogon'] = $conf['default'];
  $conf['SSOUserLogon']['authenticationMode'] = LDAP_AUTHENTICATION_EXCLUSIVE;
  $conf['SSOUserLogon']['ssoEnabled'] = TRUE;
  $conf['SSOUserLogon']['ssoRemoteUserStripDomainName'] = FALSE;
  $conf['SSOUserLogon']['cookieExpire'] = 3600; // -- 0, 3600, 86400, 604800, 2592000, 31536000, 315360000
  $conf['SSOUserLogon']['ldapImplementation'] = 'mod_auth_sspi'; //  'mod_auth_kerb'

  $conf['ExclusiveModeUserLogon3'] = $conf['default'];
  $conf['ExclusiveModeUserLogon3']['sids'] = array('activedirectory1' => 'activedirectory1');
  $conf['ExclusiveModeUserLogon3']['authenticationMode'] = LDAP_AUTHENTICATION_EXCLUSIVE;


  $conf['WL1'] = $conf['default'];
  $conf['WL1']['authenticationMode'] = LDAP_AUTHENTICATION_EXCLUSIVE;


  $conf['WL3'] = $conf['default'];
  $conf['WL3']['sids'] = array('activedirectory1' => 'activedirectory1');
  $conf['WL3']['authenticationMode'] = LDAP_AUTHENTICATION_EXCLUSIVE;


  // single sign on tests
  $conf['MixedModeUserLogonSSO'] = $conf['MixedModeUserLogon'];
  $conf['MixedModeUserLogonSSO']['ssoRemoteUserStripDomainName'] = FALSE;
  $conf['MixedModeUserLogonSSO']['seamlessLogin'] = TRUE;
  $conf['MixedModeUserLogonSSO']['ldapImplementation'] = 'mod_auth_sspi';
  $conf['MixedModeUserLogonSSO']['cookieExpire'] = 3600;

  $conf['ExclusiveModeUserLogonSSO'] = $conf['ExclusiveModeUserLogon'];
  $conf['ExclusiveModeUserLogonSSO']['ssoRemoteUserStripDomainName'] = FALSE;
  $conf['ExclusiveModeUserLogonSSO']['seamlessLogin'] = FALSE;
  $conf['ExclusiveModeUserLogonSSO']['ldapImplementation'] = 'mod_auth_sspi';
  $conf['ExclusiveModeUserLogonSSO']['cookieExpire'] = 3600;

  return $conf;

}
