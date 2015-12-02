<?php
// $Id$

/**
 * @file
 * simpletests for ldap authorization
 *
 */
require_once(drupal_get_path('module', 'ldap_servers') . '/tests/LdapTestFunctions.class.php');
require_once(drupal_get_path('module', 'ldap_authorization') . '/LdapAuthorizationConsumerConfAdmin.class.php');


class LdapAuthorizationTestCase extends DrupalWebTestCase {

  public $module_name = 'ldap_authorization';
  public $testFunctions;

  // storage for test data
  public $useFeatureData;
  public $featurePath;
  public $featureName;

  public $ldapTestId;
  public $serversData;
  public $authorizationData;
  public $authenticationData;
  public $testData = array();

  public $sid; // current, or only, sid
  public $consumerType = 'drupal_role'; // current, or only, consumer type being tested

  function setUp($addl_modules = array()) {
    parent::setUp(array_merge(array('ldap_authentication', 'ldap_authorization', 'ldap_authorization_drupal_role'), $addl_modules));
    variable_set('ldap_simpletest', 1);
    variable_set('ldap_help_watchdog_detail', 0);
  }

  function tearDown() {
    parent::tearDown();
    variable_del('ldap_help_watchdog_detail');
    variable_del('ldap_simpletest');
  }


  function prepTestData() {

    $servers = array();
    $variables = array();
    $authentication = array();
    $authorization = array();
    $this->testFunctions = new LdapTestFunctions();
    if ($this->useFeatureData) {
      module_enable(array('ctools'), TRUE);
      module_enable(array('strongarm'), TRUE);
      module_enable(array('features'), TRUE);
      module_enable(array($this->featureName), TRUE);
       // will need to set non exportables such as bind password also
       // also need to create fake ldap server data.  use

      if (! (module_exists('ctools') && module_exists('strongarm') && module_exists('features') && module_exists('$this->featureName')) ) {
        drupal_set_message(t('Features and Strongarm modules must be available to use Features as configuratio of simpletests'), 'warning');
      }


   // with test data stored in features, need to get server properties from ldap_server properties
      require_once(drupal_get_path('module', $this->featureName) . '/' . $this->featureName . '.ldap_servers.inc');
      require_once(drupal_get_path('module', $this->featureName) . '/fake_ldap_server_data.inc');
      $function_name =  $this->featureName . '_default_ldap_servers';
      $servers = call_user_func($function_name);
      foreach ($servers as $sid => $server) {
        $this->testData['servers'][$sid]['properties'] = (array)$server; // convert to array
        $this->testData['servers'][$sid]['properties']['inDatabase'] = TRUE;
        $this->testData['servers'][$sid]['properties']['bindpw'] = 'goodpwd';
        $this->testData['servers'][$sid] = array_merge($this->testData['servers'][$sid], $fake_ldap_server_data[$sid]);
      }

      // make included fake sid match feature sid
      $this->testFunctions->prepTestConfiguration($this->testData, FALSE);
    }
    else {
      include(drupal_get_path('module', 'ldap_authorization') . '/tests/' . $this->authorizationData);
      $this->testData['authorization'] = $authorization;

      include(drupal_get_path('module', 'ldap_authorization') . '/tests/' . $this->authenticationData);
      $this->testData['authentication'] = $authentication;

      include(drupal_get_path('module', 'ldap_authorization') . '/tests/' . $this->serversData);
      $this->testData['servers'] = $servers;

      $this->testData['variables'] = $variables;

      // if only one server, set as default in authentication and authorization
      if (count($this->testData['servers']) == 1) {
        $sids = array_keys($servers);
        $this->sid = $sids[0];
        foreach ($this->testData['authorization'] as $consumer_type => $consumer_conf) {
          $this->testData['authorization'][$consumer_type]['consumerType'] = $consumer_type;
          $this->testData['authorization'][$consumer_type]['sid'] = $this->sid;
        }
        $this->testData['authentication']['sids'] = array($this->sid => $this->sid);
        $this->testData['servers'][$this->sid]['sid'] = $this->sid;
      }
      $this->testFunctions->prepTestConfiguration($this->testData, FALSE);
    }
  }
}
