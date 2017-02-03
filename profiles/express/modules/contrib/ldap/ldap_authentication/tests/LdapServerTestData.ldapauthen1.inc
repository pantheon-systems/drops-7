<?php
// $Id: LdapServerTest.class.inc,v 1.4.2.1 2011/02/08 06:01:00 johnbarclay Exp $

/**
 * @file
 * test configurations for LdapServerTest.class.php
 * file name should be of form LdapServerTestData.<sid>.inc
 * where sid is the server id data is used for.
 *
 */

$test_data = array();

/**
 * $test_data['properties'] are all the initial properties of the instantiated LdapServerTest object
 */

$test_data['servers']['ldapauthen1']['properties']  = array(

    'sid' => 'ldapauthen1',
    'name'  => 'Test LDAP Server 1 for LDAP Authentication' ,
    'inDatabase' => TRUE,
    'status'  => 1,
    'type'  => 'ad',
    'address'  => 'ad.myuniversity.edu',
    'port'  => 389,
    'tls'  => FALSE,
    'bind_method' => LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT,
    'basedn'  => array(
       'ou=campus accounts,dc=ad,dc=myuniversity,dc=edu',
       'ou=education,dc=ad,dc=myuniversity,dc=edu',
       'ou=guest accounts,dc=ad,dc=myuniversity,dc=edu',
      ),
    'binddn'  => 'cn=service-account,dc=ad,dc=myuniversity,dc=edu',
    'bindpw' => 'goodpwd',
    'user_dn_expression' => 'user_dn_expression',
    'user_attr'  => 'sAMAccountName',
    'mail_attr'  => 'mail',
    'ldapToDrupalUserPhp'  => NULL,
    'testingDrupalUsername'  => 'jdoe',
    'groupObjectClass' => 'group',

  );

/**
 *
 * method responses are stored in array $test_data['methodResponses']
 * where keys are:
 * <method_name>
 * parameter1,
 * parameter2,
 * ...
 *
 * and value is the response test ldap server is expected to return. values
 * can be scalar, array, object, etc, depending on what the method being mimicked
 * is expected to return
 */

$test_data['servers']['ldapauthen1']['methodResponses']['connect'] = LDAP_SUCCESS;

$test_data['servers']['ldapauthen1']['search_results']['member=cn=verykool,ou=special guests,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu']['ou=groups,dc=ad,dc=myuniversity,dc=edu'] = array(
    0 => array('count' => 1, 'dn' => 'cn=content editors,ou=groups,dc=ad,dc=myuniversity,dc=edu'),
    1 => array('count' => 1, 'dn' => 'cn=content approvers,ou=groups,dc=ad,dc=myuniversity,dc=edu'),
    'count' => 2,
  );

$test_data['servers']['ldapauthen1']['search_results']['member=cn=jkool,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu']['ou=groups,dc=ad,dc=myuniversity,dc=edu'] = array(
    0 => array('count' => 1, 'dn' => 'cn=content editors,ou=groups,dc=ad,dc=myuniversity,dc=edu'),
    'count' => 1,
  );

/**
 * fake user data array below 'attr' should mimick ldap user result data
 */
