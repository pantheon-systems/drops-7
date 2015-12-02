<?php
// $Id$
/**
 * @file
 * ldap_authorization test data
 */

$authorization['drupal_role'] = array(

  'consumerModule' => 'ldap_authorization_drupal_role',
  'description' => 'UIUC AD',
  'status' => 1,
  'onlyApplyToLdapAuthenticated' => 1,

  'deriveFromDn' =>  0,
  'deriveFromDnAttr' =>  NULL,

  'deriveFromAttr' =>  1,
  'deriveFromAttrAttr' =>  array('memberOf'),

  'deriveFromEntry' =>  0,
  'deriveFromEntryEntries' =>  NULL,
  'deriveFromEntryEntriesAttr' => NULL,
  'deriveFromEntryMembershipAttr' =>  NULL,

  'mappings' => array(
    0 => array('cn=netadmins,ou=it,dc=ad,dc=myuniversity,dc=edu', 'netadmins'),
  ),

  'useMappingsAsFilter'  =>  1,

  'synchOnLogon'  =>  1,
  'synchManually' => 0,

  'revokeLdapProvisioned' => 1,
  'createConsumers' => 1,
  'regrantLdapProvisioned' => 1,
);
