<?php
// $Id$

/**
 * @file
 * simpltest server config
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
    'user_dn_expression' => '',
    'user_attr'  => 'uid',
    'mail_attr'  => 'mail',
    'ldapToDrupalUserPhp'  => NULL,
    'testingDrupalUsername'  => 'jdoe',
    'groupObjectClass' => 'groupOfUniqueNames',
  );



$servers['ldapauthor1']['methodResponses']['connect'] = LDAP_SUCCESS;


/**
 * fake user data array below 'attr' should mimick ldap user result data
 */

/***** users ********/

$servers['ldapauthor1']['users']['uid=unkool,ou=lost,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'uid=unkool,ou=lost,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'uid=unkool,ou=lost,dc=ad,dc=myuniversity,dc=edu'),
    'mail' => array( 0 => 'unkool@nowhere.myuniversity.edu', 'count' => 1),
    'uid' => array( 0 => 'unkool', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
  );

$servers['ldapauthor1']['users']['uid=jdoe,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'uid=jdoe,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'uid=jdoe,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu'),
    'mail' => array( 0 => 'jdoe@myuniversity.edu', 'count' => 1),
    'uid' => array( 0 => 'jdoe', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
  );

$servers['ldapauthor1']['users']['uid=joesysadmin,ou=it,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'uid=joesysadmin,ou=it,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'uid=joesysadmin,ou=it,dc=ad,dc=myuniversity,dc=edu'),
    'mail' => array( 0 => 'joesysadmin@myuniversity.edu', 'count' => 1),
    'uid' => array( 0 => 'joesysadmin', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
  );

$servers['ldapauthor1']['users']['uid=joeprojectmanager,ou=it,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'uid=joeprojectmanager,ou=it,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'uid=joeprojectmanager,ou=it,dc=ad,dc=myuniversity,dc=edu'),
    'mail' => array( 0 => 'joeprojectmanager@myuniversity.edu', 'count' => 1),
    'uid' => array( 0 => 'joeprojectmanager', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
  );

$servers['ldapauthor1']['users']['uid=joeprogrammer,ou=it,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'uid=joeprogrammer,ou=it,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'uid=joeprogrammer,ou=it,dc=ad,dc=myuniversity,dc=edu'),
    'mail' => array( 0 => 'joeprogrammer@myuniversity.edu', 'count' => 1),
    'uid' => array( 0 => 'joeprogrammer', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
  );

$servers['ldapauthor1']['users']['uid=service-account,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'uid=service-account,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'uid=service-account,dc=ad,dc=myuniversity,dc=edu'),
    'mail' => array( 0 => 'service-account@myuniversity.edu', 'count' => 1),
    'uid' => array( 0 => 'service-account', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
  );

/***** groups ********/

$servers['ldapauthor1']['groups']['cn=sysadmins,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=sysadmins,cn=groups,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=sysadmins,cn=groups,dc=ad,dc=myuniversity,dc=edu'),
    'objectclass' => array( 0 => 'groupofuniquenames', 'count' => 1),
    'uniquemember' => array(
      'count' => 2,
      0 => 'uid=joeprogrammer,ou=it,dc=ad,dc=myuniversity,dc=edu',
      1 => 'cn=sysadmins,cn=groups,dc=ad,dc=myuniversity,dc=edu',
    ),
  );

$servers['ldapauthor1']['groups']['cn=developers,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=developers,cn=groups,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=developers,cn=groups,dc=ad,dc=myuniversity,dc=edu'),
    'objectclass' => array( 0 => 'groupofuniquenames', 'count' => 1),
    'uniquemember' => array(
      'count' => 1,
      0 => 'uid=joeprogrammer,ou=it,dc=ad,dc=myuniversity,dc=edu',
    ),
  );

$servers['ldapauthor1']['groups']['cn=it,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'count' => 3,
    'dn' => 'cn=it,cn=groups,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=it,cn=groups,dc=ad,dc=myuniversity,dc=edu'),
    'objectclass' => array( 0 => 'groupofuniquenames', 'count' => 1),
    'uniquemember' => array(
      'count' => 3,
      0 => 'cn=developers,cn=groups,dc=ad,dc=myuniversity,dc=edu',
      1 => 'cn=sysadmins,cn=groups,dc=ad,dc=myuniversity,dc=edu',
      2 => 'uid=joeprojectmanager,ou=it,dc=ad,dc=myuniversity,dc=edu',
    ),
  );


$servers['ldapauthor1']['groups']['cn=staff,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=staff,cn=groups,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=staff,cn=groups,dc=ad,dc=myuniversity,dc=edu'),
    'objectclass' => array( 0 => 'groupofuniquenames', 'count' => 1),
    'uniquemember' => array(
    'count' => 2,
      0 => 'cn=it,cn=groups,dc=ad,dc=myuniversity,dc=edu',
      1 => 'uid=unkool,ou=lost,dc=ad,dc=myuniversity,dc=edu',
    ),
  );

$servers['ldapauthor1']['groups']['cn=students,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=students,cn=groups,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=students,cn=groups,dc=ad,dc=myuniversity,dc=edu'),
    'objectclass' => array( 0 => 'groupofuniquenames', 'count' => 1),
    'uniquemember' => array(
      'count' => 1,
      0 => 'uid=jdoe,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu',
    ),
  );

$servers['ldapauthor1']['groups']['cn=people,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=people,cn=groups,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=people,cn=groups,dc=ad,dc=myuniversity,dc=edu'),
    'objectclass' => array( 0 => 'groupofuniquenames', 'count' => 1),
    'uniquemember' => array(
       'count' => 2,
       0 => 'cn=students,cn=groups,dc=ad,dc=myuniversity,dc=edu',
       1 => 'cn=staff,cn=groups,dc=ad,dc=myuniversity,dc=edu',
    ),
  );



