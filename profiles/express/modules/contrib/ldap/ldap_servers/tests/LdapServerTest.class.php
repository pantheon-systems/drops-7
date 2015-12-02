<?php
// $Id: LdapServerTest.class.php,v 1.5.2.1 2011/02/08 06:01:00 johnbarclay Exp $

/**
 * @file
 * Simpletest ldapServer class for testing without an actual ldap server
 *
 */

/**
 * LDAP Server Class
 *
 *  This class is used to create, work with, and eventually destroy ldap_server
 * objects.
 *
 * @todo make bindpw protected
 */

require_once(drupal_get_path('module', 'ldap_servers') . '/LdapServer.class.php');

class LdapServerTest extends LdapServer {
  // LDAP Settings

  public $testUsers;
  public $testGroups;
  public $methodResponses;
  public $searchResults;
  public $binddn = FALSE; // Default to an anonymous bind.
  public $bindpw = FALSE; // Default to an anonymous bind.

  /**
   * Constructor Method
   *
   * can take array of form property_name => property_value
   * or $sid, where sid is used to derive the include file.
   */
  function __construct($sid) {
    if (!is_scalar($sid)) {
      $test_data = $sid;
    }
    else {
      $test_data = variable_get('ldap_test_server__' . $sid, array());
    }
    $this->sid = $sid;
    $this->methodResponses = $test_data['methodResponses'];
    $this->testUsers = $test_data['users'];
    $this->testGroups = (is_array($test_data) && isset($test_data['groups'])) ? $test_data['groups'] : array();
    $this->searchResults = (isset($test_data['search_results'])) ? $test_data['search_results'] : array();

    $this->detailedWatchdogLog = variable_get('ldap_help_watchdog_detail', 0);
    foreach ($test_data['properties'] as $property_name => $property_value ) {
      $this->{$property_name} = $property_value;
    }
    if (is_scalar($this->basedn)) {
      $this->basedn = unserialize($this->basedn);
    }
    if (isset($server_record['bindpw']) && $server_record['bindpw'] != '') {
      $this->bindpw = ldap_servers_decrypt($this->bindpw);
    }
  }

  /**
   * Destructor Method
   */
  function __destruct() {
     // if alterations to server configuration must be maintained throughout simpletest, variable_set('ldap_authorization_test_server__'. $sid, array());
  }

  /**
   * Connect Method
   */
  function connect() {
    return $this->methodResponses['connect'];
  }


  function bind($userdn = NULL, $pass = NULL, $anon_bind = FALSE) {
    $userdn = ($userdn != NULL) ? $userdn : $this->binddn;
    $pass = ($pass != NULL) ? $pass : $this->bindpw;

    if (! isset($this->testUsers[$userdn])) {
      $ldap_errno = LDAP_NO_SUCH_OBJECT;
      if (function_exists('ldap_err2str')) {
        $ldap_error = ldap_err2str($ldap_errno);
      }
      else {
        $ldap_error = "Failed to find $userdn in LdapServerTest.class.php";
      }
    }
    elseif (isset($this->testUsers[$userdn]['attr']['password'][0]) && $this->testUsers[$userdn]['attr']['password'][0] != $pass) {
      $ldap_errno = LDAP_INVALID_CREDENTIALS;
      if (function_exists('ldap_err2str')) {
        $ldap_error = ldap_err2str($ldap_errno);
      }
      else {
        $ldap_error = "Credentials for $userdn failed in LdapServerTest.class.php";
      }
    }
    else {
      return LDAP_SUCCESS;
    }

    debug(t("LDAP bind failure for user %user. Error %errno: %error",
      array('%user' => $userdn,
            '%errno' => $ldap_errno,
            '%error' => $ldap_error,
      )));

    return $ldap_errno;

  }

  /**
   * Disconnect (unbind) from an active LDAP server.
   */
  function disconnect() {

  }

