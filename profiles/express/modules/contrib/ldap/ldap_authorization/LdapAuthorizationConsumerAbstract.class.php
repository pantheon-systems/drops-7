<?php

/**
 * @file
 *
 * abstract class to represent an ldap_authorization consumer behavior
 * such as drupal_role, og_group, etc.  each authorization comsumer
 * will extend this class with its own class named
 * LdapAuthorizationConsumer<consumer type> such as LdapAuthorizationConsumerDrupalRole
 *
 */

class LdapAuthorizationConsumerAbstract {

  public $consumerType = NULL; // machine name of consumer.  e.g. og_group, drupal_role, etc.

  /**
   * the following properties are generally populated from a
   * call to hook_ldap_authorization_consumer()
   */
  public $name;  // user interface name of consumer. e.g.  drupal role, og group
  public $namePlural; // user interface name of consumer. e.g. drupal roles, og groups
  public $shortName; // user interface short name of consumer. e.g. role, group
  public $shortNamePlural; //  user interface short name of consumer plural, e.g. roles, groups
  public $description;// e.g. roles, groups
  public $consumerModule; // module providing consumer functionality e.g. ldap_authorization_drupal_roles

  public $consumerConf; // LDAPConsumerConf object class encapuslating admin form
  public $testLink; // link to test this consumer
  public $editLink; // link to configure this consumer

  public $emptyConsumer = array(
    'exists' => TRUE,
    'value' => NULL,
    'name' => NULL,
    'map_to_string' => NULL
    );

   /**
   * @property boolean $allowConsumerObjectCreation
   *
   *  Does this consumer module support creating consumer objects
   * (drupal roles,  og groups, etc.)
   *
   */

  public $allowConsumerObjectCreation = FALSE;

   /**
   * @property boolean $detailedWatchdogLog
   *
   *  should watchdog log be used for debugging, useful for non programmers
   *  who don't have php debugging enabled
   *
   */
  public $detailedWatchdogLog = FALSE;


   /**
   * @property array $defaultConsumerConfProperties
   * default properties for consumer admin UI form
   */
  public $defaultConsumerConfProperties = array(
      'onlyApplyToLdapAuthenticated' => TRUE,
      'useMappingsAsFilter' => TRUE,
      'synchOnLogon' => TRUE,
      'revokeLdapProvisioned' => TRUE,
      'regrantLdapProvisioned' => TRUE,
      'createConsumers' => TRUE,
      );

 /**
   * Constructor Method
   *
   * @param string $consumer_type e.g. drupal_role, og_group
   * @param array $params as associative array of default properties
   *
   */
  function __construct($consumer_type, $params) {
    $this->consumerType = $consumer_type;
    $this->name = $params['consumer_name'];
    $this->namePlural= $params['consumer_name_plural'];
    $this->shortName = $params['consumer_short_name'];
    $this->shortNamePlural= $params['consumer_short_name_plural'];
    $this->consumerModule = $params['consumer_module'];
    $this->mappingDirections = $params['consumer_mapping_directions'];
    $this->testLink = l(t('test') . ' ' . $this->name, LDAP_SERVERS_MENU_BASE_PATH . '/authorization/test/' . $this->consumerType);
    $this->editLink = l(t('edit') . ' ' . $this->name, LDAP_SERVERS_MENU_BASE_PATH . '/authorization/edit/' . $this->consumerType);
    ldap_servers_module_load_include('php', 'ldap_authorization', 'LdapAuthorizationConsumerConfAdmin.class');
    $this->consumerConf = new LdapAuthorizationConsumerConf($this);
  }


