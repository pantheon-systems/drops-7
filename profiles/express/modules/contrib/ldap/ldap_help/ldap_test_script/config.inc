<?php

/**
 * @file
 * test script for testing ldap functionality outside of Drupal
 *
 */

die('Move this script outside of Drupal and uncomment the die statement in config.inc to run.'); // remove this line after moving out the scripts outside of drupal root.

function ldap_help_config() {

  // these variables are just for convenience, they can be hard coded directly into the $config array
  $provision_root = 'ou=test,ou=drupal,dc=ad,dc=mycollege,dc=edu';
  $query_root = 'dc=ad,dc=mycollege,dc=edu';
  $user_root = 'ou=people,dc=mycollege,dc=edu';
  $bind_user = 'cn=drupal-ldap,ou=service accounts,dc=ad,dc=mycollege,dc=edu';
  $bind_pwd = 'password';
  $server_address = 'ad.mycollege.edu';

  $config = array(
    'servers' => array(
      'default test' => array(
        'server_address' => $server_address,
        'server_port'  => 389,
        'server_tls'  => FALSE,
        'server_bind_method'  => LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT, // LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT OR LDAP_SERVERS_BIND_METHOD_ANON
        'server_base_dn' => $user_root,
        'server_bind_dn' => $bind_user,
        'server_bind_pw' => $bind_pwd,
        'test_queries' => array(
          'user' => array(
            'filter'  => 'cn=jbarclay',
            'show_attr' => array('dn', 'cn', 'displayname', 'sn', 'givenname', 'mail', 'samaccountname', 'email'),
          ),
        ),
        'test_provisions' => array(),
      ),
      'query test' => array(
        'server_address' => $server_address,
        'server_port'  => 389,
        'server_tls'  => FALSE,
        'server_bind_method'  => LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT, // LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT OR LDAP_SERVERS_BIND_METHOD_ANON
        'server_base_dn' => $query_root,
        'server_bind_dn' => $bind_user,
        'server_bind_pw' => $bind_pwd,
        'test_queries' => array(
          'group' => array(
            'filter'  => 'cn=ed-it-nag',
            'show_attr' => array('dn', 'cn', 'samaccountname'),
          ),
        ),
        'test_provisions' => array(),
      ),
      'provision test' => array(
        'server_address' => $server_address,
        'server_port'  => 389,
        'server_tls'  => FALSE,
        'server_bind_method'  => LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT, // LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT OR LDAP_SERVERS_BIND_METHOD_ANON
        'server_base_dn' => $provision_root,
        'server_bind_dn' => $bind_user,
        'server_bind_pw' => $bind_pwd,
        'test_queries' => array(),
        'test_provisions' => array(
          'simple_user' => array(
            'delete_if_exists' => TRUE,
            'find_filter' => "distinguishedName=cn=ed-drupal-user-17,$provision_root",
            'dn' =>  "cn=ed-drupal-user-17, $provision_root",
            'attr' => array(
              "displayName" => "Drupal User",
              "cn" => 'ed-drupal-user-17',
              "samaccountname" => 'ed-drupal-user-17',
              "objectclass" => array(
                "top", "person", "organizationalPerson", "user",
              ),
              "description" => "test user",
              'mail' => 'ed-drupal-user-17@blah.com',
              'givenName' => 'Drupal',
              'sn' => 'User',
              'distinguishedName' =>  "cn=ed-drupal-user-17,$provision_root",
            ),
          ),
          'simple_group' => array(
            'delete_if_exists' => TRUE,
            'find_filter' => "distinguishedName=cn=ed-drupal-group2,$provision_root",
            'dn' =>  "cn=ed-drupal-group2,$provision_root",
            'attr' => array(
              "cn" => 'ed-drupal-group2',
              "sAMAccountName" => 'ed-drupal-group2',
              'instanceType' =>  '4',
              "objectClass" => array(
                "top", "group",
              ),
              'name' => 'ed-drupal-group2',
              'distinguishedName' =>  "cn=ed-drupal-group2,$provision_root",
            ),
          ),
        ),
      ),
    ),
  );

  return $config;

}
