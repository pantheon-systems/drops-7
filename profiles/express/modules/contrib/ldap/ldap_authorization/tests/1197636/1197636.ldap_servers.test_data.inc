<?php
// $Id$
/**
 * @file
 * simpletest fake data
 */

$servers['fake_server1']['properties']  = array(
  'name'  => 'Test LDAP Server 1 for LDAP Authorization' ,
  'inDatabase' => TRUE,
  'status'  => 1,
  'ldap_type'  => 'ad',
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

$servers['fake_server1']['methodResponses']['connect'] = LDAP_SUCCESS;

$servers['fake_server1']['search_results']['member=cn=verykool,ou=special guests,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu']['ou=groups,dc=ad,dc=myuniversity,dc=edu'] = array(
  0 => array('count' => 1, 'dn' => 'cn=content editors,ou=groups,dc=ad,dc=myuniversity,dc=edu'),
  1 => array('count' => 1, 'dn' => 'cn=content approvers,ou=groups,dc=ad,dc=myuniversity,dc=edu'),
  'count' => 2,
);


/**
 * fake ldap data.  should return data in format that php ldap extension methods would
 *
 * users go in $servers[<sid>]['users'][<dn>] where <sid> is the server id
 */
$servers['fake_server1']['users']['cn=verykool,ou=special guests,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
  'dn' => 'cn=verykool,ou=special guests,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu',
  'distinguishedname' => array( 0 => 'cn=verykool,ou=special guests,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  'mail' => array( 0 => 'verykool@myuniversity.edu', 'count' => 1),
  'sAMAccountName' => array( 0 => 'verykool', 'count' => 1),
  'password' => array( 0 => 'goodpwd', 'count' => 1),
  'memberOf' => array(
    0 => 'cn=netadmins,ou=it,dc=ad,dc=myuniversity,dc=edu',
    'count' => 1,
    ),
);

/**
 * test users should include service account if one is being used
 */
$servers['fake_server1']['users']['cn=service-account,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
  'dn' => 'cn=service-account,dc=ad,dc=myuniversity,dc=edu',
  'distinguishedname' => array( 0 => 'cn=service-account,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  'mail' => array( 0 => 'service-account@myuniversity.edu', 'count' => 1),
  'sAMAccountName' => array( 0 => 'service-account', 'count' => 1),
  'memberOf' => array(
    0 => 'CN=service_accounts,OU=ServiceAccountGroups,DC=ad,DC=myuniversity,DC=edu',
    'count' => 1,
    ),
  'password' => array( 0 => 'goodpwd', 'count' => 1),
);