// ************************** search results **************************************/

$base_dn = 'dc=ad,dc=myuniversity,dc=edu';


$filter = '(&(|(distinguishedname=cn=it,cn=groups,dc=ad,dc=myuniversity,dc=edu)(distinguishedname=cn=students,cn=groups,dc=ad,dc=myuniversity,dc=edu)))(uniquemember=uid=unkool,ou=lost,dc=ad,dc=myuniversity,dc=edu))';
$servers['ldapauthor1']['search_results'][$filter][$base_dn] = array(
    'count' => 0,
  );

// search filter for non nested groups

$filter = '(&(|(distinguishedname=cn=it,cn=groups,dc=ad,dc=myuniversity,dc=edu)(distinguishedname=cn=students,cn=groups,dc=ad,dc=myuniversity,dc=edu))(uniquemember=uid=joeprojectmanager,ou=it,dc=ad,dc=myuniversity,dc=edu))';
$servers['ldapauthor1']['search_results'][$filter][$base_dn] = array(
    'count' => 1,
    0 => $servers['ldapauthor1']['groups']['cn=it,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'],
  );

$filter = '(|(distinguishedname=uid=jdoe,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu))';
$servers['ldapauthor1']['search_results'][$filter][$base_dn] = array(
    'count' => 1,
    0 => $servers['ldapauthor1']['users']['uid=jdoe,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu']['attr'],
  );

$filter = '(&(objectClass=groupOfUniqueNames)(|(distinguishedname=uid=jdoe,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu)))';
$servers['ldapauthor1']['search_results'][$filter][$base_dn] = array(
    'count' => 1,
    0 => $servers['ldapauthor1']['users']['uid=jdoe,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu']['attr'],
  );


// search filter for non nested groups
$filter = '(&(|(distinguishedname=cn=it,cn=groups,dc=ad,dc=myuniversity,dc=edu)(distinguishedname=cn=students,cn=groups,dc=ad,dc=myuniversity,dc=edu))(uniquemember=uid=jdoe,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu))';
$servers['ldapauthor1']['search_results'][$filter][$base_dn] = array(
    'count' => 1,
    0 => $servers['ldapauthor1']['groups']['cn=students,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'] ,
  );

$filter = "(&(objectClass=groupOfUniqueNames)(|(distinguishedname=cn=developers,cn=groups,dc=ad,dc=myuniversity,dc=edu)(distinguishedname=cn=sysadmins,cn=groups,dc=ad,dc=myuniversity,dc=edu)(distinguishedname=uid=joeprojectmanager,ou=it,dc=ad,dc=myuniversity,dc=edu)))";
$servers['ldapauthor1']['search_results'][$filter][$base_dn] = array(
    'count' => 2,
    0 => $servers['ldapauthor1']['groups']['cn=developers,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'],
    1 => $servers['ldapauthor1']['groups']['cn=sysadmins,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'],
    2 => $servers['ldapauthor1']['users']['uid=joeprojectmanager,ou=it,dc=ad,dc=myuniversity,dc=edu']['attr'],
  );

$filter = '(|(distinguishedname=cn=it,cn=groups,dc=ad,dc=myuniversity,dc=edu)(distinguishedname=cn=people,cn=groups,dc=ad,dc=myuniversity,dc=edu))';
$servers['ldapauthor1']['search_results'][$filter][$base_dn] = array(
    'count' => 2,
    0 => $servers['ldapauthor1']['groups']['cn=it,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'],
    1 => $servers['ldapauthor1']['groups']['cn=people,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'],
  );

$filter = '(|(distinguishedname=cn=students,cn=groups,dc=ad,dc=myuniversity,dc=edu)(distinguishedname=cn=staff,cn=groups,dc=ad,dc=myuniversity,dc=edu))';
$servers['ldapauthor1']['search_results'][$filter][$base_dn] = array(
    'count' => 2,
    0 => $servers['ldapauthor1']['groups']['cn=students,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'],
    1 => $servers['ldapauthor1']['groups']['cn=staff,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'],
  );


$filter = '(|(distinguishedname=cn=it,cn=groups,dc=ad,dc=myuniversity,dc=edu)(distinguishedname=cn=students,cn=groups,dc=ad,dc=myuniversity,dc=edu))';
$servers['ldapauthor1']['search_results'][$filter][$base_dn] = array(
    'count' => 2,
    0 => $servers['ldapauthor1']['groups']['cn=it,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'],
    1 => $servers['ldapauthor1']['groups']['cn=students,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'],
  );

$filter = '(&(|(distinguishedname=cn=it,cn=groups,dc=ad,dc=myuniversity,dc=edu)(cn=people,cn=groups,dc=ad,dc=myuniversity,dc=edu)))';
$servers['ldapauthor1']['search_results'][$filter][$base_dn] = array(
    'count' => 2,
    0 => $servers['ldapauthor1']['groups']['cn=people,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'],
    1 => $servers['ldapauthor1']['groups']['cn=it,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'],
  );

$servers['ldapauthor1']['search_results']['(|((dn=cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu)(dn=cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu))']['ou=groups,dc=ad,dc=myuniversity,dc=edu'] = array(
    'count' => 1,
    0 => array('count' => 2,
    0 => $servers['ldapauthor1']['groups']['cn=sysadmins,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'],
    1 => $servers['ldapauthor1']['groups']['cn=staff,cn=groups,dc=ad,dc=myuniversity,dc=edu']['attr'],
    ),

  );
