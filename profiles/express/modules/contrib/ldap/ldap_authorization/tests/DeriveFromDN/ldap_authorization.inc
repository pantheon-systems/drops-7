<?php
// $Id$

/**
 * @file
 * simpletest authorization configuration
 */

$authorization['drupal_role'] =  array(
  'consumerModule' =>  'ldap_authorization_drupal_role',

  'description' =>  'UIUC AD',
  'status' =>  1,
  'onlyApplyToLdapAuthenticated' =>  1,

  'deriveFromDn' =>  1,
  'deriveFromDnAttr' =>  'ou',

  'deriveFromAttr' =>  0,
  'deriveFromAttrAttr' =>  array(),
  'deriveFromAttrNested' => 0,

  'deriveFromEntry' =>  0,
  'deriveFromEntryEntries' =>  NULL,
  'deriveFromEntryEntriesAttr' => NULL,
  'deriveFromEntryMembershipAttr' =>  NULL,
  'deriveFromEntryNested' => 0,
  'deriveFromEntryUseFirstAttr' => 0,


  'mappings' =>  array(
    0 =>  array('Campus Accounts', 'campus accounts'),
    1 =>  array('guest accounts', 'guests'),
    2 =>  array('special guests', 'special guests'),
    3 =>  array('cn=sysadmins,ou=it,dc=ad,dc=myuniversity,dc=edu', 'sysadmins'),
  ),

  'useMappingsAsFilter' =>  1,

  'synchOnLogon' =>  1,
  'synchManually' =>  1,

  'revokeLdapProvisioned' =>  1,
  'createConsumers' =>  1,
  'regrantLdapProvisioned' =>  1,
);
