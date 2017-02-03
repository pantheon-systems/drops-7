
This script is intended to help separate LDAP Drupal module configuration and bugs from LDAP server, ldap php extension, and related connectivity and LDAP permissions issues.  It uses the php ldap extension functions like ldap_connect(), ldap_search(), etc. rather than the Drupal LDAP module code.

Watchout for the following:
-- The test script does not depend on the Drupal LDAP module and should not be run within a web server context.  -- Command line PHP with the LDAP Extension enabled are requirements.
-- Often command line PHP will have a different php.ini configuration than the web server's php.ini.  See http://php.net/manual/features.commandline.php

Additional background on prerequisites and debugging Drupal LDAP module are available at:
http://drupal.org/node/1023900
http://drupal.org/node/1141764


--------------------------------
Running the Script
--------------------------------

1.  Copy this directory (ldap_test_script) outside of web root.
2.  Edit config.inc to reflect your server configuration. The array is in the form:
    $config['servers'][<server friendly name>][<server param>] = value
    $config['servers'][<server friendly name>]['test_queries'][<query name>] = array of test query data
    $config['servers'][<server friendly name>]['test_provisions'][<provision name>] = array of test provision data
    
    in provisioning part of array:
      'delete_if_exists' TURE | FALSE indicates if the provisioned object should be deleted if it exists
      'find_filter'      is the filter to find the object. eg.  'cn=jdoe', 'distinguishedname=...'
      'attr'             is the array of attribute/values to provision.  should not include 'dn'

    such as:

    array(
    'servers' => array(
      'default' => array(
        'server_address' => 'ad.mycollege.edu',
        'server_port'  => 389,
        'server_tls'  => FALSE,
        'server_bind_method'  => LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT, 
        'server_base_dn' => 'ou=people,dc=ad,dc=mycollege,dc=edu',
        'server_bind_dn' => 'cn=ldap-service-account,ou=service accounts,dc=ad,dc=mycollege,dc=edu',
        'server_bind_pw' => 'password_here',
        'test_queries' => array(
          'user' => array(
            'filter'  => 'cn=jbarclay',
            'show_attr' => array('dn','cn','displayname','sn','givenname','mail','samaccountname','email'),
          ),
        ),
        'test_provisions' => array(
          'simple_user' => array(
            'dn' =>  "cn=ed-drupal-user-17,ou=people,dc=ad,dc=mycollege,dc=edu",
            'delete_if_exists' => TRUE,
            'find_filter' => "distinguishedName=cn=ed-drupal-user-17,ou=people,dc=ad,dc=mycollege,dc=edu",
            'attr' => array(
              "displayName" => "Drupal User",
              "cn" => 'ed-drupal-user-17',
              "samaccountname" => 'ed-drupal-user-17',
              "objectclass" => array(
                "top", "person", "organizationalPerson", "user",
              ),
              "description" => "test user",
              'mail' => 'ed-drupal-user-17@ad.mycollege.edu',
              'givenName' => 'Drupal',
              'sn' => 'User',
              'distinguishedName' =>  "cn=ed-drupal-user-17,ou=people,dc=ad,dc=mycollege,dc=edu",
            ),
          ),
          'simple_group' => array(
            'dn' =>  "cn=ed-drupal-group2,ou=groups,dc=ad,dc=mycollege,dc=edu",
            'delete_if_exists' => TRUE,
            'find_filter' => "distinguishedName=cn=ed-drupal-group2,ou=groups,dc=ad,dc=mycollege,dc=edu",
            'attr' => array(
              "cn" => 'ed-drupal-group2',
              "sAMAccountName" => 'ed-drupal-group2',
              'instanceType' =>  '4',
              "objectClass" => array(
                "top", "group",
              ),
              'name' => 'ed-drupal-group2',
              'objectCategory' =>  'CN=Group,CN=Schema,CN=Configuration,dc=mycollege,dc=edu',
              'distinguishedName' =>  "cn=ed-drupal-group2,ou=groups,dc=ad,dc=mycollege,dc=edu",
            ),
          ),
        ),
      );
      
3.  Comment out the die() statement near the top of config.php
    That is:
      die('Move this..
    Becomes:
      // die('Move this...
      
4. From the ldap_test_script, type:
   php test.php


