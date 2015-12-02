<?php
// $Id: LdapAuthorizationConsumerAbstract.class.php,v 1.2.2.1 2011/02/08 20:05:41 johnbarclay Exp $

/**
 * @file
 * abstract class to represent an ldap_authorization consumer
 * such as drupal_role, og_group, etc.  each authorization comsumer
 * will extend this class with its own class named
 * LdapAuthorizationConsumer<consumer type> such as LdapAuthorizationConsumerDrupalRole
 *
 */

class LdapAuthorizationConsumerAbstract {

  public $name;  // e.g. drupal role, og group
  public $namePlural; // e.g. drupal roles, og groups
  public $shortName; // e.g. role, group
  public $shortNamePlural; // e.g. roles, groups
  public $description;
  public $consumerConf; // each consumer type has cosumer conf object
  public $consumerModule;
  public $testLink;
  public $editLink;

  protected $_availableConsumerIDs;


  /**
   * @property boolean $allowSynchBothDirections
   *
   *  Does this consumer module support synching in both directions?
   *
   */
  public $allowSynchBothDirections = FALSE;

   /**
   * @property boolean $allowConsumerObjectCreation
   *
   *  Does this consumer module support creating consumer objects
   * (drupal roles,  og groups, etc.)
   *
   */

  public $allowConsumerObjectCreation = FALSE;


  /**
   * default consumer conf property values for this consumer type.
   * Should be overridden by child classes as appropriate
   */

  public $onlyApplyToLdapAuthenticatedDefault = TRUE;
  public $useMappingsAsFilterDefault = TRUE;
  public $synchOnLogonDefault = TRUE;
  public $synchManuallyDefault = TRUE;
  public $revokeLdapProvisionedDefault = TRUE;
  public $regrantLdapProvisioned = TRUE;
  public $createConsumersDefault = TRUE;
  public $detailedWatchdogLog = FALSE;



   /**
   * @property array $defaultableConsumerConfProperties
   * properties a consumer may provide defaults for
   * should include every item in "default mapping property values" above
   */
  public $defaultableConsumerConfProperties = array(
      'onlyApplyToLdapAuthenticated',
      'useMappingsAsFilter',
      'synchOnLogon',
      'synchManually',
      'revokeLdapProvisioned',
      'regrantLdapProvisioned',
      'createConsumers'
      );


 /**
   * Constructor Method
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
    ldap_server_module_load_include('php', 'ldap_authorization', 'LdapAuthorizationConsumerConfAdmin.class');
    $this->consumerConf = new LdapAuthorizationConsumerConf($this);

  }


  /**
   * function to normalize mappings
   * should be overridden when mappings are not stored as map|authorization_id format
   * where authorization_id is the format returned by LdapAuthorizationConsumerAbstract::usersAuthorizations()
   *
   * for example ldap_authorization_og may store mapping target as:
   *   Campus Accounts|group-name=knitters,role-name=administrator member
   *
   *   but the target authorization_id format is in the form gid-rid such as 2-3
   */
  public function normalizeMappings($mappings) {
    return $mappings;
  }


  /**
   * get list of all authorization consumer ids available to a this authorization consumer.  For
   * example for drupal_roles, this would be an array of drupal roles such
   * as array('admin', 'author', 'reviewer',... ).  For organic groups in
   * might be all the names of organic groups.
   *
   * return array in form array(id1, id2, id3,...)
   *
   */
  public function availableConsumerIDs() {
     // method must be overridden
  }

  /**
   *
   * create authorization consumers
   *
   * @param array $creates an array of authorization consumer ids in form array(id1, id2, id3,...)
   *
   * return array in form array(id1, id2, id3,...) representing all
   *   existing consumer ids ($this->availableConsumerIDs())
   *
   */
  public function createConsumers($creates) {
    // method must be overridden
  }

