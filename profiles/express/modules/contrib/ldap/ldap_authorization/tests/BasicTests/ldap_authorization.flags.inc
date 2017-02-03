<?php
// $Id$

/**
 * @file
 * simpletest authorization configuration
 */

$authorization['drupal_role'] = array(
  'consumerModule' => 'ldap_authorization_drupal_role',

  'description' => 'UIUC AD',
  'status' => 1,
  'onlyApplyToLdapAuthenticated' => 1,

  'deriveFromDn' => 1,
  'deriveFromDnAttr' => 'ou',

  'deriveFromAttr' => 0,
  'deriveFromAttrAttr' => array(),
  'deriveFromAttrNested' => 0,

  'deriveFromEntry' => 0,
  'deriveFromEntryEntries' => array(),
  'deriveFromEntryEntriesAttr' => NULL,
  'deriveFromEntryMembershipAttr' => NULL,
  'deriveFromEntryNested' => 0,

  'mappings' => array(
    array('Campus Accounts', 'campus accounts'),
    array('guest accounts', 'guests'),
    array('special guests', 'special guests'),
    array('cn=sysadmins,ou=it,dc=ad,dc=myuniveristy,dc=edu', 'sysadmins'),
  ),

  'useMappingsAsFilter' => 1,

  'synchOnLogon' => 1,
  'synchManually' => 1,

  'revokeLdapProvisioned' => 1,
  'createConsumers' => 1,
  'regrantLdapProvisioned' => 1,
);
