<?php

/**
 * @file
 * This class represents a ldap_user module's configuration
 * It is extended by LdapUserConfAdmin for configuration and other admin functions
 */

require_once('ldap_user.module');

class LdapUserConf {

  /**
   * server providing Drupal account provisioning
   *
   * @var string
   *
   * @see LdapServer::sid
   */
  public $drupalAcctProvisionServer = LDAP_USER_NO_SERVER_SID;

  /**
   * server providing LDAP entry provisioning
   *
   * @var string
   *
   * @see LdapServer::sid
   */
  public $ldapEntryProvisionServer = LDAP_USER_NO_SERVER_SID;

  /**
   * Associative array mapping synch directions to ldap server instances.
   *
   * @var array
   */
  public $provisionSidFromDirection = array(
    LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER => LDAP_USER_NO_SERVER_SID,
    LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY => LDAP_USER_NO_SERVER_SID,
  );

  /**
   * Array of events that trigger provisioning of Drupal Accounts
   * Valid constants are:
   *   LDAP_USER_DRUPAL_USER_PROV_ON_AUTHENTICATE
   *   LDAP_USER_DRUPAL_USER_PROV_ON_USER_UPDATE_CREATE
   *   LDAP_USER_DRUPAL_USER_PROV_ON_ALLOW_MANUAL_CREATE
   *
   * @var array
   */
  public $drupalAcctProvisionTriggers = array(LDAP_USER_DRUPAL_USER_PROV_ON_AUTHENTICATE, LDAP_USER_DRUPAL_USER_PROV_ON_USER_UPDATE_CREATE, LDAP_USER_DRUPAL_USER_PROV_ON_ALLOW_MANUAL_CREATE);

  /**
   * Array of events that trigger provisioning of LDAP Entries
   * Valid constants are:
   *   LDAP_USER_LDAP_ENTRY_PROV_ON_USER_UPDATE_CREATE
   *   LDAP_USER_LDAP_ENTRY_PROV_ON_AUTHENTICATE
   *   LDAP_USER_LDAP_ENTRY_DELETE_ON_USER_DELETE
   *
   * @var array
   */
  public $ldapEntryProvisionTriggers = array();

  /**
   * server providing LDAP entry provisioning
   *
   * @var string
   *
   * @see LdapServer::sid
   */
  public $userConflictResolve = LDAP_USER_CONFLICT_RESOLVE_DEFAULT;

  /**
   * drupal account creation model
   *
   * @var int
   *   LDAP_USER_ACCT_CREATION_LDAP_BEHAVIOR   /admin/config/people/accounts/settings do not affect "LDAP Associated" Drupal accounts.
   *   LDAP_USER_ACCT_CREATION_USER_SETTINGS_FOR_LDAP  use Account creation settings at /admin/config/people/accounts/settings
   */
  public $acctCreation = LDAP_USER_ACCT_CREATION_LDAP_BEHAVIOR_DEFAULT;

  /**
   * has current object been saved to the database?
   *
   * @var boolean
   *
   */
  public $inDatabase = FALSE;

  /**
   * what to do when an ldap provisioned username conflicts with existing drupal user?
   *
   * @var int
   *   LDAP_USER_CONFLICT_LOG - log the conflict
   *   LDAP_USER_CONFLICT_RESOLVE - LDAP associate the existing drupal user
   *
   */
  public $manualAccountConflict = LDAP_USER_MANUAL_ACCT_CONFLICT_REJECT;

  public $setsLdapPassword = TRUE; // @todo default to FALSE and check for mapping to set to true

  public $loginConflictResolve = FALSE;

  public $disableAdminPasswordField = FALSE;
  /**
   * array of field synch mappings provided by all modules (via hook_ldap_user_attrs_list_alter())
   * array of the form: array(
   * LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER | LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY => array(
   *   <server_id> => array(
   *     'sid' => <server_id> (redundant)
   *     'ldap_attr' => e.g. [sn]
   *     'user_attr'  => e.g. [field.field_user_lname] (when this value is set to 'user_tokens', 'user_tokens' value is used.)
   *     'user_tokens' => e.g. [field.field_user_lname], [field.field_user_fname]
   *     'convert' => 1|0 boolean indicating need to covert from binary
   *     'direction' => LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER | LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY (redundant)
   *     'config_module' => 'ldap_user'
   *     'prov_module' => 'ldap_user'
   *     'enabled' => 1|0 boolean
   *      prov_events' => array( of LDAP_USER_EVENT_* constants indicating during which synch actions field should be synched)
   *         - four permutations available
   *            to ldap:   LDAP_USER_EVENT_CREATE_LDAP_ENTRY,  LDAP_USER_EVENT_SYNCH_TO_LDAP_ENTRY,
   *            to drupal: LDAP_USER_EVENT_CREATE_DRUPAL_USER, LDAP_USER_EVENT_SYNCH_TO_DRUPAL_USER
   *    )
   *  )
   */
  public $synchMapping = NULL; // array of field synching directions for each operation.  should include ldapUserSynchMappings
  // keyed on direction => property, ldap, or field token such as '[field.field_lname] with brackets in them.

  /**
  * synch mappings configured in ldap user module (not in other modules)
  *   array of the form: array(
    LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER | LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY => array(
      'sid' => <server_id> (redundant)
      'ldap_attr' => e.g. [sn]
      'user_attr'  => e.g. [field.field_user_lname] (when this value is set to 'user_tokens', 'user_tokens' value is used.)
      'user_tokens' => e.g. [field.field_user_lname], [field.field_user_fname]
      'convert' => 1|0 boolean indicating need to covert from binary
      'direction' => LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER | LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY (redundant)
      'config_module' => 'ldap_user'
      'prov_module' => 'ldap_user'
      'enabled' => 1|0 boolean
       prov_events' => array( of LDAP_USER_EVENT_* constants indicating during which synch actions field should be synched)
          - four permutations available
             to ldap:   LDAP_USER_EVENT_CREATE_LDAP_ENTRY,  LDAP_USER_EVENT_SYNCH_TO_LDAP_ENTRY,
             to drupal: LDAP_USER_EVENT_CREATE_DRUPAL_USER, LDAP_USER_EVENT_SYNCH_TO_DRUPAL_USER
      )
    )
  )
  */
  public $ldapUserSynchMappings = NULL;  //
  // keyed on property, ldap, or field token such as '[field.field_lname] with brackets in them.
  public $detailedWatchdog = FALSE;
  public $provisionsDrupalAccountsFromLdap = FALSE;
  public $provisionsLdapEntriesFromDrupalUsers = FALSE;

