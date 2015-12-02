<?php
// $Id: LdapServer.class.php,v 1.5.2.1 2011/02/08 06:01:00 johnbarclay Exp $

/**
 * @file
 * Defines server classes and related functions.
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
class LdapServer {
  // LDAP Settings

  const LDAP_CONNECT_ERROR = 0x5b;
  const LDAP_SUCCESS = 0x00;
  const LDAP_OPERATIONS_ERROR = 0x01;
  const LDAP_PROTOCOL_ERROR = 0x02;

  public $sid;
  public $name;
  public $status;
  public $ldap_type;
  public $address;
  public $port = 389;
  public $tls = FALSE;
  public $bind_method = 0;
  public $basedn = array();
  public $binddn = FALSE; // Default to an anonymous bind.
  public $bindpw = FALSE; // Default to an anonymous bind.
  public $user_dn_expression;
  public $user_attr;
  public $account_name_attr;
  public $mail_attr;
  public $mail_template;
  public $unique_persistent_attr;
  public $unique_persistent_attr_binary = FALSE;
  public $allow_conflicting_drupal_accts = FALSE;
  public $ldapToDrupalUserPhp;
  public $testingDrupalUsername;
  public $detailed_watchdog_log;
  public $editPath;
  public $queriableWithoutUserCredentials = FALSE; // can this server be queried without user credentials provided?
  public $userAttributeNeededCache = array(); // array of attributes needed keyed on $op such as 'user_update'



  /**
   * @param scalar $puid is permanent unique id value and
   */

  public function drupalUserFromPuid($puid) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'user')
    ->fieldCondition('ldap_user_puid_sid', 'value', $this->sid, '=')
    ->fieldCondition('ldap_user_puid', 'value', $puid, '=')
    ->fieldCondition('ldap_user_puid_property', 'value', $this->unique_persistent_attr, '=')
    ->addMetaData('account', user_load(1)); // run the query as user 1

    $result = $query->execute();
    if (isset($result['user'])) {
      $user = entity_load('user', array_keys($result['user']));
    }

  }


  public $paginationEnabled = FALSE; // (boolean)(function_exists('ldap_control_paged_result_response') && function_exists('ldap_control_paged_result'));

  public $searchPagination = FALSE;
  public $searchPageSize = 1000;
  public $searchPageStart = 0;
  public $searchPageEnd = NULL;

  public $groupObjectClass;

  public $inDatabase = FALSE;

  public $connection;
  // direct mapping of db to object properties
  public static function field_to_properties_map() {
    return array( 'sid' => 'sid',
    'name'  => 'name' ,
    'status'  => 'status',
    'ldap_type'  => 'ldap_type',
    'address'  => 'address',
    'port'  => 'port',
    'tls'  => 'tls',
    'bind_method' => 'bind_method',
    'basedn'  => 'basedn',
    'binddn'  => 'binddn',
    'user_dn_expression' => 'user_dn_expression',
    'user_attr'  => 'user_attr',
    'account_name_attr'  => 'account_name_attr',
    'mail_attr'  => 'mail_attr',
    'mail_template'  => 'mail_template',
    'unique_persistent_attr' => 'unique_persistent_attr',
    'allow_conflicting_drupal_accts' => 'allow_conflicting_drupal_accts',
    'ldap_to_drupal_user'  => 'ldapToDrupalUserPhp',
    'testing_drupal_username'  => 'testingDrupalUsername',
    'group_object_category' => 'groupObjectClass',
    'search_pagination' => 'searchPagination',
    'search_page_size' => 'searchPageSize',
    );

  }

  /**
   * Constructor Method
   */
  function __construct($sid) {
    if (!is_scalar($sid)) {
      return;
    }


    $this->detailed_watchdog_log = variable_get('ldap_help_watchdog_detail', 0);
    $server_record = array();
    if (module_exists('ctools')) {
      ctools_include('export');
      $result = ctools_export_load_object('ldap_servers', 'names', array($sid));
      if (isset($result[$sid])) {
        $server_record[$sid] = $result[$sid];
        foreach ($server_record[$sid] as $property => $value) {
          $this->{$property} = $value;
        }
      }
    }
    else {
      $select = db_select('ldap_servers')
        ->fields('ldap_servers')
        ->condition('ldap_servers.sid',  $sid)
        ->execute();
      foreach ($select as $record) {
        $server_record[$record->sid] = $record;
      }
    }
    if (!isset($server_record[$sid])) {
      $this->inDatabase = FALSE;
      return;
    }
    $server_record = $server_record[$sid];

    if ($server_record) {
      $this->inDatabase = TRUE;
      $this->sid = $sid;
      $this->detailedWatchdogLog = variable_get('ldap_help_watchdog_detail', 0);
    }
    else {
      // @todo throw error
    }

    foreach ($this->field_to_properties_map() as $db_field_name => $property_name ) {
      if (isset($server_record->$db_field_name)) {
        $this->{$property_name} = $server_record->$db_field_name;
      }
    }
    if (is_scalar($this->basedn)) {
      $this->basedn = unserialize($this->basedn);
    }
    if (isset($server_record->bindpw) && $server_record->bindpw != '') {
      $this->bindpw = $server_record->bindpw;
      $this->bindpw = ldap_servers_decrypt($this->bindpw);
    }
	$this->paginationEnabled = (boolean)(ldap_servers_php_supports_pagination() && $this->searchPagination);

    $this->queriableWithoutUserCredentials = (boolean)(
      $this->bind_method == LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT ||
      $this->bind_method == LDAP_SERVERS_BIND_METHOD_ANON_USER
    );
    $this->editPath = 'admin/config/people/ldap/servers/edit/' . $this->sid;
  }

  /**
   * Destructor Method
   */
  function __destruct() {
    // Close the server connection to be sure.
    $this->disconnect();
  }


  /**
   * Invoke Method
   */
  function __invoke() {
    $this->connect();
    $this->bind();
  }



  /**
   * Connect Method
   */
  function connect() {

    if (!$con = ldap_connect($this->address, $this->port)) {
      watchdog('user', 'LDAP Connect failure to ' . $this->address . ':' . $this->port);
      return LDAP_CONNECT_ERROR;
    }

    ldap_set_option($con, LDAP_OPT_PROTOCOL_VERSION, 3);
    ldap_set_option($con, LDAP_OPT_REFERRALS, 0);

    // Use TLS if we are configured and able to.
    if ($this->tls) {
      ldap_get_option($con, LDAP_OPT_PROTOCOL_VERSION, $vers);
      if ($vers == -1) {
        watchdog('user', 'Could not get LDAP protocol version.');
        return LDAP_PROTOCOL_ERROR;
      }
      if ($vers != 3) {
        watchdog('user', 'Could not start TLS, only supported by LDAP v3.');
        return LDAP_CONNECT_ERROR;
      }
      elseif (!function_exists('ldap_start_tls')) {
        watchdog('user', 'Could not start TLS. It does not seem to be supported by this PHP setup.');
        return LDAP_CONNECT_ERROR;
      }
      elseif (!ldap_start_tls($con)) {
        $msg =  t("Could not start TLS. (Error %errno: %error).", array('%errno' => ldap_errno($con), '%error' => ldap_error($con)));
        watchdog('user', $msg);
        return LDAP_CONNECT_ERROR;
      }
    }

  // Store the resulting resource
  $this->connection = $con;
  return LDAP_SUCCESS;
  }


  /**
	 * Bind (authenticate) against an active LDAP database.
	 *
	 * @param $userdn
	 *   The DN to bind against. If NULL, we use $this->binddn
	 * @param $pass
	 *   The password search base. If NULL, we use $this->bindpw
   *
   * @return
   *   Result of bind; TRUE if successful, FALSE otherwise.
   */
  function bind($userdn = NULL, $pass = NULL, $anon_bind = FALSE) {

    // Ensure that we have an active server connection.
    if (!$this->connection) {
      watchdog('ldap', "LDAP bind failure for user %user. Not connected to LDAP server.", array('%user' => $userdn));
      return LDAP_CONNECT_ERROR;
    }
    if ($anon_bind) {
      if (@!ldap_bind($this->connection)) {
        watchdog('ldap', "LDAP anonymous bind error. Error %errno: %error", array('%errno' => ldap_errno($this->connection), '%error' => ldap_error($this->connection)));
        return ldap_errno($this->connection);
      }
    }
    else {
      $userdn = ($userdn != NULL) ? $userdn : $this->binddn;
      $pass = ($pass != NULL) ? $pass : $this->bindpw;
      if (@!ldap_bind($this->connection, $userdn, $pass)) {
        watchdog('ldap', "LDAP bind failure for user %user. Error %errno: %error", array('%user' => $userdn, '%errno' => ldap_errno($this->connection), '%error' => ldap_error($this->connection)));
        return ldap_errno($this->connection);
      }
    }

    return LDAP_SUCCESS;
  }

  /**
   * Disconnect (unbind) from an active LDAP server.
   */
  function disconnect() {
    if (!$this->connection) {
      // never bound or not currently bound, so no need to disconnect
      //watchdog('ldap', 'LDAP disconnect failure from '. $this->server_addr . ':' . $this->port);
    }
    else {
      ldap_unbind($this->connection);
      $this->connection = NULL;
    }
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

  function search($base_dn = NULL, $filter, $attributes = array(),
    $attrsonly = 0, $sizelimit = 0, $timelimit = 0, $deref = NULL, $scope = LDAP_SCOPE_SUBTREE) {

     /**
      * pagingation issues:
      * -- see documentation queue: http://markmail.org/message/52w24iae3g43ikix#query:+page:1+mid:bez5vpl6smgzmymy+state:results
      * -- wait for php 5.4? https://svn.php.net/repository/php/php-src/tags/php_5_4_0RC6/NEWS (ldap_control_paged_result
      * -- http://sgehrig.wordpress.com/2009/11/06/reading-paged-ldap-results-with-php-is-a-show-stopper/
      */

    if ($base_dn == NULL) {
      if (count($this->basedn) == 1) {
        $base_dn = $this->basedn[0];
      }
      else {
        return FALSE;
      }
    }

    $attr_display =  is_array($attributes) ? join(',', $attributes) : 'none';
    $query = 'ldap_search() call: '. join(",\n", array(
      'base_dn: ' . $base_dn,
      'filter = ' . $filter,
      'attributes: ' . $attr_display,
      'attrsonly = ' .  $attrsonly,
      'sizelimit = ' .  $sizelimit,
      'timelimit = ' .  $timelimit,
      'deref = ' .  $deref,
      'scope = ' .  $scope,
      )
    );

    if ($this->detailed_watchdog_log) {
      watchdog('ldap_server', $query, array());
    }

    // When checking multiple servers, there's a chance we might not be connected yet.
    if (! $this->connection) {
      $this->connect();
      $this->bind();
    }

    $ldap_query_params = array(
      'connection' => $this->connection,
      'base_dn' => $base_dn,
      'filter' => $filter,
      'attributes' => $attributes,
      'attrsonly' => $attrsonly,
      'sizelimit' => $sizelimit,
      'timelimit' => $timelimit,
      'deref' => $deref,
      'query_display' => $query,
      'scope' => $scope,
    );
   // dpm($ldap_query_params); dpm("searchPagination=" . $this->searchPagination .",paginationEnabled=". $this->paginationEnabled .", searchPageStart=" . $this->searchPageStart);
    if ($this->searchPagination && $this->paginationEnabled) {
      $aggregated_entries = $this->pagedLdapQuery($ldap_query_params);
      return $aggregated_entries;
    }
    else {
      $result = $this->ldapQuery($scope, $ldap_query_params);
      if ($result && (ldap_count_entries($this->connection, $result) !== FALSE) ) {
        $entries = ldap_get_entries($this->connection, $result);
        drupal_alter('ldap_server_search_results', $entries, $ldap_query_params);
        return (is_array($entries)) ? $entries : FALSE;
      }
      elseif ($this->ldapErrorNumber()) {
        $watchdog_tokens =  array('%basedn' => $ldap_query_params['base_dn'], '%filter' => $ldap_query_params['filter'],
          '%attributes' => print_r($ldap_query_params['attributes'], TRUE), '%errmsg' => $this->errorMsg('ldap'),
          '%errno' => $this->ldapErrorNumber());
        watchdog('ldap', "LDAP ldap_search error. basedn: %basedn| filter: %filter| attributes:
          %attributes| errmsg: %errmsg| ldap err no: %errno|", $watchdog_tokens);
        return FALSE;
      }
      else {
        return FALSE;
      }
    }
  }


  /**
   * execute a paged ldap query and return entries as one aggregated array
   *
   * $this->searchPageStart and $this->searchPageEnd should be set before calling if
   *   a particular set of pages is desired
   *
   * @param array $ldap_query_params of form:
      'base_dn' => base_dn,
      'filter' =>  filter,
      'attributes' => attributes,
      'attrsonly' => attrsonly,
      'sizelimit' => sizelimit,
      'timelimit' => timelimit,
      'deref' => deref,
      'scope' => scope,

      (this array of parameters is primarily passed on to ldapQuery() method)
   *
   * @return array of ldap entries or FALSE on error.
   *
   */
  public function pagedLdapQuery($ldap_query_params) {

    if (!($this->searchPagination && $this->paginationEnabled)) {
      watchdog('ldap', "LDAP server pagedLdapQuery() called when functionality not available in php install or
        not enabled in ldap server configuration.  error. basedn: %basedn| filter: %filter| attributes:
         %attributes| errmsg: %errmsg| ldap err no: %errno|", $watchdog_tokens);
      RETURN FALSE;
    }

    $paged_entries = array();
    $page_token = '';
    $page = 0;
    $estimated_entries = 0;
    $aggregated_entries = array();
    $aggregated_entries_count = 0;
    $has_page_results = FALSE;

    do {
      ldap_control_paged_result($this->connection, $this->searchPageSize, true, $page_token);
      $result = $this->ldapQuery($ldap_query_params['scope'], $ldap_query_params);

      if ($page >= $this->searchPageStart) {
        $skipped_page = FALSE;
        if ($result && (ldap_count_entries($this->connection, $result) !== FALSE) ) {
          $page_entries = ldap_get_entries($this->connection, $result);
          unset($page_entries['count']);
          $has_page_results = (is_array($page_entries) && count($page_entries) > 0);
          $aggregated_entries = array_merge($aggregated_entries, $page_entries);
          $aggregated_entries_count = count($aggregated_entries);
        }
        elseif ($this->ldapErrorNumber()) {
          $watchdog_tokens =  array('%basedn' => $ldap_query_params['base_dn'], '%filter' => $ldap_query_params['filter'],
            '%attributes' => print_r($ldap_query_params['attributes'], TRUE), '%errmsg' => $this->errorMsg('ldap'),
            '%errno' => $this->ldapErrorNumber());
          watchdog('ldap', "LDAP ldap_search error. basedn: %basedn| filter: %filter| attributes:
            %attributes| errmsg: %errmsg| ldap err no: %errno|", $watchdog_tokens);
          RETURN FALSE;
        }
        else {
          return FALSE;
        }
      }
      else {
        $skipped_page = TRUE;
      }
      @ldap_control_paged_result_response($this->connection, $result, $page_token, $estimated_entries);
      if ($ldap_query_params['sizelimit'] && $this->ldapErrorNumber() == LDAP_SIZELIMIT_EXCEEDED) {
        // false positive error thrown.  do not set result limit error when $sizelimit specified
      }
      elseif ($this->hasError()) {
        watchdog('ldap_server', 'ldap_control_paged_result_response() function error. LDAP Error: %message, ldap_list() parameters: %query',
          array('%message' => $this->errorMsg('ldap'), '%query' => $ldap_query_params['query_display']),
          WATCHDOG_ERROR);
      }

      if (isset($ldap_query_params['sizelimit']) && $ldap_query_params['sizelimit'] && $aggregated_entries_count >= $ldap_query_params['sizelimit']) {
        $discarded_entries = array_splice($aggregated_entries, $ldap_query_params['sizelimit']);
        break;
      }
      elseif ($this->searchPageEnd !== NULL && $page >= $this->searchPageEnd) { // user defined pagination has run out
        break;
      }
      elseif ($page_token === NULL || $page_token == '') { // ldap reference pagination has run out
        break;
      }
      $page++;
    } while ($skipped_page || $has_page_results);

    $aggregated_entries['count'] = count($aggregated_entries);
    return $aggregated_entries;
  }

  function ldapQuery($scope, $params) {

    switch ($scope) {
      case LDAP_SCOPE_SUBTREE:
        $result = @ldap_search($this->connection, $params['base_dn'], $params['filter'], $params['attributes'], $params['attrsonly'],
          $params['sizelimit'], $params['timelimit'], $params['deref']);
        if ($params['sizelimit'] && $this->ldapErrorNumber() == LDAP_SIZELIMIT_EXCEEDED) {
          // false positive error thrown.  do not return result limit error when $sizelimit specified
        }
        elseif ($this->hasError()) {
          watchdog('ldap_server', 'ldap_search() function error. LDAP Error: %message, ldap_search() parameters: %query',
            array('%message' => $this->errorMsg('ldap'), '%query' => $params['query_display']),
            WATCHDOG_ERROR);
        }
        break;

      case LDAP_SCOPE_BASE:
        $result = @ldap_read($this->connection, $params['base_dn'], $params['filter'], $params['attributes'], $params['attrsonly'],
           $params['sizelimit'], $params['timelimit'], $params['deref']);
        if ($params['sizelimit'] && $this->ldapErrorNumber() == LDAP_SIZELIMIT_EXCEEDED) {
          // false positive error thrown.  do not result limit error when $sizelimit specified
        }
        elseif ($this->hasError()) {
          watchdog('ldap_server', 'ldap_read() function error.  LDAP Error: %message, ldap_read() parameters: %query',
            array('%message' => $this->errorMsg('ldap'), '%query' => $params['query_display']),
            WATCHDOG_ERROR);
        }
        break;

      case LDAP_SCOPE_ONELEVEL:
        $result = @ldap_list($this->connection, $params['base_dn'], $params['filter'], $params['attributes'], $params['attrsonly'],
           $params['sizelimit'], $params['timelimit'], $params['deref']);
        if ($params['sizelimit'] && $this->ldapErrorNumber() == LDAP_SIZELIMIT_EXCEEDED) {
          // false positive error thrown.  do not result limit error when $sizelimit specified
        }
        elseif ($this->hasError()) {
          watchdog('ldap_server', 'ldap_list() function error. LDAP Error: %message, ldap_list() parameters: %query',
            array('%message' => $this->errorMsg('ldap'), '%query' => $params['query_display']),
            WATCHDOG_ERROR);
        }
        break;
    }
    return $result;
  }

  function drupalToLdapNameTransform($drupal_username, &$watchdog_tokens) {
    if ($this->ldapToDrupalUserPhp && module_exists('php')) {
      global $name;
      $old_name_value = $name;
      $name = $drupal_username;
      $code = "<?php global \$name; \n". $this->ldapToDrupalUserPhp . "; \n ?>";
      $watchdog_tokens['%code'] = $this->ldapToDrupalUserPhp;
      $code_result = php_eval($code);
      $watchdog_tokens['%code_result'] = $code_result;
      $ldap_username = $code_result;
      $watchdog_tokens['%ldap_username'] = $ldap_username;
      $name = $old_name_value;  // important because of global scope of $name
      if ($this->detailedWatchdogLog) {
        watchdog('ldap_server', '%drupal_user_name tansformed to %ldap_username by applying code <code>%code</code>', $watchdog_tokens, WATCHDOG_DEBUG);
      }
    }
    else {
      $ldap_username = $drupal_username;
    }

    return $ldap_username;

  }
  /**
   * Queries LDAP server for the user.
   *
   * @param $drupal_user_name
   *  drupal user name.
   *
   * @return
   *   An array with users LDAP data or NULL if not found.
   */
  function user_lookup($drupal_user_name) {
    $watchdog_tokens = array('%drupal_user_name' => $drupal_user_name);
    $ldap_username = $this->drupalToLdapNameTransform($drupal_user_name, $watchdog_tokens);

    if (!$ldap_username) {
      return FALSE;
    }

    foreach ($this->basedn as $basedn) {
      if (empty($basedn)) continue;
      $filter = '('. $this->user_attr . '=' . ldap_server_massage_text($ldap_username, 'attr_value', LDAP_SERVER_MASSAGE_QUERY_LDAP)   . ')';
      $result = $this->search($basedn, $filter);
      if (!$result || !isset($result['count']) || !$result['count']) continue;

      // Must find exactly one user for authentication to work.
      if ($result['count'] != 1) {
        $count = $result['count'];
        watchdog('ldap_servers', "Error: !count users found with $filter under $basedn.", array('!count' => $count), WATCHDOG_ERROR);
        continue;
      }
      $match = $result[0];
      // These lines serve to fix the attribute name in case a
      // naughty server (i.e.: MS Active Directory) is messing the
      // characters' case.
      // This was contributed by Dan "Gribnif" Wilga, and described
      // here: http://drupal.org/node/87833
      $name_attr = $this->user_attr;
      if (isset($match[$name_attr][0])) {

      }
      elseif (isset($match[drupal_strtolower($name_attr)][0])) {
        $name_attr = drupal_strtolower($name_attr);
      }
      else {
        if ($this->bind_method == LDAP_SERVERS_BIND_METHOD_ANON_USER) {
          $result = array(
            'dn' =>  $match['dn'],
            'mail' => $this->deriveEmailFromEntry($match),
            'attr' => $match,
            );
          return $result;
        }
        else {
          continue;
        }
      }

      // Finally, we must filter out results with spaces added before
      // or after, which are considered OK by LDAP but are no good for us
      // We allow lettercase independence, as requested by Marc Galera
      // on http://drupal.org/node/97728
      //
      // Some setups have multiple $name_attr per entry, as pointed out by
      // Clarence "sparr" Risher on http://drupal.org/node/102008, so we
      // loop through all possible options.
      foreach ($match[$name_attr] as $value) {
        if (drupal_strtolower(trim($value)) == drupal_strtolower($ldap_username)) {
          $result = array(
            'dn' =>  $match['dn'],
            'mail' => $this->deriveEmailFromEntry($match),
            'attr' => $match,
          );

          return $result;
        }
      }
    }
  }

  /**
   * return by reference groups/authorizations when groups are defined from user attributes (such as memberOf)
   *
   *  @param array $derive_from_attribute_name.  e.g. memberOf
   *  @param array $user_ldap_entry as returned by ldap php extension
   *  @param boolean $nested if groups should be recursed or not.
   *
   *  @return array of groups specified in the derive from attribute
   */

  public function deriveFromAttrGroups($derive_from_attribute_name, $user_ldap_entry, $nested) {
    $all_groups = array();
    $groups_by_level = array();
    $level = 0;
    foreach ($user_ldap_entry['attr'] as $user_attr_name => $user_attr_values) {
      if (strcasecmp($derive_from_attribute_name, $user_attr_name) != 0) {
        continue;
      }
      // patch 1050944
      for ($i = 0; $i < $user_attr_values['count']; $i++) {
        $all_groups[] = (string)$user_attr_values[$i];
        $groups_by_level[$derive_from_attribute_name][$level][] = (string)$user_attr_values[$i];
      }
      if ($nested) {
        $this->deriveFromAttrGroupsResursive($all_groups, $groups_by_level, $level, $derive_from_attribute_name, 10); // LDAP_SERVER_GROUPS_RECURSE_DEPTH
      }
    }
    return array_unique($all_groups);
  }

  /**
   * not working yet
   * will be ton of permission issues with service accounts
   * need configurable obj type to avoid binding to a million user entries, printers, etc.
   */
  private function deriveFromAttrGroupsResursive(&$all_groups, &$groups_by_level, $level, $derive_from_attribute_name, $max_depth) {
    // derive query with & of all groups at current level
    // e.g. (|(distinguishedname=cn=content editors,ou=groups,dc=ad,dc=myuniversity,dc=edu)(distinguishedname=cn=content approvers,ou=groups,dc=ad,dc=myuniversity,dc=edu))
    // execute query and loop through it to populate $groups_by_level[$level + 1]
    // call recursively provided max depth not excluded and $groups_by_level[$level + 1] > 0

    // this needs to be configurable also and default per ldap implementation
    $group_values = $groups_by_level[$derive_from_attribute_name][$level];
    $filter = "(&\n  (objectClass=" . ldap_server_massage_text($this->groupObjectClass, 'attr_value', LDAP_SERVER_MASSAGE_QUERY_LDAP) . ")\n" .
     "(" . $derive_from_attribute_name . "=*)\n" .
     "(|\n    (distinguishedname=" . join(")\n    (distinguishedname=", ldap_server_massage_text($group_values, 'attr_value', LDAP_SERVER_MASSAGE_QUERY_LDAP)) . ")\n  )\n)";
    $level++;
    foreach ($this->basedn as $base_dn) {  // need to search on all basedns one at a time
      $entries = $this->search($base_dn, $filter, array($derive_from_attribute_name));
      foreach ($entries as $entry) {
        $attr_values = array();
        if (is_array($entry) && count($entry)) {
          if (isset($entry[$derive_from_attribute_name])) {
            $attr_values = $entry[$derive_from_attribute_name];
          }
          elseif (isset($entry[drupal_strtolower($derive_from_attribute_name)])) {
            $attr_values = $entry[drupal_strtolower($derive_from_attribute_name)];
          }
          else {
            foreach ($entry as $attr_name => $values) {
              if (strcasecmp($derive_from_attribute_name, $attr_name) != 0) {
                continue;
              }
              $attr_values = $entry[$attr_name];
              break;
            }
          }
          if (count($attr_values)) {
            for ($i = 0; $i < $attr_values['count']; $i++) {
              $value = (string)$attr_values[$i];
              if (!in_array($value, $all_groups)) {
                $groups_by_level[$derive_from_attribute_name][$level][] = $value;
                $all_groups[] = $value;
              }
            }
          }
        }
      }
    }
    if (isset($groups_by_level[$derive_from_attribute_name][$level]) && count($groups_by_level[$derive_from_attribute_name][$level]) && $level < $max_depth) {
      $this->deriveFromAttrGroupsResursive($all_groups, $groups_by_level, $level, $derive_from_attribute_name, $max_depth);
    }
  }

  /**
   * return by reference groups/authorizations when groups are defined from entry
   *
   *  @param array $entries.  e.g. array('cn=it,cn=groups,dc=ad,dc=myuniversity,dc=edu')
   *  @param string ldap attribute name $entries_attr e.g. dn, cn
   *
   *  @param string $membership_attr e.g. uniquemember
   *  @param string $user_ldap_attr e.g.  cn, dn, etc.
   *  @param boolean $nested if groups should be recursed or not.
   *
   *  @return array of groups specified in the derive from entry
   *
   *  @see tests/DeriveFromEntry/ldap_servers.inc for fuller notes and test example
   */
  public function deriveFromEntryGroups($entries, $entries_attr, $membership_attr, $user_ldap_attr, $user_ldap_entry, $nested = FALSE) {

    $authorizations = array();
    $matching_user_value = ($user_ldap_attr == 'dn') ? $user_ldap_entry['dn'] : $user_ldap_entry['attr'][$user_ldap_attr][0];
    $filter  = "(|\n    ($entries_attr=" . join(")\n    ($entries_attr=", ldap_server_massage_text($entries, 'attr_value', LDAP_SERVER_MASSAGE_QUERY_LDAP)) . ")\n)";
    if (!$nested) {
      $filter =  "(&\n  $filter  \n  (" . $membership_attr . "=" .  ldap_server_massage_text($matching_user_value, 'attr_value', LDAP_SERVER_MASSAGE_QUERY_LDAP) . ")  \n)";
    }

    $tested_groups = array(); // array of dns already tested to avoid excess queried
    foreach ($this->basedn as $base_dn) {  // need to search on all basedns one at a time
      $entries = $this->search($base_dn, $filter, array('dn', $membership_attr, $entries_attr, $user_ldap_attr, 'objectClass'));  // query for all dns list
      if ($entries !== FALSE) {
        if (!$nested) {  // if not nested all returned entries are groups that user is member of
          foreach ($entries as $entry) {
            if (isset($entry[$entries_attr])) {
              $group_id = (string)($entries_attr == 'dn') ? (string)$entry['dn'] : (string)$entry[$entries_attr][0];
              $authorizations[] = $group_id;
              $tested_groups[] = $group_id;
            }
          }
        }
        else { // if nested all returned entries are groups.  user is not necessarily a member of them
          if (isset($entries['count'])) {
            unset($entries['count']);
          };
          foreach ($entries as $i => $entry) {
            $group_id = ($entries_attr == 'dn') ? (string)$entry['dn'] : (string)$entry[$entries_attr][0];
            if (!in_array($group_id, $tested_groups) && isset($entry[$membership_attr])) {
              $members = $entry[$membership_attr];

              unset($members['count']);
              // user may be direct member of group
              if (in_array($matching_user_value, array_values($members))) {
                $authorizations[] = $group_id;
              }
              else {  // $derive_from_entry_attr, $user_ldap_attr, $user_ldap_entry $entries, $entries_attr,
                $is_member_via_child_groups = $this->groupsByEntryIsMember($members, $entries_attr, $base_dn,  $tested_groups, $membership_attr, $matching_user_value, 0, 10);
                if ($is_member_via_child_groups) {
                   $authorizations[] = $group_id;
                }
              }
            }
            $tested_groups[] = $group_id;
          }
        }
      }
    }

    return $authorizations;
  }

  /** looking at all members of a child group.  only need to determine if member of one of the groups, doesn't matter which one.
   *
   *  @param string ldap attribute value $group_id. represents group in question
   *  @param array $members. list of current group members  e.g. array('cn=it,cn=groups,dc=ad,dc=myuniversity,dc=edu')
   *  @param string ldap attribute name $entries_attr that $members represent
   *
   *  @param string $base_dn to be searched
   *  @param array $tested_groups is an array of group_ids in form of whatever $entries_attr is (e.g. cns, dns,...)

   *  @param string $membership_attr e.g. uniquemember
   *  @param array $user_ldap_entry
   *  @param int $depth, current recursion depth
   *  @param int $max_depth, max allowed recursion
   *
   *
   *  @return TRUE or FALSE
   *
   *  @see tests/DeriveFromEntry/ldap_servers.inc for fuller notes and test example
   */

  public function groupsByEntryIsMember($members, $entries_attr, $base_dn, &$tested_groups, $membership_attr, $matching_user_value, $depth, $max_depth) {
    // query for all members that are groups
    $filter = "(&(objectClass=". ldap_server_massage_text($this->groupObjectClass, 'attr_value', LDAP_SERVER_MASSAGE_QUERY_LDAP)
    . ")(|\n  ($entries_attr="
    . join(")\n    ($entries_attr=", ldap_server_massage_text($members, 'attr_value', LDAP_SERVER_MASSAGE_QUERY_LDAP)) . ")\n  ))";
    $entries = $this->search($base_dn, $filter, array('dn', $entries_attr, $membership_attr));

    if (isset($entries['count'])) {
      unset($entries['count']);
    };
    if ($entries !== FALSE) {
      foreach ($entries as $i => $entry) {
        $group_id = ($entries_attr == 'dn' || $entries_attr == 'distinguishedname') ? (string)$entry['dn'] : (string)$entry[$entries_attr][0];
        if (!in_array($group_id, $tested_groups)) {
          $tested_groups[] = $group_id;
          $child_members = (isset($entry[$membership_attr])) ? $entry[$membership_attr] : array('count' => 0);
          unset($child_members['count']);

          if (count($child_members) == 0) {
            return FALSE;
          }
          elseif (in_array($matching_user_value, array_values($child_members))) {
            return TRUE; // user is direct member of child group
          }
          elseif ($depth < $max_depth) { // $derive_from_entry_attr, $user_ldap_attr, $user_ldap_entry
            $result = $this->groupsByEntryIsMember($child_members, $entries_attr, $base_dn, $tested_groups, $membership_attr, $matching_user_value, $depth + 1, $max_depth);
            return $result;
          }
        }
      }
    }
    return FALSE;
  }

  public function deriveEmailFromEntry($ldap_entry) {
    if ($this->mail_attr) { // not using template
      return @$ldap_entry[ldap_server_massage_text($this->mail_attr, 'attr_name', LDAP_SERVER_MASSAGE_QUERY_ARRAY)][0];
    }
    elseif ($this->mail_template) {  // template is of form [cn]@illinois.edu
      ldap_server_module_load_include('inc', 'ldap_servers', 'ldap_servers.functions');
      return ldap_server_token_replace($ldap_entry, $this->mail_template);
    }
    else {
      return FALSE;
    }
  }


  /**
   * Error methods and properties.
   */

  public $detailedWatchdogLog = FALSE;
  protected $_errorMsg = NULL;
  protected $_hasError = FALSE;
  protected $_errorName = NULL;

  public function setError($_errorName, $_errorMsgText = NULL) {
    $this->_errorMsgText = $_errorMsgText;
    $this->_errorName = $_errorName;
    $this->_hasError = TRUE;
  }

  public function clearError() {
    $this->_hasError = FALSE;
    $this->_errorMsg = NULL;
    $this->_errorName = NULL;
  }

  public function hasError() {
    return ($this->_hasError || $this->ldapErrorNumber());
  }

  public function errorMsg($type = NULL) {
    if ($type == 'ldap' && $this->connection) {
      return ldap_err2str(ldap_errno($this->connection));
    }
    elseif ($type == NULL) {
      return $this->_errorMsg;
    }
    else {
      return NULL;
    }
  }

  public function errorName($type = NULL) {
    if ($type == 'ldap' && $this->connection) {
      return "LDAP Error: " . ldap_error($this->connection);
    }
    elseif ($type == NULL) {
      return $this->_errorName;
    }
    else {
      return NULL;
    }
  }

  public function ldapErrorNumber() {
    if ($this->connection && ldap_errno($this->connection)) {
      return ldap_errno($this->connection);
    }
    else {
      return FALSE;
    }
  }

}
