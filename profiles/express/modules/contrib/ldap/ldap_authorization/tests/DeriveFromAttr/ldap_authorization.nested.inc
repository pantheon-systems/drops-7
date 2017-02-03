<?php
// $Id$

/**
 * @file
 * simpletest authorization configuration
 */


$authorization['drupal_role'] = array(

  'consumerType' => 'drupal_role',  // change as desired
  'consumerModule' => 'ldap_authorization_drupal_role',

  'description' => 'UIUC AD',
  'status' =>  1,
  'onlyApplyToLdapAuthenticated' => 1,

  'deriveFromDn' => 0,
  'deriveFromDnAttr' => NULL,

  'deriveFromAttr' => 1,
  'deriveFromAttrAttr' => array('memberOf'),
  'deriveFromAttrUseFirstAttr' => 0,
  'deriveFromAttrNested' => 1,

  'deriveFromEntry' => 0,
  'deriveFromEntryEntries' => NULL,
  'deriveFromEntryEntriesAttr' => NULL,
  'deriveFromEntryMembershipAttr' => NULL,
  'deriveFromEntryNested' => 0,
  'deriveFromEntryUseFirstAttr' => 0,
  'deriveFromEntryAttrMatchingUserAttr' => 'dn',

  'mappings' => array(),

  'useMappingsAsFilter' => 0,

  'synchOnLogon' => 1,
  'synchManually' => 1,

  'revokeLdapProvisioned' => 1,
  'createConsumers' => 1,
  'regrantLdapProvisioned' => 1,
);