  // what should be done with ldap provisioned accounts that no longer have associated drupal accounts.
  public $orphanedDrupalAcctBehavior = 'ldap_user_orphan_email';
   /** options are partially derived from user module account cancel options:
    *
    'ldap_user_orphan_do_not_check' => Do not check for orphaned Drupal accounts.)
    'ldap_user_orphan_email' => Perform no action, but email list of orphaned accounts. (All the other options will send email summaries also.)
    'user_cancel_block' => Disable the account and keep its content.
    'user_cancel_block_unpublish' => Disable the account and unpublish its content.
    'user_cancel_reassign' => Delete the account and make its content belong to the Anonymous user.
    'user_cancel_delete' => Delete the account and its content.
    */

  public $orphanedCheckQty = 100;

// public $wsKey = NULL;
//  public $wsEnabled = 0;
//  public $wsUserIps = array();

  public $provisionsLdapEvents = array();
  public $provisionsDrupalEvents = array();

  public $saveable = array(
    'drupalAcctProvisionServer',
    'ldapEntryProvisionServer',
    'drupalAcctProvisionTriggers',
    'ldapEntryProvisionTriggers',
    'orphanedDrupalAcctBehavior',
    'orphanedCheckQty',
    'userConflictResolve',
    'manualAccountConflict',
    'acctCreation',
    'ldapUserSynchMappings',
    'disableAdminPasswordField',
  );
// 'wsKey','wsEnabled','wsUserIps',
  function __construct() {
    $this->load();

    $this->provisionSidFromDirection[LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER] = $this->drupalAcctProvisionServer;
    $this->provisionSidFromDirection[LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY] = $this->ldapEntryProvisionServer;

    $this->provisionsLdapEvents = array(
      LDAP_USER_EVENT_CREATE_LDAP_ENTRY => t('On LDAP Entry Creation'),
      LDAP_USER_EVENT_SYNCH_TO_LDAP_ENTRY => t('On Synch to LDAP Entry'),
      );

    $this->provisionsDrupalEvents = array(
      LDAP_USER_EVENT_CREATE_DRUPAL_USER => t('On Drupal User Creation'),
      LDAP_USER_EVENT_SYNCH_TO_DRUPAL_USER => t('On Synch to Drupal User'),
      );

    $this->provisionsDrupalAccountsFromLdap = (
      $this->drupalAcctProvisionServer &&
      $this->drupalAcctProvisionServer &&
      (count(array_filter(array_values($this->drupalAcctProvisionTriggers))) > 0)
    );

    $this->provisionsLdapEntriesFromDrupalUsers = (
      $this->ldapEntryProvisionServer
      && $this->ldapEntryProvisionServer
      && (count(array_filter(array_values($this->ldapEntryProvisionTriggers))) > 0)
      );

    $this->setSynchMapping(TRUE);
    $this->detailedWatchdog = variable_get('ldap_help_watchdog_detail', 0);
  }

  function load() {

    if ($saved = variable_get("ldap_user_conf", FALSE)) {
      $this->inDatabase = TRUE;
      foreach ($this->saveable as $property) {
        if (isset($saved[$property])) {
          $this->{$property} = $saved[$property];
        }
      }
    }
    else {
      $this->inDatabase = FALSE;
    }
    // determine account creation configuration
    $user_register = variable_get('user_register', USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL);
    if ($this->acctCreation == LDAP_USER_ACCT_CREATION_LDAP_BEHAVIOR_DEFAULT || $user_register == USER_REGISTER_VISITORS) {
      $this->createLDAPAccounts = TRUE;
      $this->createLDAPAccountsAdminApproval = FALSE;
    }
    elseif ($user_register == USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL) {
      $this->createLDAPAccounts = FALSE;
      $this->createLDAPAccountsAdminApproval = TRUE;
    }
    else {
      $this->createLDAPAccounts = FALSE;
      $this->createLDAPAccountsAdminApproval = FALSE;
    }
  }

  /**
   * Destructor Method
   */
  function __destruct() { }


  /**
   * Util to fetch mappings for a given direction
   *
   * @param string $sid
   *   The server id
   * @param string $direction LDAP_USER_PROV_DIRECTION_* constant
   * @param array $prov_events
   *
   * @return array/bool
   *   Array of mappings (may be empty array)
  */
  public function getSynchMappings($direction = LDAP_USER_PROV_DIRECTION_ALL, $prov_events = NULL) {
    if (!$prov_events) {
      $prov_events = ldap_user_all_events();
    }

    $mappings = array();
    if ($direction == LDAP_USER_PROV_DIRECTION_ALL) {
      $directions = array(LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER, LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY);
    }
    else {
      $directions = array($direction);
    }
    foreach ($directions as $direction) {
      if (!empty($this->ldapUserSynchMappings[$direction])) {
        foreach ($this->ldapUserSynchMappings[$direction] as $attribute => $mapping) {
          if (!empty($mapping['prov_events'])) {
            $result = count(array_intersect($prov_events, $mapping['prov_events']));
            if ($result) {
              $mappings[$attribute] = $mapping;
            }
          }
        }
      }
    }
    return $mappings;
  }