  /**
   * function to normalize mappings
   * should be overridden when mappings are not stored as map|authorization_id format
   * where authorization_id is the format returned by
   *   LdapAuthorizationConsumerAbstract::usersAuthorizations()
   *
   * for example ldap_authorization_og may store mapping target as:
   *   Campus Accounts|group-name=knitters,role-name=administrator member
   *
   *   normalized mappings are of form such as for organic groups:
   *
   *   array(
         array(
           'from' => 'students',
           'normalized' => 'node:21:1',
           'simplified' => 'node:students:member',
           'user_entered' => 'students'
           'valid' => TRUE,
           'error_message' => '',
           ),

         ...
        )

   *   or for drupal role where rid 3 is moderator and rid 2 is admin:
   *   array(
         array(
           'from' => 'students',
           'normalized' => '2',
           'simplified' => 'admin',
           'user_entered' => 'admin',
           'valid' => TRUE,
           'error_message' => '',
           ),
         ...
        )

        where 'normalized' is in id format and 'simplified' is user shorthand
   )
   */
  public function normalizeMappings($mappings) {
    return $mappings;
  }

  /**
   *
   * create authorization consumers
   *
   * @param string (lowercase) $consumer_id
   * @param array $consumer as associative array with the following key/values
   *   'value' => NULL | mixed consumer such as drupal role name, og group entity, etc.
   *   'name' => name of consumer for UI, logging etc.
   *   'map_to_string' => string mapped to in ldap authorization.  mixed case string
   *   'exists' => TRUE indicates consumer is known to exist,
   *               FALSE indicates consumer is known to not exist,
   *               NULL indicate consumer's existance not checked yet
   *
   */
  public function createConsumer($consumer_id, $consumer) {
    // method must be overridden
  }

  /**
   * populate consumer side of $consumers array
   *
   * @param array $consumers as associative array keyed on $consumer_id with values
   *   of $consumer.  $consumer_id and $consumer have structure in LdapAuthorizationConsumerAbstractClass::createConsumer
   *   when values are $consumer['exists'] != TRUE need to be populated by consumer object
   * @param boolean $create_missing_consumers indicates if consumers (drupal roles, og groups, etc) should be created
   *   if values are NULL, object will be created if
   *
   * @return $consumers by reference
   */

  public function populateConsumersFromConsumerIds(&$consumers, $create_missing_consumers = FALSE) {
    // method must be overridden
  }

  public function authorizationDiff($initial, $current) {
    return array_diff($initial, $current);
  }


  /**
   * grant authorizations to a user
   *
   * @param object $user drupal user object
   * @param array $consumers in form of LdapAuthorizationConsumerAbstractClass::populateConsumersFromConsumerIds
   * @param array $ldap_entry is ldap data from ldap entry which drupal user is mapped to
   * @param boolean $user_save.  should user object be saved by authorizationGrant method
   *
   * @return array $results.  Array of form
   *   array(
   *    <authz consumer id1> => 1,
   *    <authz consumer id2> => 0,
   *   )
   *   where 1s and 0s represent success and failure to grant
   *
   *
   *  method may be desireable to override, if consumer benefits from adding grants as a group rather than one at a time
   */

  public function authorizationGrant(&$user, &$user_auth_data, $consumers, $ldap_entry = NULL, $user_save = TRUE) {
    $this->filterOffPastAuthorizationRecords($user, $user_auth_data);
    $this->grantsAndRevokes('grant', $user, $user_auth_data, $consumers, $ldap_entry, $user_save);
  }

  /**
   * revoke authorizations to a user
   *
   * @param object $user drupal user object
   * @param array $consumers in form of LdapAuthorizationConsumerAbstractClass::populateConsumersFromConsumerIds
   * @param array $ldap_entry is ldap data from ldap entry which drupal user is mapped to
   * @param boolean $user_save.  should user object be saved by authorizationGrant method
   *
   * @return array $results.  Array of form
   *   array(
   *    <authz consumer id1> => 1,
   *    <authz consumer id2> => 0,
   *   )
   *   where 1s and 0s represent success and failure to revoke
   *  $user_auth_data is returned by reference
   *
   *  method may be desireable to override, if consumer benefits from revoking grants as a group rather than one at a time
   */