$test_data['servers']['ldapauthen1']['users']['cn=jdoe,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=jdoe,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=jdoe,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
    'mail' => array( 0 => 'jdoe@myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'jdoe', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
  );


$test_data['servers']['ldapauthen1']['users']['cn=jkool,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=jkool,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=jkool,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
    'mail' => array( 0 => 'jkool@guests.myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'jkool', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
    'memberOf' => array( 0 => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

$test_data['servers']['ldapauthen1']['users']['cn=unkool,ou=lost,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=unkool,ou=lost,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=unkool,ou=lost,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
    'mail' => array( 0 => 'unkool@nowhere.myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'jkool', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
    'memberOf' => array( 0 => 'cn=unknown_people,ou=nowhere,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

$test_data['servers']['ldapauthen1']['users']['cn=verykool,ou=special guests,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=verykool,ou=special guests,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=verykool,ou=special guests,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
    'mail' => array( 0 => 'verykool@myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'verykool', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
    'meMBErof' => array(
      0 => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu',
      1 => 'CN=NETadmins,ou=it,dc=ad,dc=myuniversity,dc=edu',
      'count' => 2,
      ),
  );


/**
 * test users should include service account if one is being used
 */
$test_data['servers']['ldapauthen1']['users']['cn=service-account,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=service-account,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=service-account,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
    'mail' => array( 0 => 'service-account@myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'service-account', 'count' => 1),
    'memberOf' => array(
      0 => 'CN=service_accounts,OU=ServiceAccountGroups,DC=ad,DC=myuniversity,DC=edu',
      'count' => 2,
      ),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
  );


/** the ldap_authentication data is keyed on the test id such
 * that the same ldap test data above can be reused for all authentication configurations
 */

$test_data['ldap_authentication']['MixedModeUserLogon'] = array(
    'sids' => array('ldapauthen1' => 'ldapauthen1'),
    'authenticationMode' => LDAP_AUTHENTICATION_MIXED,
    'loginConflictResolve' => LDAP_AUTHENTICATION_CONFLICT_LOG,
    'acctCreation' => LDAP_AUTHENTICATION_ACCT_CREATION_LDAP_BEHAVIOR,
    'ldapUserHelpLinkUrl' => '',
    'ldapUserHelpLinkText' => '',
    'emailOption' => LDAP_AUTHENTICATION_EMAIL_FIELD_DISABLE,
    'emailUpdate' => LDAP_AUTHENTICATION_EMAIL_UPDATE_ON_LDAP_CHANGE_DISABLE,
    'allowOnlyIfTextInDn' => NULL,
    'excludeIfTextInDn' => NULL,
    'allowTestPhp' => NULL,
    'excludeIfNoAuthorizations' => LDAP_AUTHENTICATION_EXCL_IF_NO_AUTHZ_DEFAULT,
  );

$test_data['ldap_authentication']['ExclusiveModeUserLogon'] = array(
    'sids' => array('ldapauthen1' => 'ldapauthen1'),
    'authenticationMode' => LDAP_AUTHENTICATION_EXCLUSIVE,
    'loginConflictResolve' => LDAP_AUTHENTICATION_CONFLICT_LOG,
    'acctCreation' => LDAP_AUTHENTICATION_ACCT_CREATION_LDAP_BEHAVIOR,
    'ldapUserHelpLinkUrl' => '',
    'ldapUserHelpLinkText' => '',
    'emailOption' => LDAP_AUTHENTICATION_EMAIL_FIELD_DISABLE,
    'emailUpdate' => LDAP_AUTHENTICATION_EMAIL_UPDATE_ON_LDAP_CHANGE_DISABLE,
    'allowOnlyIfTextInDn' => NULL,
    'excludeIfTextInDn' => NULL,
    'allowTestPhp' => NULL,
    'excludeIfNoAuthorizations' => LDAP_AUTHENTICATION_EXCL_IF_NO_AUTHZ_DEFAULT,
  );

$test_data['ldap_authentication']['WL1'] = array(
    'sids' => array('ldapauthen1' => 'ldapauthen1'),
    'authenticationMode' => LDAP_AUTHENTICATION_EXCLUSIVE,
    'loginConflictResolve' => LDAP_AUTHENTICATION_CONFLICT_LOG,
    'acctCreation' => LDAP_AUTHENTICATION_ACCT_CREATION_LDAP_BEHAVIOR,
    'ldapUserHelpLinkUrl' => '',
    'ldapUserHelpLinkText' => '',
    'emailOption' => LDAP_AUTHENTICATION_EMAIL_FIELD_DISABLE,
    'emailUpdate' => LDAP_AUTHENTICATION_EMAIL_UPDATE_ON_LDAP_CHANGE_DISABLE,
    'allowOnlyIfTextInDn' => NULL,
    'excludeIfTextInDn' => NULL,
    'allowTestPhp' => NULL,
    'excludeIfNoAuthorizations' => 0,
  );

// single sign on tests
$test_data['ldap_authentication']['MixedModeUserLogonSSO'] = $test_data['ldap_authentication']['MixedModeUserLogon'];
$test_data['ldap_authentication']['MixedModeUserLogonSSO']['ssoRemoteUserStripDomainName'] = FALSE;
$test_data['ldap_authentication']['MixedModeUserLogonSSO']['seamlessLogin'] = TRUE;
$test_data['ldap_authentication']['MixedModeUserLogonSSO']['ldapImplementation'] = 'mod_auth_sspi';
$test_data['ldap_authentication']['MixedModeUserLogonSSO']['cookieExpire'] = 3600;

$test_data['ldap_authentication']['ExclusiveModeUserLogonSSO'] = $test_data['ldap_authentication']['ExclusiveModeUserLogon'];
$test_data['ldap_authentication']['ExclusiveModeUserLogonSSO']['ssoRemoteUserStripDomainName'] = FALSE;
$test_data['ldap_authentication']['ExclusiveModeUserLogonSSO']['seamlessLogin'] = FALSE;
$test_data['ldap_authentication']['ExclusiveModeUserLogonSSO']['ldapImplementation'] = 'mod_auth_sspi';
$test_data['ldap_authentication']['ExclusiveModeUserLogonSSO']['cookieExpire'] = 3600;




$test_data['ldap_authorization_conf']['consumer_conf']['sid'] = 'ldapauthen1';

$test_data['ldap_authorization_conf']['consumer_conf']['consumerType'] = 'drupal_role';  // change as desired
$test_data['ldap_authorization_conf']['consumer_conf']['consumerModule'] = 'ldap_authorization_drupal_role';

$test_data['ldap_authorization_conf']['consumer_conf']['description'] = 'UIUC AD';
$test_data['ldap_authorization_conf']['consumer_conf']['status'] = 1;
$test_data['ldap_authorization_conf']['consumer_conf']['onlyApplyToLdapAuthenticated'] = 1;

$test_data['ldap_authorization_conf']['consumer_conf']['deriveFromDn'] = 1;
$test_data['ldap_authorization_conf']['consumer_conf']['deriveFromDnAttr'] = 'ou';

$test_data['ldap_authorization_conf']['consumer_conf']['deriveFromAttr'] = 0;
$test_data['ldap_authorization_conf']['consumer_conf']['deriveFromAttrAttr'] = array();

$test_data['ldap_authorization_conf']['consumer_conf']['deriveFromEntry'] = 0;
$test_data['ldap_authorization_conf']['consumer_conf']['deriveFromEntryEntries'] = NULL;
$test_data['ldap_authorization_conf']['consumer_conf']['deriveFromEntryEntriesAttr'] = NULL;
$test_data['ldap_authorization_conf']['consumer_conf']['deriveFromEntryMembershipAttr'] = NULL;


$test_data['ldap_authorization_conf']['consumer_conf']['mappings'][] = array('Campus Accounts', 'campus accounts');
$test_data['ldap_authorization_conf']['consumer_conf']['mappings'][] = array('guest accounts', 'guests');
$test_data['ldap_authorization_conf']['consumer_conf']['mappings'][] = array('special guests', 'special guests');
$test_data['ldap_authorization_conf']['consumer_conf']['mappings'][] = array('cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu', 'sysadmins');

$test_data['ldap_authorization_conf']['consumer_conf']['useMappingsAsFilter'] = 1;

$test_data['ldap_authorization_conf']['consumer_conf']['synchOnLogon'] = 1;
$test_data['ldap_authorization_conf']['consumer_conf']['synchManually'] = 1;

$test_data['ldap_authorization_conf']['consumer_conf']['revokeLdapProvisioned'] = 1;
$test_data['ldap_authorization_conf']['consumer_conf']['createConsumers'] = 1;
$test_data['ldap_authorization_conf']['consumer_conf']['regrantLdapProvisioned'] = 1;
