<?php

/**
 * @file
 * Active Directory LDAP Implementation Details
 *
 */

require_once(drupal_get_path('module', 'ldap_servers') . '/ldap_types/LdapTypeAbstract.class.php');

class LdapTypeOpenLdap extends LdapTypeAbstract {

  public $documentation = '';
  public $name = 'openLDAP LDAP';
  public $typeId = 'OpenLdap';
  public $description = 'openLDAP LDAP';
  public $port = 389;
  public $tls = 1;
  public $encrypted = 0;
  public $user_attr = 'cn';
  public $mail_attr = 'mail';

  public $groupObjectClassDefault = NULL;

  public $groupDerivationModelDefault = LDAP_SERVERS_DERIVE_GROUP_FROM_ENTRY;

  public $groupDeriveFromEntryAttrDefault = 'members';
  public $groupDeriveFromEntryUserIdDefault = 'dn';

}