  /**
   * grant authorizations to a user
   *
   * @param object $user drupal user object
   *
   * @param $consumer_ids string or array of strings that are authorization consumer ids
   *
   * @param array $ldap_entry is ldap data from ldap entry which drupal user is mapped to
   *
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

  public function authorizationGrant(&$user, &$user_auth_data, $consumer_ids, $ldap_entry = NULL, $user_save = TRUE) {
    $this->grantsAndRevokes('grant', $user, $user_auth_data, $consumer_ids, $ldap_entry, $user_save);
  }

  /**
   * revoke authorizations to a user
   *
   * @param object $user drupal user object
   *
   * @param $consumer_ids string or array of strings that are authorization consumer ids
   *
   * @param array $ldap_entry is ldap data from ldap entry which drupal user is mapped to
   *
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

  public function authorizationRevoke(&$user, &$user_auth_data, $consumer_ids, $ldap_entry, $user_save = TRUE) {
    $this->grantsAndRevokes('revoke', $user, $user_auth_data, $consumer_ids, $ldap_entry, $user_save);
  }

  /**
   * some authorization schemes such as organic groups, require a certain order.  implement this method
   * to sort consumer ids/authorization ids
   *
   * @param string $op 'grant' or 'revoke' signifying what to do with the $consumer_ids
   *
   * alters $consumer_ids by reference
   */
  public function sortConsumerIds($op, &$consumer_ids) { } // some


  /**
   * @param string $op 'grant' or 'revoke' signifying what to do with the $consumer_ids
   * @param drupal user object $object
   * @param array $user_auth_data is array specific to this consumer_type.  Stored at $user->data['ldap_authorizations'][<consumer_type>]
   * @param array $consumer_ids (aka $authorization_ids) e.g. array(id1, id2, ...)
   * @param array $ldap_entry, when available user's ldap entry.
   * @param boolean $user_save indicates is user data array should be saved or not.  this depends on the implementation calling this function
   */

  protected function grantsAndRevokes($op, &$user, &$user_auth_data, $consumer_ids, &$ldap_entry = NULL, $user_save = TRUE) {

    if (!is_array($user_auth_data)) {
      $user_auth_data = array();
    }

    $detailed_watchdog_log = variable_get('ldap_help_watchdog_detail', 0);
    $this->sortConsumerIds($op, $consumer_ids);
    $results = array();
    $watchdog_tokens = array();
    if (!is_array($consumer_ids)) {
      $consumer_ids = array($consumer_ids);
    }
    $watchdog_tokens['%username'] = $user->name;
    $watchdog_tokens['%action'] = $op;
    $watchdog_tokens['%user_save'] = $user_save;
    $consumer_ids_log = array();
    $users_authorization_ids = $this->usersAuthorizations($user);
    $watchdog_tokens['%users_authorization_ids'] = join(', ', $users_authorization_ids);
    if ($detailed_watchdog_log) {watchdog('ldap_authorization', "on call of grantsAndRevokes: user_auth_data=" . print_r($user_auth_data, TRUE), $watchdog_tokens, WATCHDOG_DEBUG);}

    foreach ($consumer_ids as $consumer_id) {
      if ($detailed_watchdog_log) {watchdog('ldap_authorization', "consumer_id=$consumer_id, user_save=$user_save, op=$op", $watchdog_tokens, WATCHDOG_DEBUG);}
      $log = "consumer_id=$consumer_id, op=$op,";
      $results[$consumer_id] = TRUE;
      if ($op == 'grant' && in_array($consumer_id, $users_authorization_ids) && !isset($user_auth_data[$consumer_id])) {
         // authorization id already exists for user, but is not ldap provisioned.  mark as ldap provisioned, but don't regrant
         $user_auth_data[$consumer_id] = array('date_granted' => time() );
      }
      elseif ($op == 'grant' && !in_array($consumer_id, $users_authorization_ids)) {
        $log .=" grant existing consumer id ($consumer_id), ";
        if (!in_array($consumer_id, $this->availableConsumerIDs(TRUE))) {
          $log .= "consumer id not available for $op, ";
          if ($this->allowConsumerObjectCreation) {
            $this->createConsumers(array($consumer_id));
            if (in_array($consumer_id, $this->availableConsumerIDs(TRUE))) {
              if ($detailed_watchdog_log) {watchdog('ldap_authorization', "grantSingleAuthorization : consumer_id=$consumer_id, op=$op", $watchdog_tokens, WATCHDOG_DEBUG);}
              $this->grantSingleAuthorization($user, $consumer_id, $user_auth_data);  // allow consuming module to add additional data to $user_auth_data
              $user_auth_data[$consumer_id] = array('date_granted' => time() );
              $log .= "created consumer object, ";
            }
            else {
              $log .= "tried and failed to create consumer object, ";
              $results[$consumer_id] = FALSE;
               // out of luck, failed to create consumer id
            }
          }
          else {
            $log .= "consumer does not support creating consumer object, ";
            // out of luck. can't create new consumer id.
            $results[$consumer_id] = FALSE;
          }
        }
        if ($results[$consumer_id]) {
          if ($detailed_watchdog_log) {watchdog('ldap_authorization', "grantSingleAuthorization : consumer_id=$consumer_id, op=$op", $watchdog_tokens, WATCHDOG_DEBUG);}
          $log .= "granting existing consumer object, ";
          $results[$consumer_id] = $this->grantSingleAuthorization($user, $consumer_id, $user_auth_data); // allow consuming module to add additional data to $user_auth_data

          if ($results[$consumer_id]) {
            $user_auth_data[$consumer_id] = array('date_granted' => time() );
          }
          $log .= t(',result=') . (boolean)($results[$consumer_id]);
        }
      }
      elseif ($op == 'revoke') {
        if (isset($user_auth_data[$consumer_id])) {
          $log .= "revoking existing consumer object, ";
          if (in_array($consumer_id, $users_authorization_ids)) {
            $results[$consumer_id] = $this->revokeSingleAuthorization($user, $consumer_id, $user_auth_data);  // defer to default for $user_save param
            if ($results[$consumer_id]) {
              unset($user_auth_data[$consumer_id]);
            }
            $log .= t(',result=') . (boolean)($results[$consumer_id]);
          }
          else {
            unset($user_auth_data[$consumer_id]);
          }
        }
      }
      $consumer_ids_log[] = $log;
      if ($detailed_watchdog_log) {watchdog('ldap_authorization', "user_auth_data after consumer $consumer_id" . print_r($user_auth_data, TRUE), $watchdog_tokens, WATCHDOG_DEBUG);}

      $watchdog_tokens['%consumer_ids_log'] = (count($consumer_ids_log)) ? join('<hr/>', $consumer_ids_log) : t('no actions');
    }

    if ($user_save) {
      $user = user_load($user->uid, TRUE);
      $user_edit = $user->data;
      $user_edit['data']['ldap_authorizations'][$this->consumerType] = $user_auth_data;
      $user = user_save($user, $user_edit);
    }

    watchdog('ldap_authorization', '%username:
      <hr/>LdapAuthorizationConsumerAbstract grantsAndRevokes() method log.  action=%action:<br/> %consumer_ids_log
      ',
      $watchdog_tokens, WATCHDOG_DEBUG);

  }

