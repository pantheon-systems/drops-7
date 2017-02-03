<?php

/**
 * @file
 * abstract class to represent an ldap implementation type
 * such as active directory, open ldap, novell, etc.
 *
 */

abstract class LdapTypeAbstract {

  public $name;
  public $typeId;
  public $description;
  public $port = 389;
  public $tls = 0;
  public $encrypted = 0;
  public $user_attr = 'cn';
  public $mail_attr = 'mail';

  public $groupObjectClassDefault = NULL;

  public $groupDerivationModelDefault = NULL;
  public $groupDeriveFromAttributeNameDefault = NULL;
  public $groupDeriveFromAttrDnAttrDefault = NULL;

  public $groupDeriveFromEntryAttrDefault = NULL;
  public $groupDeriveFromEntryUserIdDefault = NULL;
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
