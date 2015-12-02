<?php
// $Id$


/**
 * @file
 *  LdapTestCase.class.php
 *
 * stub for test class for any ldap module.  has functionality of fake ldap server
 * with user configurations.  Intended to help unifying test environment
 *
 */

class LdapTestFunctions  {

  function prepTestConfiguration($test_data, $feetures = FALSE) {
    $this->prepTestServers($test_data['servers'], $feetures);

    if (!$feetures && isset($test_data['authentication'])) {
      $this->configureAuthentication($test_data['authentication']);
    }

    if (!$feetures && isset($test_data['authorization'])) {
      $this->prepConsumerConf($test_data['authorization']);
    }

    if (!$feetures && isset($test_data['variables'])) {
      foreach ($test_data['variables'] as $name => $value) {
        variable_set($name, $value);
      }
    }

  //$consumer_conf_admin = ldap_authorization_get_consumer_admin_object('drupal_role');
  //$consumer_conf_admin->status = 1;
  //$consumer_conf_admin->save();

  }

  function prepTestServers($servers, $feetures = FALSE, $feature_name = NULL) {
    $current_sids = array();
    foreach ($servers as $sid => $server_data) {
      $current_sids[$sid] = $sid;
      variable_set('ldap_test_server__' . $sid, $server_data);
    }
    variable_set('ldap_test_servers', $current_sids);
  }

  function setFakeServerProperty($sid, $prop, $value) {
    $test_data = variable_get('ldap_test_server__' . $sid, array());
    $test_data['properties'][$prop] = $value;
    variable_set('ldap_test_server__' . $sid, $test_data);
  }


  function configureAuthentication($options) {
    require_once(drupal_get_path('module', 'ldap_authentication') . '/LdapAuthenticationConfAdmin.class.php');

    $ldapServerAdmin = new LdapAuthenticationConfAdmin();

    foreach ($ldapServerAdmin->saveable as $prop_name) {
      if (isset( $options[$prop_name])) {
        $ldapServerAdmin->{$prop_name} = $options[$prop_name];
      }
    }
    $ldapServerAdmin->save();
  }


  function prepConsumerConf($consumer_confs) {
    // create consumer authorization configuration.
    foreach ($consumer_confs as $consumer_type => $consumer_conf) {
      $consumer_obj = ldap_authorization_get_consumer_object($consumer_type);
      $consumer_conf_admin = new LdapAuthorizationConsumerConfAdmin($consumer_obj, TRUE);
      foreach ($consumer_conf as $property_name => $property_value) {
        $consumer_conf_admin->{$property_name} = $property_value;
      }
      $consumer_conf_admin->save();
    }
  }



  function ldapUserIsAuthmapped($username) {
    $authmaps = user_get_authmaps($username);
    return ($authmaps && in_array('ldap_authentication', array_keys($authmaps)));
  }

  function drupalLdapUpdateUser($edit = array(), $ldap_authenticated = FALSE, $user) {

    if (count($edit)) {
      $user = user_save($user, $edit);
    }

    if ($ldap_authenticated) {
      user_set_authmaps($user, array('authname_ldap_authentication' => $user->name));
    }

    return $user;
  }

}
