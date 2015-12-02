<?php
// $Id$

/**
 * @file
 * simpltest authorization config
 */

$servers['ldapauthor1']['properties']  = array(
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



$servers['ldapauthor1']['methodResponses']['connect'] = LDAP_SUCCESS;

$servers['ldapauthor1']['search_results']['(member=cn=verykool,ou=special guests,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu)']['ou=groups,dc=ad,dc=myuniversity,dc=edu'] = array(
    0 => array('count' => 1, 'dn' => 'cn=content editors,ou=groups,dc=ad,dc=myuniversity,dc=edu'),
    1 => array('count' => 1, 'dn' => 'cn=content approvers,ou=groups,dc=ad,dc=myuniversity,dc=edu'),
    'count' => 2,
  );

$servers['ldapauthor1']['search_results']['(member=cn=jkool,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu)']['ou=groups,dc=ad,dc=myuniversity,dc=edu'] = array(
    0 => array('count' => 1, 'dn' => 'cn=content editors,ou=groups,dc=ad,dc=myuniversity,dc=edu'),
    'count' => 1,
  );

/**
 * fake user data array below 'attr' should mimick ldap user result data
 */
$servers['ldapauthor1']['users']['cn=jdoe,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=jdoe,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=jdoe,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu'),
    'mail' => array( 0 => 'jdoe@myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'jdoe', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
  );


$servers['ldapauthor1']['users']['cn=jkool,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=jkool,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=jkool,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu'),
    'mail' => array( 0 => 'jkool@guests.myuniversity.edu', 'count' => 1),
    'mailcode' => array( 0 => '17', 'count' => 1),
    'sAMAccountName' => array( 0 => 'jkool', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
    'memberOf' => array( 0 => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

// duplicate of previous with escaped commas in cn.
$servers['ldapauthor1']['users']['cn=Flintstone\, Wilma,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=Flintstone\, Wilma,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=Flintstone\, Wilma,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu'),
    'mail' => array( 0 => 'wilmaf@guests.myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'wilmaf', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
    'memberOf' => array(
       0 => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu',
       1 => 'cn=punctuated\,comma\,freaks,ou=it,dc=ad,dc=myuniversity,dc=edu',
      'count' => 2
      ),
  );

// duplicate of previous with quoted cn.
$servers['ldapauthor1']['users']['cn=Rubble\, Barney,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=Rubble\, Barney,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=Rubble\, Barney,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu'),
    'mail' => array( 0 => 'barneyr@guests.myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'barneyr', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
    'memberOf' => array(
       0 => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu',
       1 => 'cn="punctuated,comma,freaks",ou=it,dc=ad,dc=myuniversity,dc=edu',
      'count' => 2
      ),
  );


$servers['ldapauthor1']['users']['cn=unkool,ou=lost,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=unkool,ou=lost,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=unkool,ou=lost,dc=ad,dc=myuniversity,dc=edu'),
    'mail' => array( 0 => 'unkool@nowhere.myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'jkool', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
    'memberOf' => array( 0 => 'cn=unknown_people,ou=nowhere,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

$servers['ldapauthor1']['users']['cn=verykool,ou=special guests,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=verykool,ou=special guests,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=verykool,ou=special guests,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu'),
    'mail' => array( 0 => 'verykool@myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'verykool', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
    'meMBErof' => array(
      0 => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu',
      1 => 'CN=NETadmins,ou=it,dc=ad,dc=myuniversity,dc=edu',
      2 => 'cn=phone operators,ou=it,dc=ad,dc=myuniversity,dc=edu',
      'count' => 3,
      ),
  );



$servers['ldapauthor1']['users']['cn=newkool,ou=special guests,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=newkool,ou=special guests,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=newkool,ou=special guests,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu'),
    'mail' => array( 0 => 'newkool@guests.myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'newkool', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
    'memberOf' => array( 0 => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

$servers['ldapauthor1']['users']['cn=joeprogrammer,ou=it,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=joeprogrammer,ou=it,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=joeprogrammer,ou=it,dc=ad,dc=myuniversity,dc=edu'),
    'mail' => array( 0 => 'joeprogrammer@myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'joeprogrammer', 'count' => 1),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
    'memberOf' => array( 0 => 'cn=developers,ou=it,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

/**
 * nested groups for both derive by attr and derive by entry
 *  cn=people,ou=it,dc=ad,dc=myuniversity,dc=edu
 *    cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu
 *      cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu
 *        cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu
 *        cn=developers,ou=it,dc=ad,dc=myuniversity,dc=edu
 *
 */


/**
 *  derive by attr entries deriveFromAttrAttr=memberOf
 */
$servers['ldapauthor1']['groups']['cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu'),
    'memberOf' => array( 0 => 'cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

$servers['ldapauthor1']['groups']['cn=developers,ou=it,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=developers,ou=it,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=developers,ou=it,dc=ad,dc=myuniversity,dc=edu'),
    'memberOf' => array( 0 => 'cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

$servers['ldapauthor1']['groups']['cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu'),
    'memberOf' => array( 0 => 'cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
    'memberOf' => array( 0 => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),  // bogus recursion to test bogus recursion
  );

$servers['ldapauthor1']['groups']['cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu'),
    'memberOf' => array( 0 => 'cn=people,ou=it,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

$servers['ldapauthor1']['groups']['cn=people,ou=it,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=people,ou=it,dc=ad,dc=myuniversity,dc=edu',
    'distinguishedname' => array( 0 => 'cn=people,ou=it,dc=ad,dc=myuniversity,dc=edu'),
    'memberOf' => array('count' => 0),
  );


/**
 *  derive by entry entries
 *    deriveFromEntryMembershipAttr=members
 *    deriveFromEntryAttrMatchingUserAttr=dn
 *    groupObjectClass=group
 */
/**
$servers['ldapauthor1']['groups']['cn=people,ou=it,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=people,ou=it,dc=ad,dc=myuniversity,dc=edu',
    'ObjectClass' => 'group',
    'members' => array( 0 => 'cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

$servers['ldapauthor1']['groups']['cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu',
    'ObjectClass' => 'group',
    'members' => array( 0 => 'cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

$servers['ldapauthor1']['groups']['cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=it,ou=it,dc=ad,dc=myuniversity,dc=edu',
    'ObjectClass' => 'group',
    'members' => array( 0 => 'cn=developers,ou=it,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
    'members' => array( 0 => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),  // bogus recursion to test bogus recursion
  );

$servers['ldapauthor1']['groups']['cn=developers,ou=it,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=developers,ou=it,dc=ad,dc=myuniversity,dc=edu',
    'ObjectClass' => 'group',
    'members' => array( 0 => 'cn=people,ou=it,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

$servers['ldapauthor1']['groups']['cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu',
    'ObjectClass' => 'group',
    'members' => array( 0 => 'cn=newkool,ou=special guests,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu', 'count' => 1),
  );

 **/
// nested group queries
$servers['ldapauthor1']['search_results']['(|((dn=cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu)(dn=cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu))']['ou=groups,dc=ad,dc=myuniversity,dc=edu'] = array(
    0 => array('count' => 2,
          'dn' => 'cn=staff,ou=people,dc=ad,dc=myuniversity,dc=edu',
          'dn' => 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu',
          ),
    'count' => 2,
  );



/**
 * test users should include service account if one is being used
 */
$servers['ldapauthor1']['users']['cn=service-account,dc=ad,dc=myuniversity,dc=edu']['attr'] = array(
    'dn' => 'cn=service-account,dc=ad,dc=myuniversity,dc=edu',
            'distinguishedname' => array( 0 => 'cn=service-account,dc=ad,dc=myuniversity,dc=edu'),
    'mail' => array( 0 => 'service-account@myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'service-account', 'count' => 1),
    'memberOf' => array(
      0 => 'CN=service_accounts,OU=ServiceAccountGroups,DC=ad,DC=myuniversity,DC=edu',
      'count' => 2,
      ),
    'password' => array( 0 => 'goodpwd', 'count' => 1),
  );