  /**
   * @param drupal user object $user to have $consumer_id revoked
   * @param string $consumer_id $consumer_id such as drupal role name, og group name, etc.
   * @param array $user_auth_data array of $user data specific to this consumer type.
   *   stored in $user->data['ldap_authorization'][<consumer_type>] array
   *
   * return boolen TRUE on success, FALSE on fail.  If user save is FALSE, the user object will
   *   not be saved and reloaded, so a returned TRUE may be misleading.
   */

  public function revokeSingleAuthorization(&$user, $role_name, &$user_auth_data) {
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
  * @param string $consumer_id $consumer_id such as drupal role name, og group name, etc.
   * @param array $user_auth_data array of $user data specific to this consumer type.
   *   stored in $user->data['ldap_authorization'][<consumer_type>] array
  *
  * return boolen TRUE on success, FALSE on fail.  If user save is FALSE, the user object will
  *   not be saved and reloaded, so a returned TRUE may be misleading.
  */
  public function createSingleAuthorization(&$user, $role_name, &$user_auth_data) {
     // method must be overridden
  }

  public function hasLdapGrantedAuthorization(&$user, $authorization_id) {
    // @todo load user and check field ldap_authorizations
    return @$user->data['ldap_authorizations'][$this->consumerType][$authorization_id];
  }

  public function hasAuthorization(&$user, $authorization_id) {
    return @in_array($authorization_id, $this->usersAuthorizations($user));
  }

  /**
   * @param string $map_to such as drupal role or og group/role
   * @return array with validation type ('error', 'warning', 'status')
   *   and message text
   */
  public function validateAuthorizationMappingTarget($map_to, $form_values = NULL, $clear_cache = FALSE) {
    $message_type = NULL;
    $message_text = NULL;
		return array($message_type, $message_text);
	}


}