  public function authorizationRevoke(&$user, &$user_auth_data, $consumers, $ldap_entry, $user_save = TRUE) {
    $this->filterOffPastAuthorizationRecords($user, $user_auth_data);
    $this->grantsAndRevokes('revoke', $user, $user_auth_data, $consumers, $ldap_entry, $user_save);
  }



  /**
   * this is a function to clear off
   */
  public function filterOffPastAuthorizationRecords(&$user, &$user_auth_data, $time = NULL) {
    if ($time != NULL || variable_get('ldap_help_user_data_clear', 0)) {
      $clear_time = ($time) ? $time : variable_get('ldap_help_user_data_clear_set_date', 0);
      if ($clear_time > 0 && $clear_time < time()) {
        foreach ($user_auth_data as $consumer_id => $entry) {
          if ($entry['date_granted'] < $clear_time) {
            unset($user_auth_data[$consumer_id]);
            if (isset($user->data['ldap_authorizations'][$this->consumerType][$consumer_id])) {
              unset($user->data['ldap_authorizations'][$this->consumerType][$consumer_id]);
            }
          }
        }
      }
    }
  }

  /**
   * some authorization schemes such as organic groups, require a certain order.  implement this method
   * to sort consumer ids/authorization ids
   *
   * @param string $op 'grant' or 'revoke' signifying what to do with the $consumer_ids
   * @param $consumers associative array in form of LdapAuthorizationConsumerAbstract::populateConsumersFromConsumerIds
   *
   * alters $consumers by reference
   *
   */
  public function sortConsumerIds($op, &$consumers) { }


  /**
   * attempt to flush related caches.  This will be something like og_invalidate_cache($gids)
   *
   * @param $consumers associative array in form of LdapAuthorizationConsumerAbstract::populateConsumersFromConsumerIds
   *
   *
   */
  public function flushRelatedCaches($consumers = NULL) { }

  /**
   * @param string $op 'grant' or 'revoke' signifying what to do with the $consumer_ids
   * @param drupal user object $object
   * @param array $user_auth_data is array specific to this consumer_type.  Stored at $user->data['ldap_authorizations'][<consumer_type>]
   * @param $consumers as associative array in form of LdapAuthorizationConsumerAbstract::populateConsumersFromConsumerIds
   * @param array $ldap_entry, when available user's ldap entry.
   * @param boolean $user_save indicates is user data array should be saved or not.  this depends on the implementation calling this function
   */