  public function isDrupalAcctProvisionServer($sid) {
    if (!$sid || !$this->drupalAcctProvisionServer) {
      return FALSE;
    }
    elseif ($this->ldapEntryProvisionServer == $sid) {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  public function isLdapEntryProvisionServer($sid) {
    if (!$sid || !$this->ldapEntryProvisionServer) {
      return FALSE;
    }
    elseif ($this->ldapEntryProvisionServer == $sid) {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Util to fetch attributes required for this user conf, not other modules.
   *
   * @param enum $direction LDAP_USER_PROV_DIRECTION_* constants
   * @param string $ldap_context
   *
  */
  public function getLdapUserRequiredAttributes($direction = LDAP_USER_PROV_DIRECTION_ALL, $ldap_context = NULL) {

    $attributes_map = array();
    $required_attributes = array();
    if ($this->drupalAcctProvisionServer) {
      $prov_events = $this->ldapContextToProvEvents($ldap_context);
      $attributes_map = $this->getSynchMappings($direction, $prov_events);
      $required_attributes = array();
      foreach ($attributes_map as $detail) {
        if (count(array_intersect($prov_events, $detail['prov_events']))) {
          // Add the attribute to our array.
          if ($detail['ldap_attr']) {
            ldap_servers_token_extract_attributes($required_attributes,  $detail['ldap_attr']);
          }
        }
      }
    }
    return $required_attributes;
  }

/**
 * converts the more general ldap_context string to its associated ldap user event
 */

  public function ldapContextToProvEvents($ldap_context = NULL) {

    switch ($ldap_context) {

      case 'ldap_user_prov_to_drupal':
        $result = array(LDAP_USER_EVENT_SYNCH_TO_DRUPAL_USER, LDAP_USER_EVENT_CREATE_DRUPAL_USER, LDAP_USER_EVENT_LDAP_ASSOCIATE_DRUPAL_ACCT);
        break;

      case 'ldap_user_prov_to_ldap':
        $result = array(LDAP_USER_EVENT_SYNCH_TO_LDAP_ENTRY, LDAP_USER_EVENT_CREATE_LDAP_ENTRY);
        break;

      default:
        $result = ldap_user_all_events();

    }

    return $result;

  }


/**
 * converts the more general ldap_context string to its associated ldap user prov direction
 */

  public function ldapContextToProvDirection($ldap_context = NULL) {

    switch ($ldap_context) {

      case 'ldap_user_prov_to_drupal':
        $result = LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER;
        break;

      case 'ldap_user_prov_to_ldap':
      case 'ldap_user_delete_drupal_user':
        $result = LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY;
        break;

      // provisioning is can hapen in both directions in most contexts
      case 'ldap_user_insert_drupal_user':
      case 'ldap_user_update_drupal_user':
      case 'ldap_authentication_authenticate':
      case 'ldap_user_insert_drupal_user':
      case 'ldap_user_disable_drupal_user':
        $result = LDAP_USER_PROV_DIRECTION_ALL;
        break;

      default:
        $result = LDAP_USER_PROV_DIRECTION_ALL;

    }

    return $result;
  }

  /**
    derive mapping array from ldap user configuration and other configurations.
    if this becomes a resource hungry function should be moved to ldap_user functions
    and stored with static variable. should be cached also.

    this should be cached and modules implementing ldap_user_synch_mapping_alter
    should know when to invalidate cache.

   */

  function setSynchMapping($reset = TRUE) {  // @todo change default to false after development
    $synch_mapping_cache = cache_get('ldap_user_synch_mapping');
    if (!$reset && $synch_mapping_cache) {
      $this->synchMapping = $synch_mapping_cache->data;
    }
    else {
      $available_user_attrs = array();
      foreach (array(LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER, LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY) as $direction) {
        $sid = $this->provisionSidFromDirection[$direction];
        $available_user_attrs[$direction] = array();
        $ldap_server = ($sid) ? ldap_servers_get_servers($sid, NULL, TRUE) : FALSE;

        $params = array(
          'ldap_server' => $ldap_server,
          'ldap_user_conf' => $this,
          'direction' => $direction,
        );

        drupal_alter('ldap_user_attrs_list', $available_user_attrs[$direction], $params);
      }
    }
    $this->synchMapping = $available_user_attrs;

    cache_set('ldap_user_synch_mapping',  $this->synchMapping);
  }

  /**
   * given a $prov_event determine if ldap user configuration supports it.
   *   this is overall, not per field synching configuration
   *
   * @param enum $direction LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER or LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY
   *
   * @param enum $prov_event
   *   LDAP_USER_EVENT_SYNCH_TO_DRUPAL_USER, LDAP_USER_EVENT_CREATE_DRUPAL_USER
   *   LDAP_USER_EVENT_SYNCH_TO_LDAP_ENTRY LDAP_USER_EVENT_CREATE_LDAP_ENTRY
   *   LDAP_USER_EVENT_LDAP_ASSOCIATE_DRUPAL_ACCT
   *   LDAP_USER_EVENT_ALL
   *
   * @param enum $action 'synch', 'provision', 'delete_ldap_entry', 'delete_drupal_entry', 'cancel_drupal_entry'
   * @return boolean
   */

  public function provisionEnabled($direction, $provision_trigger) {
    $result = FALSE;

    if ($direction == LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY) {

      if (!$this->ldapEntryProvisionServer) {
        $result = FALSE;
      }
      else {
        $result = in_array($provision_trigger, $this->ldapEntryProvisionTriggers);
      }

    }
    elseif ($direction == LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER) {
      if (!$this->drupalAcctProvisionServer) {
        $result = FALSE;
      }
      else {
        $result = in_array($provision_trigger, $this->drupalAcctProvisionTriggers);
      }
    }

    return $result;
  }

 /**
   * given a drupal account, provision an ldap entry if none exists.  if one exists do nothing
   *
   * @param object $account drupal account object with minimum of name property
   * @param array $ldap_user as prepopulated ldap entry.  usually not provided
   *
   * @return array of form:
   *     array('status' => 'success', 'fail', or 'conflict'),
   *     array('ldap_server' => ldap server object),
   *     array('proposed' => proposed ldap entry),
   *     array('existing' => existing ldap entry),
   *     array('description' = > blah blah)
   *
   */

  public function provisionLdapEntry($account, $ldap_user = NULL, $test_query = FALSE) {
    $watchdog_tokens = array();
    $result = array(
      'status' => NULL,
      'ldap_server' => NULL,
      'proposed' => NULL,
      'existing' => NULL,
      'description' => NULL,
    );

    if (is_scalar($account)) {
      $username = $account;
      $account = new stdClass();
      $account->name = $username;
    }

    list($account, $user_entity) = ldap_user_load_user_acct_and_entity($account->name);

    if (is_object($account) && property_exists($account, 'uid') && $account->uid == 1) {
      $result['status'] = 'fail';
      $result['error_description'] = 'can not provision drupal user 1';
      return $result; // do not provision or synch user 1
    }

    if ($account == FALSE || $account->uid == 0) {
      $result['status'] = 'fail';
      $result['error_description'] = 'can not provision ldap user unless corresponding drupal account exists first.';
      return $result;
    }

    if (!$this->ldapEntryProvisionServer || !$this->ldapEntryProvisionServer) {
      $result['status'] = 'fail';
      $result['error_description'] = 'no provisioning server enabled';
      return $result;
    }

    $ldap_server = ldap_servers_get_servers($this->ldapEntryProvisionServer, NULL, TRUE);
    $params = array(
      'direction' => LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY,
      'prov_events' => array(LDAP_USER_EVENT_CREATE_LDAP_ENTRY),
      'module' => 'ldap_user',
      'function' => 'provisionLdapEntry',
      'include_count' => FALSE,
    );

    list($proposed_ldap_entry, $error) = $this->drupalUserToLdapEntry($account, $ldap_server, $params, $ldap_user);
    $proposed_dn = (is_array($proposed_ldap_entry) && isset($proposed_ldap_entry['dn']) && $proposed_ldap_entry['dn']) ? $proposed_ldap_entry['dn'] : NULL;
    $proposed_dn_lcase = drupal_strtolower($proposed_dn);
    $existing_ldap_entry = ($proposed_dn) ? $ldap_server->dnExists($proposed_dn, 'ldap_entry') : NULL;

    if ($error == LDAP_USER_PROV_RESULT_NO_PWD) {
      $result['status'] = 'fail';
      $result['description'] = 'Can not provision ldap account without user provided password.';
      $result['existing'] = $existing_ldap_entry;
      $result['proposed'] = $proposed_ldap_entry;
      $result['ldap_server'] = $ldap_server;
    }
    elseif (!$proposed_dn) {
      $result['status'] = 'fail';
      $result['description'] = t('failed to derive dn and or mappings');
      return $result;
    }
    elseif ($existing_ldap_entry) {
      $result['status'] = 'conflict';
      $result['description'] = 'can not provision ldap entry because exists already';
      $result['existing'] = $existing_ldap_entry;
      $result['proposed'] = $proposed_ldap_entry;
      $result['ldap_server'] = $ldap_server;
    }
    elseif ($test_query) {
      $result['status'] = 'fail';
      $result['description'] = 'not created because flagged as test query';
      $result['proposed'] = $proposed_ldap_entry;
      $result['ldap_server'] = $ldap_server;
    }
    else {
      // stick $proposed_ldap_entry in $ldap_entries array for drupal_alter call
      $ldap_entries = array($proposed_dn_lcase => $proposed_ldap_entry);
      $context = array(
        'action' => 'add',
        'corresponding_drupal_data' => array($proposed_dn_lcase => $account),
        'corresponding_drupal_data_type' => 'user',
      );
      drupal_alter('ldap_entry_pre_provision', $ldap_entries, $ldap_server, $context);
      // remove altered $proposed_ldap_entry from $ldap_entries array
      $proposed_ldap_entry = $ldap_entries[$proposed_dn_lcase];

      $ldap_entry_created = $ldap_server->createLdapEntry($proposed_ldap_entry, $proposed_dn);
      if ($ldap_entry_created) {
        module_invoke_all('ldap_entry_post_provision', $ldap_entries, $ldap_server, $context);
        $result['status'] = 'success';
        $result['description'] = 'ldap account created';
        $result['proposed'] = $proposed_ldap_entry;
        $result['created'] = $ldap_entry_created;
        $result['ldap_server'] = $ldap_server;

        // need to store <sid>|<dn> in ldap_user_prov_entries field, which may contain more than one
        $ldap_user_prov_entry = $ldap_server->sid . '|' . $proposed_ldap_entry['dn'];
        if (!isset($user_entity->ldap_user_prov_entries[LANGUAGE_NONE])) {
          $user_entity->ldap_user_prov_entries = array(LANGUAGE_NONE => array());
        }
        $ldap_user_prov_entry_exists = FALSE;
        foreach ($user_entity->ldap_user_prov_entries[LANGUAGE_NONE] as $i => $field_value_instance) {
          if ($field_value_instance == $ldap_user_prov_entry) {
            $ldap_user_prov_entry_exists = TRUE;
          }
        }
        if (!$ldap_user_prov_entry_exists) {
          $user_entity->ldap_user_prov_entries[LANGUAGE_NONE][] = array(
            'value' =>  $ldap_user_prov_entry,
          );

          // Save the field without calling user_save()
          field_attach_presave('user', $user_entity);
          field_attach_update('user', $user_entity);
        }

      }
      else {
        $result['status'] = 'fail';
        $result['proposed'] = $proposed_ldap_entry;
        $result['created'] = $ldap_entry_created;
        $result['ldap_server'] = $ldap_server;
        $result['existing'] = NULL;
      }
    }

    $tokens = array(
      '%dn' => isset($result['proposed']['dn']) ? $result['proposed']['dn'] : NULL,
      '%sid' => (isset($result['ldap_server']) && $result['ldap_server']) ? $result['ldap_server']->sid : 0,
      '%username' => @$account->name,
      '%uid' => @$account->uid,
      '%description' => @$result['description'],
    );
    if (!$test_query && isset($result['status'])) {
      if ($result['status'] == 'success') {
        if ($this->detailedWatchdog) {
          watchdog('ldap_user', 'LDAP entry on server %sid created dn=%dn.  %description. username=%username, uid=%uid', $tokens, WATCHDOG_INFO);
        }
      }
      elseif ($result['status'] == 'conflict') {
        if ($this->detailedWatchdog) {
          watchdog('ldap_user', 'LDAP entry on server %sid not created because of existing ldap entry. %description. username=%username, uid=%uid', $tokens, WATCHDOG_WARNING);
        }
      }
      elseif ($result['status'] == 'fail') {
        watchdog('ldap_user', 'LDAP entry on server %sid not created because error.  %description. username=%username, uid=%uid', $tokens, WATCHDOG_ERROR);
      }
    }
    return $result;
  }


  /**
   * given a drupal account, synch to related ldap entry
   *
   * @param drupal user object $account.  Drupal user object
   * @param array $user_edit.  Edit array for user_save.  generally null unless user account is being created or modified in same synching
   * @param array $ldap_user.  current ldap data of user. @see README.developers.txt for structure
   *
   * @return TRUE on success or FALSE on fail.
   */

  public function synchToLdapEntry($account, $user_edit = NULL, $ldap_user =  array(), $test_query = FALSE) {

    if (is_object($account) && property_exists($account, 'uid') && $account->uid == 1) {
      return FALSE; // do not provision or synch user 1
    }

    $watchdog_tokens = array();
    $result = FALSE;
    $proposed_ldap_entry = FALSE;

    if ($this->ldapEntryProvisionServer) {
      $ldap_server = ldap_servers_get_servers($this->ldapEntryProvisionServer, NULL, TRUE);

      $params = array(
        'direction' => LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY,
        'prov_events' => array(LDAP_USER_EVENT_SYNCH_TO_LDAP_ENTRY),
        'module' => 'ldap_user',
        'function' => 'synchToLdapEntry',
        'include_count' => FALSE,
      );

      list($proposed_ldap_entry, $error) = $this->drupalUserToLdapEntry($account, $ldap_server, $params, $ldap_user);
      if ($error != LDAP_USER_PROV_RESULT_NO_ERROR) {
        $result = FALSE;
      }
      elseif (is_array($proposed_ldap_entry) && isset($proposed_ldap_entry['dn'])) {
        $existing_ldap_entry = $ldap_server->dnExists($proposed_ldap_entry['dn'], 'ldap_entry');
        $attributes = array(); // this array represents attributes to be modified; not comprehensive list of attributes
        foreach ($proposed_ldap_entry as $attr_name => $attr_values) {
          if ($attr_name != 'dn') {
            if (isset($attr_values['count'])) {
              unset($attr_values['count']);
            }
            if (count($attr_values) == 1) {
              $attributes[$attr_name] = $attr_values[0];
            }
            else {
              $attributes[$attr_name] = $attr_values;
            }
          }
        }

        if ($test_query) {
          $proposed_ldap_entry = $attributes;
          $result = array(
            'proposed' => $proposed_ldap_entry,
            'server' => $ldap_server,
          );
        }
        else {
          // stick $proposed_ldap_entry in $ldap_entries array for drupal_alter call
          $proposed_dn_lcase = drupal_strtolower($proposed_ldap_entry['dn']);
          $ldap_entries = array($proposed_dn_lcase => $attributes);
          $context = array(
            'action' => 'update',
            'corresponding_drupal_data' => array($proposed_dn_lcase => $attributes),
            'corresponding_drupal_data_type' => 'user',
          );
          drupal_alter('ldap_entry_pre_provision', $ldap_entries, $ldap_server, $context);
          // remove altered $proposed_ldap_entry from $ldap_entries array
          $attributes = $ldap_entries[$proposed_dn_lcase];
          $result = $ldap_server->modifyLdapEntry($proposed_ldap_entry['dn'], $attributes);
          if ($result) { // success
            module_invoke_all('ldap_entry_post_provision', $ldap_entries, $ldap_server, $context);
          }
        }
      }
      else { // failed to get acceptable proposed ldap entry
        $result = FALSE;
      }
    }

    $tokens = array(
      '%dn' => isset($proposed_ldap_entry['dn']) ? $proposed_ldap_entry['dn'] : NULL,
      '%sid' => $this->ldapEntryProvisionServer,
      '%username' => $account->name,
      '%uid' => ($test_query || !property_exists($account, 'uid')) ? '' : $account->uid,
    );

    if ($result) {
      watchdog('ldap_user', 'LDAP entry on server %sid synched dn=%dn. username=%username, uid=%uid', $tokens, WATCHDOG_INFO);
    }
    else {
      watchdog('ldap_user', 'LDAP entry on server %sid not synched because error. username=%username, uid=%uid', $tokens, WATCHDOG_ERROR);
    }

    return $result;

  }

  /**
   * given a drupal account, query ldap and get all user fields and create user account
   *
   * @param array $account drupal account array with minimum of name
   * @param array $user_edit drupal edit array in form user_save($account, $user_edit) would take,
   *   generally empty unless overriding synchToDrupalAccount derived values
   * @param array $ldap_user as user's ldap entry.  passed to avoid requerying ldap in cases where already present
   * @param boolean $save indicating if drupal user should be saved.  generally depends on where function is called from.
   *
   * @return result of user_save() function is $save is true, otherwise return TRUE
   *   $user_edit data returned by reference
   *
   */
  public function synchToDrupalAccount($drupal_user, &$user_edit, $prov_event = LDAP_USER_EVENT_SYNCH_TO_DRUPAL_USER, $ldap_user = NULL,  $save = FALSE) {
    
    $debug = array(
      'account' => $drupal_user,
      'user_edit' => $user_edit,
      'ldap_user' => $ldap_user,
    );

    if (
        (!$ldap_user  && !isset($drupal_user->name)) ||
        (!$drupal_user && $save) ||
        ($ldap_user && !isset($ldap_user['sid']))
    ) {
       // should throw watchdog error also
      return FALSE;
    }

    if (!$ldap_user && $this->drupalAcctProvisionServer) {
      $ldap_user = ldap_servers_get_user_ldap_data($drupal_user->name, $this->drupalAcctProvisionServer, 'ldap_user_prov_to_drupal');
    }

    if (!$ldap_user) {
      return FALSE;
    }

    if ($this->drupalAcctProvisionServer) {
      $ldap_server = ldap_servers_get_servers($this->drupalAcctProvisionServer, NULL, TRUE);
      $this->entryToUserEdit($ldap_user, $user_edit, $ldap_server, LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER, array($prov_event));
    }

    if ($save) {
      $account = user_load($drupal_user->uid);
      $result = user_save($account, $user_edit, 'ldap_user');
      return $result;
    }
    else {
      return TRUE;
    }
  }


  /**
   * given a drupal account, delete user account
   *
   * @param string $username drupal account name
   * @return TRUE or FALSE.  FALSE indicates failed or action not enabled in ldap user configuration
   */
  public function deleteDrupalAccount($username) {
    $user = user_load_by_name($username);
    if (is_object($user)) {
      user_delete($user->uid);
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  /**
   * given a drupal account, find the related ldap entry.
   *
   * @param drupal user object $account
   *
   * @return FALSE or ldap entry
   */
  public function getProvisionRelatedLdapEntry($account, $prov_events = NULL) {
    if (!$prov_events) {
      $prov_events = ldap_user_all_events();
    }
    $sid = $this->ldapEntryProvisionServer; //
    if (!$sid) {
      return FALSE;
    }
    // $user_entity->ldap_user_prov_entries,
    $ldap_server = ldap_servers_get_servers($sid, NULL, TRUE);
    $params = array(
      'direction' => LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY,
      'prov_events' => $prov_events,
      'module' => 'ldap_user',
      'function' => 'getProvisionRelatedLdapEntry',
      'include_count' => FALSE,
      );
    list($proposed_ldap_entry, $error) = $this->drupalUserToLdapEntry($account, $ldap_server, $params);
    if (!(is_array($proposed_ldap_entry) && isset($proposed_ldap_entry['dn']) && $proposed_ldap_entry['dn'])) {
      return FALSE;
    }
    $ldap_entry = $ldap_server->dnExists($proposed_ldap_entry['dn'], 'ldap_entry', array());
    return $ldap_entry;

  }

  /**
   * given a drupal account, delete ldap entry that was provisioned based on it
   *   normally this will be 0 or 1 entry, but the ldap_user_provisioned_ldap_entries
   *   field attached to the user entity track each ldap entry provisioned
   *
   * @param object $account drupal account
   * @return TRUE or FALSE.  FALSE indicates failed or action not enabled in ldap user configuration
   */
  public function deleteProvisionedLdapEntries($account) {
    // determine server that is associated with user

    $boolean_result = FALSE;
    if (isset($account->ldap_user_prov_entries[LANGUAGE_NONE][0])) {
      foreach ($account->ldap_user_prov_entries[LANGUAGE_NONE] as $i => $field_instance) {
        $parts = explode('|', $field_instance['value']);
        if (count($parts) == 2) {

          list($sid, $dn) = $parts;
          $ldap_server = ldap_servers_get_servers($sid, NULL, TRUE);
          if (is_object($ldap_server) && $dn) {
            $boolean_result = $ldap_server->delete($dn);
            $tokens = array('%sid' => $sid, '%dn' => $dn, '%username' => $account->name, '%uid' => $account->uid);
            if ($boolean_result) {
              watchdog('ldap_user', 'LDAP entry on server %sid deleted dn=%dn. username=%username, uid=%uid', $tokens, WATCHDOG_INFO);
            }
            else {
              watchdog('ldap_user', 'LDAP entry on server %sid not deleted because error. username=%username, uid=%uid', $tokens, WATCHDOG_ERROR);
            }
          }
          else {
            $boolean_result = FALSE;
          }
        }
      }
    }
    return $boolean_result;

  }

/**
  *  populate ldap entry array for provisioning
  *
  * @param array $account drupal account
  * @param object $ldap_server
  * @param array $ldap_user ldap entry of user, returned by reference
  * @param array $params with the following key values:
  *    'ldap_context' =>
       'module' => module calling function, e.g. 'ldap_user'
       'function' => function calling function, e.g. 'provisionLdapEntry'
       'include_count' => should 'count' array key be included
       'direction' => LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY || LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER
  *
  * @return array(ldap entry, $result) in ldap extension array format.!THIS IS NOT THE ACTUAL LDAP ENTRY
  */

  function drupalUserToLdapEntry($account, $ldap_server, $params, $ldap_user_entry = NULL) {
    $provision = (isset($params['function']) && $params['function'] == 'provisionLdapEntry');
    $result = LDAP_USER_PROV_RESULT_NO_ERROR;
    if (!$ldap_user_entry) {
      $ldap_user_entry = array();
    }

    if (!is_object($account) || !is_object($ldap_server)) {
      return array(NULL, LDAP_USER_PROV_RESULT_BAD_PARAMS);
    }
    $watchdog_tokens = array(
      '%drupal_username' => $account->name,
    );
    $include_count = (isset($params['include_count']) && $params['include_count']);

    $direction = isset($params['direction']) ? $params['direction'] : LDAP_USER_PROV_DIRECTION_ALL;
    $prov_events = empty($params['prov_events']) ? ldap_user_all_events() : $params['prov_events'];

    $mappings = $this->getSynchMappings($direction, $prov_events);
    foreach ($mappings as $field_key => $field_detail) {
      list($ldap_attr_name, $ordinal, $conversion) = ldap_servers_token_extract_parts($field_key, TRUE);  //trim($field_key, '[]');
      $ordinal = (!$ordinal) ? 0 : $ordinal;
      if ($ldap_user_entry && isset($ldap_user_entry[$ldap_attr_name]) && is_array($ldap_user_entry[$ldap_attr_name]) && isset($ldap_user_entry[$ldap_attr_name][$ordinal]) ) {
        continue; // don't override values passed in;
      }

      $synched = $this->isSynched($field_key, $params['prov_events'], LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY);
      if ($synched) {
        $token = ($field_detail['user_attr'] == 'user_tokens') ? $field_detail['user_tokens'] : $field_detail['user_attr'];
        $value = ldap_servers_token_replace($account, $token, 'user_account');

        if (substr($token, 0, 10) == '[password.' && (!$value || $value == $token)) { // deal with empty/unresolved password
          if (!$provision) {
            continue; //don't overwrite password on synch if no value provided
          }
        }

        if ($ldap_attr_name == 'dn' && $value) {
          $ldap_user_entry['dn'] = $value;
        }
        elseif ($value) {
          if (!isset($ldap_user_entry[$ldap_attr_name]) || !is_array($ldap_user_entry[$ldap_attr_name])) {
            $ldap_user_entry[$ldap_attr_name] = array();
          }
          $ldap_user_entry[$ldap_attr_name][$ordinal] = $value;
          if ($include_count) {
            $ldap_user_entry[$ldap_attr_name]['count'] = count($ldap_user_entry[$ldap_attr_name]);
          }

        }

      }

    }

    /**
     * 4. call drupal_alter() to allow other modules to alter $ldap_user
     */

    drupal_alter('ldap_entry', $ldap_user_entry, $params);

    return array($ldap_user_entry, $result);

  }



   /**
   * given a drupal account, query ldap and get all user fields and save user account
   * (note: parameters are in odd order to match synchDrupalAccount handle)
   *
   * @param array $account drupal account object or null
   * @param array $user_edit drupal edit array in form user_save($account, $user_edit) would take.
   * @param array $ldap_user as user's ldap entry.  passed to avoid requerying ldap in cases where already present
   * @param boolean $save indicating if drupal user should be saved.  generally depends on where function is called from and if the
   *
   * @return result of user_save() function is $save is true, otherwise return TRUE on success or FALSE on any problem
   *   $user_edit data returned by reference
   *
   */

  public function provisionDrupalAccount($account = FALSE, &$user_edit, $ldap_user = NULL, $save = TRUE) {

    $watchdog_tokens = array();
    /**
     * @todo
     * -- add error catching for conflicts, conflicts should be checked before calling this function.
     *
     */

    if (!$account) {
      $account = new stdClass();
    }
    $account->is_new = TRUE;

    if (!$ldap_user && !isset($user_edit['name'])) {
      return FALSE;
    }

    if (!$ldap_user) {
      $watchdog_tokens['%username'] = $user_edit['name'];
      if ($this->drupalAcctProvisionServer) {
        $ldap_user = ldap_servers_get_user_ldap_data($user_edit['name'], $this->drupalAcctProvisionServer, 'ldap_user_prov_to_drupal');
      }
      if (!$ldap_user) {
        if ($this->detailedWatchdog) {
          watchdog('ldap_user', '%username : failed to find associated ldap entry for username in provision.', $watchdog_tokens, WATCHDOG_DEBUG);
        }
        return FALSE;
      }
    }

    if (!isset($user_edit['name']) && isset($account->name)) {
      $user_edit['name'] = $account->name;
      $watchdog_tokens['%username'] = $user_edit['name'];
    }
    //When using the multi-domain last authentication option
    //$ldap_server breaks beacause $this->drupalAcctProvisionServer is set on LDAP_USER_AUTH_SERVER_SID
    //So we need to check it's not the case before using ldap_servers_get_servers
    if ($this->drupalAcctProvisionServer && $this->drupalAcctProvisionServer != LDAP_USER_AUTH_SERVER_SID) {

      $ldap_server = ldap_servers_get_servers($this->drupalAcctProvisionServer, 'enabled', TRUE);  // $ldap_user['sid']

      $params = array(
        'account' => $account,
        'user_edit' => $user_edit,
        'prov_event' => LDAP_USER_EVENT_CREATE_DRUPAL_USER,
        'module' => 'ldap_user',
        'function' => 'provisionDrupalAccount',
        'direction' => LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER,
      );

      drupal_alter('ldap_entry', $ldap_user, $params);

      // look for existing drupal account with same puid.  if so update username and attempt to synch in current context
      $puid = $ldap_server->userPuidFromLdapEntry($ldap_user['attr']);
      $account2 = ($puid) ? $ldap_server->userUserEntityFromPuid($puid) : FALSE;

      if ($account2) { // synch drupal account, since drupal account exists
        // 1. correct username and authmap
        $this->entryToUserEdit($ldap_user, $user_edit, $ldap_server, LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER, array(LDAP_USER_EVENT_SYNCH_TO_DRUPAL_USER));
        $account = user_save($account2, $user_edit, 'ldap_user');
        user_set_authmaps($account, array("authname_ldap_user" => $user_edit['name']));
        // 2. attempt synch if appropriate for current context
        if ($account) {
          $account = $this->synchToDrupalAccount($account, $user_edit, LDAP_USER_EVENT_SYNCH_TO_DRUPAL_USER, $ldap_user, TRUE);
        }
        return $account;
      }
      else { // create drupal account
        $this->entryToUserEdit($ldap_user, $user_edit, $ldap_server, LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER, array(LDAP_USER_EVENT_CREATE_DRUPAL_USER));
        if ($save) {
          $watchdog_tokens = array('%drupal_username' =>  $user_edit['name']);
          if (empty($user_edit['name'])) {
            drupal_set_message(t('User account creation failed because of invalid, empty derived Drupal username.'), 'error');
            watchdog('ldap_user',
              'Failed to create Drupal account %drupal_username because drupal username could not be derived.',
              $watchdog_tokens,
              WATCHDOG_ERROR
            );
            return FALSE;
          }
          if (!isset($user_edit['mail']) || !$user_edit['mail']) {
            drupal_set_message(t('User account creation failed because of invalid, empty derived email address.'), 'error');
            watchdog('ldap_user',
              'Failed to create Drupal account %drupal_username because email address could not be derived by LDAP User module',
              $watchdog_tokens,
              WATCHDOG_ERROR
            );
            return FALSE;
          }
          if ($account_with_same_email = user_load_by_mail($user_edit['mail'])) {
            $watchdog_tokens['%email'] = $user_edit['mail'];
            $watchdog_tokens['%duplicate_name'] = $account_with_same_email->name;
            watchdog('ldap_user', 'LDAP user %drupal_username has email address
              (%email) conflict with a drupal user %duplicate_name', $watchdog_tokens, WATCHDOG_ERROR);
            drupal_set_message(t('Another user already exists in the system with the same email address. You should contact the system administrator in order to solve this conflict.'), 'error');
            return FALSE;
          }
          $account = user_save(NULL, $user_edit, 'ldap_user');
          if (!$account) {
            drupal_set_message(t('User account creation failed because of system problems.'), 'error');
          }
          else {
            user_set_authmaps($account, array('authname_ldap_user' => $account->name));
            ldap_user_ldap_provision_semaphore('drupal_created', 'set', $account->name);
          }
          return $account;
        }
        return TRUE;
      }
    }
  }

  /**
   * set ldap associations of a drupal account by altering user fields
   *
   * @param string $drupal_username
   *
   * @return boolean TRUE on success, FALSE on error or failure because of invalid user or ldap accounts
   *
   */
  function ldapAssociateDrupalAccount($drupal_username) {

    if ($this->drupalAcctProvisionServer) {
      $prov_events = array(LDAP_USER_EVENT_LDAP_ASSOCIATE_DRUPAL_ACCT);
      $ldap_server = ldap_servers_get_servers($this->drupalAcctProvisionServer, 'enabled', TRUE);  // $ldap_user['sid']
      $account = user_load_by_name($drupal_username);
      $ldap_user = ldap_servers_get_user_ldap_data($drupal_username, $this->drupalAcctProvisionServer, 'ldap_user_prov_to_drupal');
      if (!$account) {
        watchdog(
          'ldap_user',
          'Failed to LDAP associate drupal account %drupal_username because account not found',
          array('%drupal_username' => $drupal_username),
          WATCHDOG_ERROR
        );
        return FALSE;
      }
      elseif (!$ldap_user) {
        watchdog(
          'ldap_user',
          'Failed to LDAP associate drupal account %drupal_username because corresponding LDAP entry not found',
          array('%drupal_username' => $drupal_username),
          WATCHDOG_ERROR
        );
        return FALSE;
      }
      else {
        $user_edit = array();
        $user_edit['data']['ldap_user']['init'] = array(
          'sid'  => $ldap_user['sid'],
          'dn'   => $ldap_user['dn'],
          'mail'   => $account->mail,
        );
        $ldap_user_puid = $ldap_server->userPuidFromLdapEntry($ldap_user['attr']);
        if ($ldap_user_puid) {
          $user_edit['ldap_user_puid'][LANGUAGE_NONE][0]['value'] = $ldap_user_puid; //
        }
        $user_edit['ldap_user_puid_property'][LANGUAGE_NONE][0]['value'] = $ldap_server->unique_persistent_attr;
        $user_edit['ldap_user_puid_sid'][LANGUAGE_NONE][0]['value'] = $ldap_server->sid;
        $user_edit['ldap_user_current_dn'][LANGUAGE_NONE][0]['value'] = $ldap_user['dn'];
        $account = user_save($account, $user_edit, 'ldap_user');
        return (boolean)$account;
      }
    }
    else {
      return FALSE;
    }
  }

  /** populate $user edit array (used in hook_user_save, hook_user_update, etc)
   * ... should not assume all attribues are present in ldap entry
   *
   * @param array ldap entry $ldap_user
   * @param array $edit see hook_user_save, hook_user_update, etc
   * @param object $ldap_server
   * @param enum $direction
   * @param array $prov_events
   *
   */

  function entryToUserEdit($ldap_user, &$edit, $ldap_server, $direction = LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER, $prov_events = NULL) {

    // need array of user fields and which direction and when they should be synched.
    if (!$prov_events) {
      $prov_events = ldap_user_all_events();
    }
    $mail_synched = $this->isSynched('[property.mail]', $prov_events, $direction);
    if (!isset($edit['mail']) && $mail_synched) {
      $derived_mail = $ldap_server->userEmailFromLdapEntry($ldap_user['attr']);
      if ($derived_mail) {
        $edit['mail'] = $derived_mail;
      }
    }

    $drupal_username = $ldap_server->userUsernameFromLdapEntry($ldap_user['attr']);
		if ($this->isSynched('[property.picture]', $prov_events, $direction)){

			$picture = $ldap_server->userPictureFromLdapEntry($ldap_user['attr'], $drupal_username);

			if ($picture){
				$edit['picture'] = $picture;
				if(isset($picture->md5Sum)){
					$edit['data']['ldap_user']['init']['thumb5md'] = $picture->md5Sum;
				}
			}
		}

    if ($this->isSynched('[property.name]', $prov_events, $direction) && !isset($edit['name']) && $drupal_username) {
      $edit['name'] = $drupal_username;
    }

    if ($direction == LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER && in_array(LDAP_USER_EVENT_CREATE_DRUPAL_USER, $prov_events)) {
      $edit['mail'] = isset($edit['mail']) ? $edit['mail'] : $ldap_user['mail'];
      if (!isset($edit['pass'])) {
        $edit['pass'] = user_password(20);
        watchdog('ldap_user', '20 character random password generated for the %username account that has been created.', array('%username' => $drupal_username), WATCHDOG_INFO);
      }
      $edit['init'] = isset($edit['init']) ? $edit['init'] : $edit['mail'];
      $edit['status'] = isset($edit['status']) ? $edit['status'] : 1;
      $edit['signature'] = isset($edit['signature']) ? $edit['signature'] : '';

      $edit['data']['ldap_user']['init'] = array(
        'sid'  => $ldap_user['sid'],
        'dn'   => $ldap_user['dn'],
        'mail' => $edit['mail'],
      );
    }

    /*
     * Make sure the user account has the latest ldap_user settings
     * when syncing the profile.
     */
    if ($direction == LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER && in_array(LDAP_USER_EVENT_SYNCH_TO_DRUPAL_USER, $prov_events)) {
       $edit['data']['ldap_user']['init'] = array(
        'sid'  => $ldap_user['sid'],
        'dn'   => $ldap_user['dn'],
        'mail' => isset($edit['mail']) && !empty($edit['mail']) ? $edit['mail'] : $ldap_user['mail'],
      );
    }

    /**
     * basic $user ldap fields
     */
    if ($this->isSynched('[field.ldap_user_puid]', $prov_events, $direction)) {
      $ldap_user_puid = $ldap_server->userPuidFromLdapEntry($ldap_user['attr']);
      if ($ldap_user_puid) {
        $edit['ldap_user_puid'][LANGUAGE_NONE][0]['value'] = $ldap_user_puid; //
      }
    }
    if ($this->isSynched('[field.ldap_user_puid_property]', $prov_events, $direction)) {
      $edit['ldap_user_puid_property'][LANGUAGE_NONE][0]['value'] = $ldap_server->unique_persistent_attr;
    }
    if ($this->isSynched('[field.ldap_user_puid_sid]', $prov_events, $direction)) {
      $edit['ldap_user_puid_sid'][LANGUAGE_NONE][0]['value'] = $ldap_server->sid;
    }
    if ($this->isSynched('[field.ldap_user_current_dn]', $prov_events, $direction)) {
      $edit['ldap_user_current_dn'][LANGUAGE_NONE][0]['value'] = $ldap_user['dn'];
    }

    // Get any additional mappings.
    $mappings = $this->getSynchMappings($direction, $prov_events);

     // Loop over the mappings.
    foreach ($mappings as $user_attr_key => $field_detail) {

       // Make sure this mapping is relevant to the sync context.
      if (!$this->isSynched($user_attr_key, $prov_events, $direction)) {
        continue;
      }
       /**
        * if "convert from binary is selected" and no particular method is in token,
        * default to ldap_servers_binary() function
        */
      if ($field_detail['convert'] && strpos($field_detail['ldap_attr'], ';') === FALSE) {
        $field_detail['ldap_attr'] = str_replace(']', ';binary]', $field_detail['ldap_attr']);
      }
      $value = ldap_servers_token_replace($ldap_user['attr'], $field_detail['ldap_attr'], 'ldap_entry');
      list($value_type, $value_name, $value_instance) = ldap_servers_parse_user_attr_name($user_attr_key);

      // $value_instance not used, may have future use case

      // Are we dealing with a field?
      if ($value_type == 'field') {
        // Field api field - first we get the field.
        $field = field_info_field($value_name);
        // Then the columns for the field in the schema.
        $columns = array_keys($field['columns']);
        // Then we convert the value into an array if it's scalar.
        $values = $field['cardinality'] == 1 ? array($value) : (array) $value;

        $items = array();
        // Loop over the values and set them in our $items array.
        foreach ($values as $delta => $value) {
          if (isset($value)) {
            // We set the first column value only, this is consistent with
            // the Entity Api (@see entity_metadata_field_property_set).
            $items[$delta][$columns[0]] = $value;
          }
        }
        // Add them to our edited item.
        $edit[$value_name][LANGUAGE_NONE] = $items;
      }
      elseif ($value_type == 'property') {
        // Straight property.
        $edit[$value_name] = $value;
      }
    }

    // Allow other modules to have a say.

    drupal_alter('ldap_user_edit_user', $edit, $ldap_user, $ldap_server, $prov_events);
    if (isset($edit['name']) && $edit['name'] == '') {  // don't let empty 'name' value pass for user
      unset($edit['name']);
    }

  }
  /**
   * given configuration of synching, determine is a given synch should occur
   *
   * @param string $attr_token e.g. [property.mail], [field.ldap_user_puid_property]
   * @param object $ldap_server
   * @param array $prov_events e.g. array(LDAP_USER_EVENT_CREATE_DRUPAL_USER).  typically array with 1 element
   * @param scalar $direction LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER or LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY
   */

  public function isSynched($attr_token, $prov_events, $direction) {
    $result = (boolean)(
      isset($this->synchMapping[$direction][$attr_token]['prov_events']) &&
      count(array_intersect($prov_events, $this->synchMapping[$direction][$attr_token]['prov_events']))
    );
    if (!$result) {
      if (isset($this->synchMapping[$direction][$attr_token])) {
      }
      else {
      }
    }
    return $result;
  }

}
