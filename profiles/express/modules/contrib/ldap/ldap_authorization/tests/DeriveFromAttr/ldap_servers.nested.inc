<?php
// $Id$

/**
 * @file
 * simpltest authorization config
 */

$servers['ldapauthor1']['properties']  = array(
    'name'  => 'Test LDAP Server for LDAP Authorization' ,
    'inDatabase' => TRUE,
    'status'  => 1,
    'ldap_type'  => 'ad',
    'address'  => 'ad.myuniversity.edu',
    'port'  => 389,
    'tls'  => FALSE,
    'bind_method' => LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT,
    'basedn'  => array(
       'dc=ad,dc=myuniversity,dc=edu',
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

$servers['ldapauthor1']['methodResponses']['connect'] = LDAP_SUCCESS;

$servers['ldapauthor1']['search_results']['(&(objectClass=group)(memberOf=*)(|(distinguishedname=cn=people,dc=ad,dc=myuniversity,dc=edu)))']['dc=ad,dc=myuniversity,dc=edu'] = array(
    'count' => 0,
  );

$servers['ldapauthor1']['search_results']['(&(objectClass=group)(memberOf=*)(|(distinguishedname=cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu)))']['dc=ad,dc=myuniversity,dc=edu'] = array(
  'count' => 1,
  0 =>
  array (
    'memberof' =>
    array (
      'count' => 1,
      0 => 'cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu',
    ),
    0 => 'memberof',
    'count' => 1,
    'dn' => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu',
  ),


  0 => array(
    'count' => 2,
    'memberOf' => array(
      0 => 'cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu',
      'count' => 1
    ),
    'dn' => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu',
    ),
  );


$servers['ldapauthor1']['search_results']['(&(objectClass=group)(memberOf=*)(|(distinguishedname=cn=developers,ou=it,dc=ad,dc=myuniversity,dc=edu)))']['dc=ad,dc=myuniversity,dc=edu'] = array(
  'count' => 1,
  0 => array(
    'count' => 2,
    'memberOf' => array(
      0 => 'cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu',
      'count' => 1
    ),
    'dn' => 'cn=developers,ou=it,dc=ad,dc=myuniversity,dc=edu',
    ),
  );

$servers['ldapauthor1']['search_results']['(&(objectClass=group)(memberOf=*)(|(distinguishedname=cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu)))']['dc=ad,dc=myuniversity,dc=edu'] = array(
  'count' => 1,
  0 => array(
    'count' => 2,
    'memberOf' => array(
      0 => 'cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu',
      'count' => 1
    ),
    'dn' => 'cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu',
    ),
  );

$servers['ldapauthor1']['search_results']['(&(objectClass=group)(memberOf=*)(|(distinguishedname=cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu)))']['dc=ad,dc=myuniversity,dc=edu'] = array(
  'count' => 1,
  0 => array(
    'count' => 2,
    'memberOf' => array(
      0 => 'cn=people,dc=ad,dc=myuniversity,dc=edu',
      'count' => 1
    ),
    'dn' => 'cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu',
    ),
  );




/**
 * test users should include service account if one is being used
 */

$servers['ldapauthor1']['users']['cn=service-account,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=service-account,dc=ad,dc=myuniversity,dc=edu',
    'mail' => array( 0 => 'service-account@myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'service-account', 'count' => 1),
    'memberOf' => array(
      0 => 'CN=service_accounts,OU=ServiceAccountGroups,DC=ad,DC=myuniversity,DC=edu',
      'count' => 2,
      ),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
  );

$servers['ldapauthor1']['users']['cn=unkool,ou=lost,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=unkool,ou=lost,dc=ad,dc=myuniversity,dc=edu',
    'mail' => array( 0 => 'unkool@nowhere.myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'jkool', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
    'memberOf' => array( 0 => 'cn=unknown_people,ou=nowhere,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );


$servers['ldapauthor1']['users']['cn=justin,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=justin,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu',
    'mail' => array( 0 => 'justin@myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'justin', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
    'memberOf' => array( 0 => 'cn=people,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );


$servers['ldapauthor1']['users']['cn=newkool,ou=it,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=newkool,ou=it,dc=ad,dc=myuniversity,dc=edu',
    'mail' => array( 0 => 'newkool@guests.myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'newkool', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
    'memberOf' => array( 0 => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

$servers['ldapauthor1']['users']['cn=joeprogrammer,ou=it,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=joeprogrammer,ou=it,dc=ad,dc=myuniversity,dc=edu',
    'mail' => array( 0 => 'joeprogrammer@myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'joeprogrammer', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
    'memberOf' => array( 0 => 'cn=developers,ou=it,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

/**
 * nested groups for derive by attr
 *  cn=people,ou=it,dc=ad,dc=myuniversity,dc=edu
 *    cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu
 *      cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu
 *        cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu
 *        cn=developers,ou=it,dc=ad,dc=myuniversity,dc=edu
 *
 * above 4 users results should be:
 *
 *  cn=unkool
 *    cn=unknown_people,ou=nowhere,dc=ad,dc=myuniversity,dc=edu
 *
 *  cn=justin
 *    cn=people,ou=it,dc=ad,dc=myuniversity,dc=edu
 *
 *  cn=newkool
 *    cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu
 *    cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu
 *    cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu
 *    cn=people,ou=it,dc=ad,dc=myuniversity,dc=edu
 *
 *  cn=joeprogrammer
 *    cn=developers,ou=it,dc=ad,dc=myuniversity,dc=edu
 *    cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu
 *    cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu
 *    cn=people,ou=it,dc=ad,dc=myuniversity,dc=edu
 *

 */


/**
 *  derive by attr entries deriveFromAttrAttr=memberOf
 */
$servers['ldapauthor1']['groups']['cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu',
    'memberOf' => array( 0 => 'cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

$servers['ldapauthor1']['groups']['cn=developers,ou=it,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=developers,ou=it,dc=ad,dc=myuniversity,dc=edu',
    'memberOf' => array( 0 => 'cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

$servers['ldapauthor1']['groups']['cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu',
    'memberOf' => array( 0 => 'cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
    'memberOf' => array( 0 => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),  // bogus recursion to test bogus recursion
  );

$servers['ldapauthor1']['groups']['cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu',
    'memberOf' => array( 0 => 'cn=people,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

$servers['ldapauthor1']['groups']['cn=people,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=people,dc=ad,dc=myuniversity,dc=edu',
    'memberOf' => array('count' => 0),
  );

/**
 * some recursion tests
 */

$servers['ldapauthor1']['users']['cn=memento,ou=it,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=memento,ou=it,dc=ad,dc=myuniversity,dc=edu',
    'mail' => array( 0 => 'memento@guests.myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'memento', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
    'memberOf' => array( 0 => 'cn=lessrecursive,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

$servers['ldapauthor1']['groups']['cn=recursive,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=recursive,dc=ad,dc=myuniversity,dc=edu',
    'memberOf' => array( 0 => 'cn=recursive,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
    'memberOf' => array( 0 => 'cn=lessrecursive,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

$servers['ldapauthor1']['groups']['cn=lessrecursive,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=lessrecursive,dc=ad,dc=myuniversity,dc=edu',
    'memberOf' => array( 0 => 'cn=recursive,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

$servers['ldapauthor1']['search_results']['(&(objectClass=group)(memberOf=*)(|(distinguishedname=cn=lessrecursive,dc=ad,dc=myuniversity,dc=edu)))']['dc=ad,dc=myuniversity,dc=edu'] = array(
  'count' => 1,
  0 => array(
    'count' => 2,
    'memberOf' => array(
      0 => 'cn=recursive,dc=ad,dc=myuniversity,dc=edu',
      'count' => 1
    ),
    'dn' => 'cn=lessrecursive,dc=ad,dc=myuniversity,dc=edu',
    ),
  );

$servers['ldapauthor1']['search_results']['(&(objectClass=group)(memberOf=*)(|(distinguishedname=cn=recursive,dc=ad,dc=myuniversity,dc=edu)))']['dc=ad,dc=myuniversity,dc=edu'] = array(
  'count' => 1,
  0 => array(
    'count' => 2,
    'memberOf' => array(
      0 => 'cn=recursive,dc=ad,dc=myuniversity,dc=edu',
      1 => 'cn=lessrecursive,dc=ad,dc=myuniversity,dc=edu',
      'count' => 2
    ),
    'dn' => 'cn=recursive,dc=ad,dc=myuniversity,dc=edu',
    ),
  );
