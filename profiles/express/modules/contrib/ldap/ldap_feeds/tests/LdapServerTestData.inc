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

$test_data['server']['properties']  = array(

    'sid' => 'ldapfeeds',
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

$test_data['server']['methodResponses']['connect'] = LDAP_SUCCESS;

$test_data['server']['search_results']['objectclass=user']['ou=campus accounts,dc=ad,dc=myuniversity,dc=edu'] = array(
    0 => array(
      'count' => 4,
      'dn' => 'cn=jkool,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu',
      'mail' => 'jkool@myuniversity.edu',
      'cn' => 'jkool',
      'sn' => 'kool',
      ),
    1 => array(
      'count' => 4,
      'dn' => 'cn=bkool,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu',
      'mail' => 'bkool@myuniversity.edu',
      'cn' => 'bkool',
      'sn' => 'kool',
      ),
    2 => array(
      'count' => 4,
      'dn' => 'cn=rkool,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu',
      'mail' => 'rkool@myuniversity.edu',
      'cn' => 'rkool',
      'sn' => 'kool',
      ),
    'count' => 3,
  );

$test_data['server']['search_results']['(member=cn=jkool,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu)']['ou=groups,dc=ad,dc=myuniversity,dc=edu'] = array(
    0 => array('count' => 1, 'dn' => 'cn=content editors,ou=groups,dc=ad,dc=myuniversity,dc=edu'),
    'count' => 1,
  );

/**
 * fake user data array below 'attr' should mimick ldap user result data
 */
$test_data['server']['users']['cn=jdoe,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=jdoe,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu',
    'mail' => array( 0 => 'jdoe@myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'jdoe', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
  );


$test_data['server']['users']['cn=jkool,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=jkool,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu',
    'mail' => array( 0 => 'jkool@guests.myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'jkool', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
    'memberOf' => array( 0 => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

$test_data['server']['users']['cn=unkool,ou=lost,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=unkool,ou=lost,dc=ad,dc=myuniversity,dc=edu',
    'mail' => array( 0 => 'unkool@nowhere.myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'jkool', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
    'memberOf' => array( 0 => 'cn=unknown_people,ou=nowhere,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

$test_data['server']['users']['cn=verykool,ou=special guests,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=verykool,ou=special guests,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu',
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
$test_data['server']['users']['cn=service-account,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=service-account,dc=ad,dc=myuniversity,dc=edu',
    'mail' => array( 0 => 'service-account@myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'service-account', 'count' => 1),
    'memberOf' => array(
      0 => 'CN=service_accounts,OU=ServiceAccountGroups,DC=ad,DC=myuniversity,DC=edu',
      'count' => 2,
      ),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
  );
