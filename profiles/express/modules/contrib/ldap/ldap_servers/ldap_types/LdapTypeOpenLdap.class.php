<?php

/**
 * @file
 * OpenLDAP LDAP Implementation Details
 *
 * see: http://www.zytrax.com/books/ldap/
 *      http://www.openldap.org/doc/
 */

require_once(drupal_get_path('module', 'ldap_servers') . '/ldap_types/LdapTypeAbstract.class.php');

class LdapTypeOpenLdap extends LdapTypeAbstract {

  // generic properties
  public $documentation = '';
  public $name = 'openLDAP LDAP';
  public $typeId = 'OpenLdap';
  public $description = 'openLDAP LDAP';

  // ldap_servers configuration
  public $tls = 0;
  public $encrypted = 0;
  public $user_attr = 'cn';
  public $mail_attr = 'mail';
  public $unique_persistent_attr = 'entryUUID';
  public $unique_persistent_attr_binary = FALSE;
  public $groupObjectClassDefault = 'groupofnames';

  // ldap_authorization configuration
  public $groupDerivationModelDefault = LDAP_SERVERS_DERIVE_GROUP_FROM_ENTRY;
  public $groupMembershipsAttr = 'member';
  public $groupMembershipsAttrMatchingUserAttr = 'dn';


}