  /**
   * Perform an LDAP search.
   * @param string $basedn
   *   The search base. If NULL, we use $this->basedn. should not be esacaped
   *
   * @param string $filter
   *   The search filter. such as sAMAccountName=jbarclay.  attribute values (e.g. jbarclay) should be esacaped before calling

   * @param array $attributes
   *   List of desired attributes. If omitted, we only return "dn".
   *
   * @remaining params mimick ldap_search() function params
   *
   * @return
   *   An array of matching entries->attributes, or FALSE if the search is empty.
   */
  function search($base_dn = NULL, $filter, $attributes = array(), $attrsonly = 0, $sizelimit = 0, $timelimit = 0, $deref = LDAP_DEREF_NEVER, $scope = LDAP_SCOPE_SUBTREE) {
    //debug("feaux_server_filter=$filter");
    $filter = trim(str_replace(array("\n", "  "),array('',''), $filter)); // for test matching simplicity remove line breaks and tab spacing
   // debug("filter=$filter");
   // debug('search');  debug("base_dn: $base_dn"); debug("filter:<pre>$filter</pre>");
    $my_debug = ($filter == "(samaccountname=verykool)" || $filter == "(sAMAccountName=verykool)");
  //  if ($my_debug) {debug('attributes'); debug($attributes); debug('search');  debug("base_dn: $base_dn"); debug("filter:<pre>$filter</pre>");}

    if ($base_dn == NULL) {
      if (count($this->basedn) == 1) {
        $base_dn = $this->basedn[0];
      }
      else {
        return FALSE;
      }
    }

    // return prepolulated search results in test data array if present
    if (isset($this->searchResults[$filter][$base_dn])) {
    //  if ($my_debug) {debug('set search results'); debug($this->searchResults[$filter][$base_dn]);}
      return $this->searchResults[$filter][$base_dn];
    }
    $base_dn = drupal_strtolower($base_dn);
    $filter = strtolower(trim($filter,"()"));

    list($filter_attribute, $filter_value) = explode('=', $filter);
    if (strtolower($filter_attribute) == 'dn') { // don't allow filtering on dn.  filters should use distinguishedName
      continue;
    }
  //  $filter_value = ldap_pear_unescape_filter_value($filter_value);
    // need to perform feaux ldap search here with data in
    $results = array();
   // debug('test users'); debug($this->testUsers); debug("filter_attribute=$filter_attribute, filter_value=$filter_value");
    foreach ($this->testUsers as $dn => $user_data) {
      $my_debug2 = ($my_debug && $dn == 'cn=Flintstone\\, Wilma,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu');
     // if ($my_debug2) {debug('test user ' . $dn); debug($user_data);}
      $user_data_lcase = array();
      foreach ($user_data['attr'] as $attr => $values) {
        $user_data_lcase['attr'][drupal_strtolower($attr)] = $values;
      }

      
      // if not in basedn, skip
      // eg. basedn ou=campus accounts,dc=ad,dc=myuniversity,dc=edu
      // should be leftmost string in:
      // cn=jdoe,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu
      $dn = strtolower($dn);
      $pos = stripos($dn, $base_dn);
      if ($pos === FALSE || strcasecmp($base_dn, substr($dn, 0, $pos + 1)) == FALSE) {
        if ($my_debug2) {debug("dn=$dn not in base_dn=$base_dn");}
        continue; // not in basedn
      }

      // check for mixed case and lowercase attribute's existance
      // trying to mimic ldap implementation
      if (isset($user_data['attr'][$filter_attribute])) {
        $contained_values = $user_data['attr'][$filter_attribute];
        // if ($my_debug2) { debug('mixed case match success');}
      }
      elseif (isset($user_data_lcase['attr'][ldap_server_massage_text($filter_attribute, 'attr_name', LDAP_SERVER_MASSAGE_QUERY_ARRAY)])) {
        $contained_values = $user_data_lcase['attr'][ldap_server_massage_text($filter_attribute, 'attr_name', LDAP_SERVER_MASSAGE_QUERY_ARRAY)];
       //  if ($my_debug2) { debug('lower case match success');}
      }
      else {
       // if ($my_debug2) {debug('match fail');}
        continue;
      }
      //debug("contained_values"); debug($contained_values);
      unset($contained_values['count']);
      if (!in_array($filter_value, array_values($contained_values))) {
     //   if ($my_debug2) { debug("match to value $filter_attribute=$filter_value failed");}
        continue;
      }



      // loop through all attributes, if any don't match continue
      $user_data_lcase['attr']['distinguishedname'] = $dn;
      if ($attributes) {
        $selected_user_data = array();
        foreach ($attributes as $i => $attr_name) {
          $key = drupal_strtolower($attr_name);
          $selected_user_data[$key] = (isset($user_data_lcase['attr'][$key])) ? $user_data_lcase['attr'][$key] : NULL;
        }
        $results[] = $selected_user_data;
      }
      elseif (isset($user_data_lcase['attr'])) {
        $results[] = $user_data_lcase['attr'];
      }
    }
//    if ($my_debug) { debug("results post user loop"); debug($results);}
    foreach ($this->testGroups as $dn => $group_data) {
      // debug("group dn $dn"); debug($group_data);
      // if not in basedn, skip
      // eg. basedn ou=campus accounts,dc=ad,dc=myuniversity,dc=edu
      // should be leftmost string in:
      // cn=jdoe,ou=campus accounts,dc=ad,dc=myuniversity,dc=edu
      $pos = strpos($dn, $base_dn);
      if ($pos === FALSE || strcasecmp($base_dn, substr($dn, 0, $pos + 1)) == FALSE) {
        continue; // not in basedn
      }
      else {
      }

      // if doesn't filter attribute has no data, continue
      if (!isset($group_data['attr'][$filter_attribute])) {
        continue;
      }

      // if doesn't match filter, continue
      $contained_values = $group_data['attr'][$filter_attribute];
      unset($contained_values['count']);
      if (!in_array($filter_value, array_values($contained_values))) {
        continue;
      }

      // loop through all attributes, if any don't match continue
     // $group_data['attr']['distinguishedname'] = $dn;
     // $group_data['distinguishedname'] = $dn;

      if ($attributes) {
        $selected_group_data = array();
        foreach ($attributes as $key => $value) {
          $selected_group_data[$key] = (isset($group_data['attr'][$key])) ? $group_data['attr'][$key] : NULL;
        }
        $results[] = $selected_group_data;
      }
      else {
        $results[] = $group_data['attr'];
      }
     // debug($results);
    }

    $results['count'] = count($results);
    $results = ($results['count'] > 0) ? $results : FALSE;
   // if ($my_debug) {  debug('search-results 2'); debug($results);}
    return $results;
  }


  public static function getLdapServerObjects($sid = NULL, $type = NULL, $class = 'LdapServerTest') {

    $server_ids = variable_get('ldap_test_servers', array());
    $servers = array();
    foreach ($server_ids as $sid => $_sid) {
      $server_data = variable_get('ldap_test_server__' . $sid, array());
      $servers[$sid] = new LdapServerTest($server_data);
    }

    return $servers;

  }



}