  protected function grantsAndRevokes($op, &$user, &$user_auth_data, $consumers, &$ldap_entry = NULL, $user_save = TRUE) {

    if (!is_array($user_auth_data)) {
      $user_auth_data = array();
    }

    $detailed_watchdog_log = variable_get('ldap_help_watchdog_detail', 0);
    $this->sortConsumerIds($op, $consumers);
    $results = array();
    $watchdog_tokens = array();
    $watchdog_tokens['%username'] = $user->name;
    $watchdog_tokens['%action'] = $op;
    $watchdog_tokens['%user_save'] = $user_save;
    $consumer_ids_log = array();
    $users_authorization_ids = $this->usersAuthorizations($user);
    $watchdog_tokens['%users_authorization_ids'] = join(', ', $users_authorization_ids);
    if ($detailed_watchdog_log) {
      watchdog('ldap_authorization', "on call of grantsAndRevokes: user_auth_data=" . print_r($user_auth_data, TRUE), $watchdog_tokens, WATCHDOG_DEBUG);
    }

    foreach ($consumers as $consumer_id => $consumer) {
      if ($detailed_watchdog_log) {
        watchdog('ldap_authorization', "consumer_id=$consumer_id, user_save=$user_save, op=$op", $watchdog_tokens, WATCHDOG_DEBUG);
      }
      $log = "consumer_id=$consumer_id, op=$op,";
      $user_has_authorization = in_array($consumer_id, $users_authorization_ids);
      $user_has_authorization_recorded = isset($user_auth_data[$consumer_id]);

      /** grants **/
      if ($op == 'grant') {
        if ($user_has_authorization && !$user_has_authorization_recorded) {
          // grant case 1: authorization id already exists for user, but is not ldap provisioned.  mark as ldap provisioned, but don't regrant
          $results[$consumer_id] = TRUE;
          $user_auth_data[$consumer_id] = array(
            'date_granted' => time(),
            'consumer_id_mixed_case' => $consumer_id,
          );
        }
        elseif (!$user_has_authorization && $consumer['exists']) {
          // grant case 2: consumer exists, but user is not member. grant authorization
          $results[$consumer_id] = $this->grantSingleAuthorization($user, $consumer_id, $consumer, $user_auth_data, $user_save);  // allow consuming module to add additional data to $user_auth_data
          $existing = empty($user_auth_data[$consumer_id]) ? array() : $user_auth_data[$consumer_id];
          $user_auth_data[$consumer_id] = $existing + array(
            'date_granted' => time(),
            'consumer_id_mixed_case' => $consumer_id,
          );
        }
        elseif ($consumer['exists'] !== TRUE) {
          // grant case 3: something is wrong. consumers should have been created before calling grantsAndRevokes
          $results[$consumer_id] = FALSE;
        }
        elseif ($consumer['exists'] === TRUE) {
          // grant case 4: consumer exists and user has authorization recorded. do nothing
          $results[$consumer_id] = TRUE;
        }
        else {
          // grant case 5: $consumer['exists'] has not been properly set before calling function
          $results[$consumer_id] = FALSE;
          watchdog('ldap_authorization', "grantsAndRevokes consumer[exists] not properly set. consumer_id=$consumer_id, op=$op, username=%username", $watchdog_tokens, WATCHDOG_ERROR);
        }
      }
      /** revokes **/
      elseif ($op == 'revoke') {

        $log .= "revoking existing consumer object, ";
        if ($user_has_authorization) {
          // revoke case 1: user has authorization, revoke it.  revokeSingleAuthorization will remove $user_auth_data[$consumer_id]
          $results[$consumer_id] = $this->revokeSingleAuthorization($user, $consumer_id, $consumer, $user_auth_data, $user_save);  // defer to default for $user_save param
          $log .= t(',result=') . (boolean)($results[$consumer_id]);
        }
        elseif ($user_has_authorization_recorded)  {
          // revoke case 2: user does not have authorization, but has record of it. remove record of it.
          unset($user_auth_data[$consumer_id]);
          $results[$consumer_id] = TRUE;
        }
        else {
          // revoke case 3: trying to revoke something that isn't there
          $results[$consumer_id] = TRUE;
        }

      }
      $consumer_ids_log[] = $log;
      if ($detailed_watchdog_log) {
        watchdog('ldap_authorization', "user_auth_data after consumer $consumer_id" . print_r($user_auth_data, TRUE), $watchdog_tokens, WATCHDOG_DEBUG);
      }

    }
    $watchdog_tokens['%consumer_ids_log'] = (count($consumer_ids_log)) ? join('<hr/>', $consumer_ids_log) : t('no actions');

    if ($user_save) {
      $user = user_load($user->uid, TRUE);
      $user_edit = $user->data;
      $user_edit['data']['ldap_authorizations'][$this->consumerType] = $user_auth_data;
      $user = user_save($user, $user_edit);
      $user_auth_data = $user->data['ldap_authorizations'][$this->consumerType];  // reload this.
    }
    $this->flushRelatedCaches($consumers);

    if ($detailed_watchdog_log) {
      watchdog('ldap_authorization', '%username:
        <hr/>LdapAuthorizationConsumerAbstract grantsAndRevokes() method log.  action=%action:<br/> %consumer_ids_log
        ',
        $watchdog_tokens, WATCHDOG_DEBUG);
    }

  }

