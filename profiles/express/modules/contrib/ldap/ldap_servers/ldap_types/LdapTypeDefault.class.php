<?php

/**
 * @file
 * Generic LDAP Implementation Details
 *
 */

module_load_include('php', 'ldap_servers', 'ldap_types/LdapTypeAbstract.class');

class LdapTypeDefault extends LdapTypeAbstract {

  public $name = 'Default LDAP';
  public $typeId = 'Default';
  public $description = 'Other LDAP Type';
  public $port = 389;
  public $tls = 1;
  public $encrypted = 0;
  public $user_attr = 'cn';
  public $mail_attr = 'mail';
  public $supportsNestGroups = FALSE;

 /**
   * Constructor Method
   *
   */
  function __construct($params = array()) {
    foreach ($params as $k => $v) {
      if (property_exists($this, $k)) {
        $this->{$k} = $v;
      }
    }
  }

}
