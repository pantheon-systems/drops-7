<?php

/**
 * @file
 *
 */

/*** activedirectory is default Active Directory server config **/
function ldap_test_ldap_servers_data() {

$servers['ldapauthor1']['properties']  = array(
    'sid' => 'openldap1',
    'name'  => 'Test LDAP Server LDAP Authorization' ,
    'inDatabase' => TRUE,
    'status'  => 1,
    'ldap_type'  => 'openldap',
    'address'  => 'ldap.hogwarts.edu',
    'port'  => 389,
    'tls'  => FALSE,
    'bind_method' => LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT,
    'basedn'  => array(
       'dc=hogwarts,dc=edu',
      ),
    'binddn'  => 'cn=service-account,ou=people,dc=hogwarts,dc=edu',
    'bindpw' => 'goodpwd',
    'user_dn_expression' => NULL,
    'user_attr'  => 'cn',
    'mail_attr'  => 'mail',
    'mail_template'  => NULL,
    'unique_persistent_attr' => 'guid',
    'ldap_to_drupal_user' => FALSE,
    'ldapToDrupalUserPhp'  => NULL,
    'groupObjectClass' => 'groupofnames',
    'groupUserMembershipsAttrExists' => FALSE,
    'groupUserMembershipsAttr' => NULL,
    'groupMembershipsAttr' => 'member',
    'groupMembershipsAttrMatchingUserAttr' => 'dn',
    'search_pagination' => 0,
    'searchPageSize' => NULL,
  );



$conf['openldap1']['properties']  = array(
    'sid' => 'openldap1',
    'name'  => 'Test Open LDAP' ,
    'inDatabase' => TRUE,
    'status'  => 1,
    'ldap_type'  => 'openldap',
    'address'  => 'ldap.hogwarts.edu',
    'port'  => 389,
    'tls'  => FALSE,
    'bind_method' => LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT,
    'basedn'  => array(
       'dc=hogwarts,dc=edu',
    ),
    'binddn'  => 'cn=service-account,ou=people,dc=hogwarts,dc=edu',
    'bindpw' => 'goodpwd',
    'user_dn_expression' => NULL,
    'user_attr'  => 'cn',
    'mail_attr'  => 'mail',
    'mail_template'  => NULL,
    'unique_persistent_attr' => 'guid',
    'unique_persistent_attr_binary' => FALSE,
    'ldap_to_drupal_user' => FALSE,
    'ldapToDrupalUserPhp'  => NULL,
    'groupObjectClass' => 'groupofnames',
    'groupUserMembershipsAttrExists' => FALSE,
    'groupUserMembershipsAttr' => NULL,
    'groupMembershipsAttr' => 'member',
    'groupMembershipsAttrMatchingUserAttr' => 'dn',
    'search_pagination' => 0,
    'searchPageSize' => NULL,
  );

$conf['openldap1']['methodResponses']['connect'] = LDAP_SUCCESS;


$conf['openldap1']['search_results'] = array();


$conf['openldap1']['search_results']['(&(objectClass=group)(|(member=cn=gryffindor,ou=groups,dc=hogwarts,dc=edu)(member=cn=students,ou=groups,dc=hogwarts,dc=edu)(member=cn=honors students,ou=groups,dc=hogwarts,dc=edu)))']['dc=hogwarts,dc=edu'] = array(
  0 => array('count' => 1, 'dn' => 'cn=users,ou=groups,dc=hogwarts,dc=edu'),
  'count' => 1,
);

$conf['openldap1']['search_results']['(cn=hpotter)']['dc=hogwarts,dc=edu'] = array(
  0 => array(
    'count' => 1,
    'dn' => 'cn=hpotter,ou=people,dc=hogwarts,dc=edu',
    'FULLENTRY' => TRUE,
  ),
  'count' => 1,
);

$conf['openldap1']['search_results']['(cn=hpotter)']['dc=hogwarts,dc=edu'] = array(
  0 => array(
    'count' => 1,
    'dn' => 'cn=hpotter,ou=people,dc=hogwarts,dc=edu',
    'FULLENTRY' => TRUE,
  ),
  'count' => 1,
);

$conf['openldap1']['search_results']['(cn=hpotter-granger)']['dc=hogwarts,dc=edu'] = array(
  0 => array(
    'count' => 1,
    'dn' => 'cn=hpotter,ou=people,dc=hogwarts,dc=edu',
    'FULLENTRY' => TRUE,
  ),
  'count' => 1,
);

$conf['openldap1']['search_results']['(cn=ssnape)']['dc=hogwarts,dc=edu'] = array(
  0 => array(
    'count' => 1,
    'dn' => 'cn=ssnape,ou=people,dc=hogwarts,dc=edu',
    'FULLENTRY' => TRUE,
  ),
  'count' => 1,
);

$conf['openldap1']['search_results']['(cn=adumbledore)']['dc=hogwarts,dc=edu'] = array(
  0 => array(
    'count' => 1,
    'dn' => 'cn=adumbledore,ou=people,dc=hogwarts,dc=edu',
    'FULLENTRY' => TRUE,
  ),
  'count' => 1,
);

$conf['openldap1']['search_results']['(&(objectClass=groupofnames)(member=cn=hpotter,ou=people,dc=hogwarts,dc=edu))']['dc=hogwarts,dc=edu'] = array(
  0 => array('count' => 1, 'dn' => 'cn=gryffindor,ou=groups,dc=hogwarts,dc=edu'),
  1 => array('count' => 1, 'dn' => 'cn=students,ou=groups,dc=hogwarts,dc=edu'),
  2 => array('count' => 1, 'dn' => 'cn=honors students,ou=groups,dc=hogwarts,dc=edu'),
  'count' => 3,
);

$conf['openldap1']['search_results']['(&(objectClass=groupofnames)(|(member=cn=gryffindor,ou=groups,dc=hogwarts,dc=edu)(member=cn=students,ou=groups,dc=hogwarts,dc=edu)(member=cn=honors students,ou=groups,dc=hogwarts,dc=edu)))']['dc=hogwarts,dc=edu'] = array(
  0 => array('count' => 1, 'dn' => 'cn=users,ou=groups,dc=hogwarts,dc=edu'),
  'count' => 1,
);



$conf['activedirectory1']['properties']  = array(
    'sid' => 'activedirectory1',
    'name'  => 'Test Active Directory LDAP' ,
    'inDatabase' => TRUE,
    'status'  => 1,
    'ldap_type'  => 'activedirectory',
    'address'  => 'ldap.hogwarts.edu',
    'port'  => 389,
    'tls'  => FALSE,
    'bind_method' => LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT,
    'basedn'  => array(
       'dc=hogwarts,dc=edu',
    ),
    'binddn'  => 'cn=service-account,ou=people,dc=hogwarts,dc=edu',
    'bindpw' => 'goodpwd',
    'user_dn_expression' => NULL,
    'user_attr'  => 'samaccountname',
    'mail_attr'  => 'mail',
    'mail_template'  => NULL,
    'unique_persistent_attr' => 'guid',
    'unique_persistent_attr_binary' => FALSE,
    'ldap_to_drupal_user' => FALSE,
    'ldapToDrupalUserPhp'  => NULL,
    'groupNested' => 0,
    'groupObjectClass' => 'group',
    'groupUserMembershipsAttrExists' => TRUE,
    'groupUserMembershipsAttr' => 'memberof',
    'groupMembershipsAttr' => 'member',
    'groupMembershipsAttrMatchingUserAttr' => 'dn',
    'search_pagination' => 0,
    'searchPageSize' => NULL,
  );

$conf['activedirectory1']['methodResponses']['connect'] = LDAP_SUCCESS;
$conf['activedirectory1']['clones'] = LDAP_TEST_USER_ORPHAN_CLONE_COUNT;

$conf['activedirectory1']['search_results'] = array();

$conf['activedirectory1']['search_results']['(&(objectClass=group)(member=cn=hpotter,ou=people,dc=hogwarts,dc=edu))']['dc=hogwarts,dc=edu'] = array(
  0 => array('count' => 1, 'dn' => 'cn=gryffindor,ou=groups,dc=hogwarts,dc=edu'),
  1 => array('count' => 1, 'dn' => 'cn=students,ou=groups,dc=hogwarts,dc=edu'),
  2 => array('count' => 1, 'dn' => 'cn=honors students,ou=groups,dc=hogwarts,dc=edu'),
  'count' => 3,
);

$conf['activedirectory1']['search_results']['(&(objectClass=group)(|(member=cn=gryffindor,ou=groups,dc=hogwarts,dc=edu)(member=cn=students,ou=groups,dc=hogwarts,dc=edu)(member=cn=honors students,ou=groups,dc=hogwarts,dc=edu)))']['dc=hogwarts,dc=edu'] = array(
  0 => array('count' => 1, 'dn' => 'cn=users,ou=groups,dc=hogwarts,dc=edu'),
  'count' => 1,
);

foreach (array('hpotter', 'hgrainger', 'ssnape', 'adumbledore') as $cn) {

  $conf['activedirectory1']['search_results']["(cn=$cn)"]['dc=hogwarts,dc=edu'] = array(
    0 => array(
      'count' => 1,
      'dn' => "cn=$cn,ou=people,dc=hogwarts,dc=edu",
      'FULLENTRY' => TRUE,
    ),
    'count' => 1,
  );

  $conf['activedirectory1']['search_results']["(samaccountname=$cn)"]['dc=hogwarts,dc=edu'] = array(
    0 => array(
      'count' => 1,
      'dn' => "cn=$cn,ou=people,dc=hogwarts,dc=edu",
      'FULLENTRY' => TRUE,
    ),
    'count' => 1,
  );

}


$conf['activedirectory1']['search_results']['(samaccountname=hpotter-granger)']['dc=hogwarts,dc=edu'] = array(
  0 => array(
    'count' => 1,
    'dn' => 'cn=hpotter,ou=people,dc=hogwarts,dc=edu',
    'FULLENTRY' => TRUE,
  ),
  'count' => 1,
);



return $conf;
}