  /**
   * @param drupal user object $user to have $consumer_id revoked
   * @param string lower case $consumer_id $consumer_id such as drupal role name, og group name, etc.
   * @param mixed $consumer.  depends on type of consumer.  Drupal roles are strings, og groups are ??
   * @param array $user_auth_data array of $user data specific to this consumer type.
   *   stored in $user->data['ldap_authorizations'][<consumer_type>] array
   * @param boolean $reset signifying if caches associated with $consumer_id should be invalidated.
   *
   * return boolen TRUE on success, FALSE on fail.  If user save is FALSE, the user object will
   *   not be saved and reloaded, so a returned TRUE may be misleading.
   *   $user_auth_data should have successfully revoked consumer id removed
   */

  public function revokeSingleAuthorization(&$user, $consumer_id, $consumer, &$user_auth_data, $user_save = FALSE, $reset = FALSE) {
     // method must be overridden
  }

  /**
   * @param stdClass $user as drupal user object to have $consumer_id granted
   * @param string lower case $consumer_id $consumer_id such as drupal role name, og group name, etc.
   * @param mixed $consumer.  depends on type of consumer.  Drupal roles are strings, og groups are ??
   * @param array $user_auth_data in form
   *   array('my drupal role' =>
   *     'date_granted' => 1351814718,
   *     'consumer_id_mixed_case' => 'My Drupal Role',
   *     )
   * @param boolean $reset signifying if caches associated with $consumer_id should be invalidated.
   *  @return boolean FALSE on failure or TRUE on success
   */
  public function grantSingleAuthorization(&$user, $consumer_id, $consumer, &$user_auth_data, $user_save = FALSE, $reset = FALSE) {
     // method must be overridden
  }

  /**
	 * Return all user consumer ids
	 *   regardless of it they were granted by this module
	 *
	 * @param user object $user
	 * @return array of consumer ids such as array('3-2','7-2'), array('admin','user_admin')
	 */

  public function usersAuthorizations(&$user) {
    // method must be overridden
  }

  /**
   * put authorization ids in displayable format
   */
  public function convertToFriendlyAuthorizationIds($authorizations) {
    return $authorizations;
  }

  /**
  * @param drupal user object $user to have $consumer_id granted
  * @param string lower case $consumer_id $consumer_id such as drupal role name, og group name, etc.
  * @param mixed $consumer.  depends on type of consumer.  Drupal roles are strings, og groups are ??
  *
  * return boolen TRUE on success, FALSE on fail.  If user save is FALSE, the user object will
  *   not be saved and reloaded, so a returned TRUE may be misleading.
  */
  public function createSingleAuthorization(&$user, $consumer_id, $consumer, &$user_auth_data) {
     // method must be overridden
  }

  /**
  * @param drupal user object $user
  * @param string lowercase $consumer_id such as drupal role name, og group name, etc.
  *
  * @return boolean if an ldap_authorization_* module granted the authorization id
  */
  public function hasLdapGrantedAuthorization(&$user, $consumer_id) {
    return (!empty($user->data['ldap_authorizations'][$this->consumerType][$consumer_id]));
  }

  /**
   * NOTE this is in mixed case, since we must rely on whatever module is storing
   * the authorization id
   *
   * @param drupal user object $user
   * @param string lowercase case $consumer_id such as drupal role name, og group name, etc.
   *
   * @return param boolean is user has authorization id, regardless of what module granted it.
   */
  public function hasAuthorization(&$user, $consumer_id) {
    return @in_array($consumer_id, $this->usersAuthorizations($user));
  }

  /**
	 * Validate authorization mappings on LDAP Authorization OG Admin form.
	 *
	 * @param array $mapping single mapping in format generated in normalizeMappings method
	 * @param array $form_values from authorization configuration form
	 * @param boolean $clear_cache
	 *
	 * @return array of form array($message_type, $message_text) where message type is status, warning, or error
	 *   and $message_text is what the user should see.
	 *
	 */

  public function validateAuthorizationMappingTarget($mapping, $form_values = NULL, $clear_cache = FALSE) {
    $message_type = NULL;
    $message_text = NULL;
    return array($message_type, $message_text);
  }


}
