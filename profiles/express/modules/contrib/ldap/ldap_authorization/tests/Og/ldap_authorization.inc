<?php

/**
 * @file
 * simpletest authorization configuration
 */

/**
 *
 *    mapping notes

  jkool:  guest accounts, cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu
  should yield: 'gid=3,rid=2', 'gid=3,rid=3'

  verykool: 'cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu', special guests, guest accounts
  should yield: 'gid=3,rid=3', 'gid=3,rid=2', group-name=knitters,role-name=member

**/

$authorization['og_group'] = array(
  'consumerModule' => 'ldap_authorization_drupal_role',

  'description' => 'UIUC AD',
  'status' => 1,
  'onlyApplyToLdapAuthenticated' => 1,

  'deriveFromDn' =>  1,
  'deriveFromDnAttr' =>  'ou',

  'deriveFromAttr' => 1,
  'deriveFromAttrAttr' =>  array('memberOf', 'mailcode'),
  'deriveFromAttrUseFirstAttr' => 0,

  'deriveFromEntry' => 0,
  'deriveFromEntryEntries' => array(),
    'deriveFromEntryEntriesAttr' => NULL,
  'deriveFromEntryMembershipAttr' => array(),

  'mappings' => array(
    0 => array('cn=content editors,ou=groups,dc=ad,dc=myuniversity,dc=edu',
               'group-name=knitters,role-name=member',
                ),
    1 => array('cn=content approvers,ou=groups,dc=ad,dc=myuniversity,dc=edu',
               'gid=3,rid=2'
               ),
    2 => array('guest accounts',
               'gid=3,rid=2'
               ),
    3 => array('cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu',
               'group-name=knitters,role-name=prince'
               ),
    4 => array('special guests',
               'group-name=knitters,role-name=member'
               ),
    5 => array('cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu',
               'group-name=knitters,role-name=administrator member'
               ),
    6 => array('cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu',
               'gid=3,rid=3'
               ),
   ),

  'useMappingsAsFilter' => 1,

  'synchOnLogon' => 1,
  'synchManually' => 1,

  'revokeLdapProvisioned' => 1,
  'createConsumers' => 0,
  'regrantLdapProvisioned' => 1,
);

  /**
   *      5 => array('cn=content approvers,ou=groups,dc=ad,dc=myuniversity,dc=edu',
               'node.field_state_id=IL,rid=2'
               ),
               **/

$og_groups = array(
  'knitters' => array('entity_type' => 'node', 'label' => 'knitters'),
  'bakers'   => array('entity_type' => 'node', 'label' => 'bakers'),
  'butchers'   => array('entity_type' => 'node', 'label' => 'butchers'),
 );

$og_roles = array(
  'prince' => array(),
  'pauper' => array(),
  );
