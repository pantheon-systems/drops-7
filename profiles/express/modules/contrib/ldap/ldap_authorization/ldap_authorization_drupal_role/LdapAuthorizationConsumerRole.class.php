<?php

/**
 * @file
 *
 * class to represent configuration of ldap authorizations to drupal roles
 *
 *
 */

module_load_include('php', 'ldap_authorization', 'LdapAuthorizationConsumerAbstract.class');

class LdapAuthorizationConsumerDrupalRole extends LdapAuthorizationConsumerAbstract {

  public $consumerType = 'drupal_role';
  public $allowConsumerObjectCreation = TRUE;

  public $defaultConsumerConfProperties = array(
    'onlyApplyToLdapAuthenticated' => TRUE,
    'useMappingsAsFilter' => TRUE,
    'synchOnLogon' => TRUE,
    'revokeLdapProvisioned' => TRUE,
    'regrantLdapProvisioned' => TRUE,
    'createConsumers' => TRUE,
    );

  function __construct($consumer_type = NULL) {
    $params = ldap_authorization_drupal_role_ldap_authorization_consumer();
    parent::__construct('drupal_role', $params['drupal_role']);
  }

  /**
   * @see LdapAuthorizationConsumerAbstract::createConsumer
   */

  public function createConsumer($consumer_id, $consumer) {
    $roles_by_consumer_id = $this->existingRolesByRoleName();
    $existing_role = isset($roles_by_consumer_id[$consumer_id]) ? $roles_by_consumer_id[$consumer_id] : FALSE;

    if ($existing_role) {
      return FALSE; // role exists
    }
    elseif (drupal_strlen($consumer_id) > 63) {
      watchdog('ldap_authorization_drupal_role', 'Tried to create drupal role
        with name of over 63 characters (%group_name).  Please correct your
        drupal ldap_authorization settings', array('%group_name' => $consumer_id));
      return FALSE;
    }

    $new_role = new stdClass();
    $new_role->name = empty($consumer['value']) ? $consumer_id : $consumer['value'];
    if (! ($status = user_role_save($new_role))) {
      // if role is not created, remove from array to user object doesn't have it stored as granted
      watchdog('user', 'failed to create drupal role %role in ldap_authorizations module', array('%role' => $new_role->name));
      return FALSE;
    }
    else {
      $roles_by_consumer_id = $this->existingRolesByRoleName(TRUE); // flush existingRolesByRoleName cache after creating new role
      watchdog('user', 'created drupal role %role in ldap_authorizations module', array('%role' => $new_role->name));
    }
    return TRUE;
  }


  /**
   * @see LdapAuthorizationConsumerAbstract::populateConsumersFromConsumerIds
   */

  public function populateConsumersFromConsumerIds(&$consumers, $create_missing_consumers = FALSE) {

    $roles_by_consumer_id = $this->existingRolesByRoleName(TRUE);
    foreach ($consumers as $consumer_id => $consumer) {

      if (!$consumer['exists']) { // role marked as not existing
        if (isset($roles_by_consumer_id[$consumer_id])) { // check if is existing
          $consumer['exists'] = TRUE;
          $consumer['value'] = $roles_by_consumer_id[$consumer_id]['role_name'];
          $consumer['name'] = $consumer['map_to_string'];
          $consumer['map_to_string'] = $roles_by_consumer_id[$consumer_id]['role_name'];
        }
        elseif ($create_missing_consumers) {
          $consumer['value'] = $consumer['map_to_string'];
          $consumer['name'] = $consumer['map_to_string'];
          $result = $this->createConsumer($consumer_id, $consumer);
          $consumer['exists'] = $result;
        }
        else {
          $consumer['exists'] = FALSE;
        }
      }
      elseif (empty($consumer['value'])) {
        $consumer['value'] = $roles_by_consumer_id[$consumer_id]['role_name'];
      }
      $consumers[$consumer_id] = $consumer;
    }
  }


  public function revokeSingleAuthorization(&$user, $consumer_id, $consumer, &$user_auth_data, $user_save = FALSE, $reset = FALSE) {

    $role_name_lcase = $consumer_id;
    $role_name = empty($consumer['value']) ? $consumer_id : $consumer['value'];
    $rid = $this->getDrupalRoleIdFromRoleName($role_name);
    if (!$rid) {
      $result = FALSE; // role id not found
    }
    elseif (!$user->roles[$rid]) { // user doesn't have role
      if (isset($user_auth_data[$consumer_id])) {
        unset($user_auth_data[$consumer_id]);
      }
      $result = TRUE;
    }
    else {
      unset($user->roles[$rid]);
      $user_edit = array('roles' => $user->roles);
      $account = user_load($user->uid);
      $user = user_save($account, $user_edit);
      $result = ($user && !isset($user->roles[$rid]));
      if ($result && isset($user_auth_data[$consumer_id])) {
        unset($user_auth_data[$consumer_id]);
      }
    }

    if ($this->detailedWatchdogLog) {
      watchdog('ldap_authorization', 'LdapAuthorizationConsumerDrupalRole.revokeSingleAuthorization()
        revoked:  rid=%rid, role_name=%role_name for username=%username, result=%result',
        array('%rid' => $rid, '%role_name' => $role_name, '%username' => $user->name,
          '%result' => $result), WATCHDOG_DEBUG);
    }

    return $result;

  }

  /**
   * extends grantSingleAuthorization()
   */

  public function grantSingleAuthorization(&$user, $consumer_id, $consumer, &$user_auth_data, $user_save = FALSE, $reset = FALSE) {

    $role_name_lcase = $consumer_id;
    $role_name = empty($consumer['value']) ? $consumer_id : $consumer['value'];
    $rid = $this->getDrupalRoleIdFromRoleName($role_name);
    if (is_null($rid)) {
      watchdog('ldap_authorization', 'LdapAuthorizationConsumerDrupalRole.grantSingleAuthorization()
      failed to grant %username the role %role_name because role does not exist',
      array('%role_name' => $role_name, '%username' => $user->name),
      WATCHDOG_ERROR);
      return FALSE;
    }

    $user->roles[$rid] = $role_name;
    $user_edit = array('roles' => $user->roles);
    if ($this->detailedWatchdogLog) {
      watchdog('ldap_authorization', 'grantSingleAuthorization in drupal rold' . print_r($user, TRUE), array(), WATCHDOG_DEBUG);
    }

    $account = user_load($user->uid);
    $user = user_save($account, $user_edit);
    $result = ($user && !empty($user->roles[$rid]));

    if ($this->detailedWatchdogLog) {
      watchdog('ldap_authorization', 'LdapAuthorizationConsumerDrupalRole.grantSingleAuthorization()
        granted: rid=%rid, role_name=%role_name for username=%username, result=%result',
        array('%rid' => $rid, '%role_name' => $role_name, '%username' => $user->name,
          '%result' => $result), WATCHDOG_DEBUG);
    }

    return $result;

  }

  public function usersAuthorizations(&$user) {
    $authorizations = array();
    foreach ($user->roles as $rid => $role_name_mixed_case) {
      $authorizations[] = drupal_strtolower($role_name_mixed_case);
    }
    return $authorizations;
  }

  public function validateAuthorizationMappingTarget($mapping, $form_values = NULL, $clear_cache = FALSE) {

    $has_form_values = is_array($form_values);
    $message_type = NULL;
    $message_text = NULL;
    $role_name = $mapping['normalized'];
    $tokens = array('!map_to' => $role_name);
    $roles_by_name = $this->existingRolesByRoleName();
    $pass = isset($roles_by_name[drupal_strtolower($role_name)]);


    if (!$pass) {
      $message_text = '"' . t('Drupal role') . ' ' . t('!map_to', $tokens) . '" ' . t('does not map to any existing Drupal roles.');
      if ($has_form_values) {
        $create_consumers = (isset($form_values['synchronization_actions']['create_consumers']) && $form_values['synchronization_actions']['create_consumers']);
      }
      else {
        $create_consumers = $this->consumerConf->createConsumers;
      }
      if ($create_consumers && $this->allowConsumerObjectCreation) {
        $message_type = 'warning';
        $message_text .= ' ' . t('"!map_to" will be created when needed.  If "!map_to" is not intentional, please fix it.', $tokens);
      }
      elseif (!$this->allowConsumerObjectCreation) {
        $message_type = 'error';
        $message_text .= ' ' . t('Since automatic Drupal role creation is not possible with this module, an existing role must be mapped to.');
      }
      elseif (!$create_consumers) {
        $message_type = 'error';
        $message_text .= ' ' . t('Since automatic Drupal role creation is disabled, an existing role must be mapped to.  Either enable role creation or map to an existing role.');
      }
    }
    return array($message_type, $message_text);
  }

  /**
   * @param string mixed case $role_name
   * @return integer role id
   */

  private function getDrupalRoleIdFromRoleName($role_name) {
    $role_ids_by_name = $this->existingRolesByRoleName();
    $role_name_lowercase = drupal_strtolower($role_name);
    return empty($role_ids_by_name[$role_name_lowercase]) ? NULL : $role_ids_by_name[$role_name_lowercase]['rid'];
  }

  /**
   * @param boolean $reset to reset static values
   * @return associative array() keyed on lowercase role names with values
   *   of array('rid' => role id, 'role_name' => mixed case role name)
   */
  public function existingRolesByRoleName($reset = FALSE) {

    static $roles_by_name;

    if ($reset || !is_array($roles_by_name)) {
      $roles_by_name = array();
      foreach (array_flip(user_roles(TRUE)) as $role_name => $rid) {
        $roles_by_name[drupal_strtolower($role_name)]['rid'] = $rid;
        $roles_by_name[drupal_strtolower($role_name)]['role_name'] = $role_name;
      }
    }
    return $roles_by_name;
  }

 /**
   * @see LdapAuthorizationConsumerAbstract::normalizeMappings
   */
  public function normalizeMappings($mappings) {

    $new_mappings = array();
    $roles = user_roles(TRUE); // in rid => role name format
    $roles_by_name = array_flip($roles);
    foreach ($mappings as $i => $mapping) {
      $new_mapping = array();
      $new_mapping['user_entered'] = $mapping[1];
      $new_mapping['from'] = $mapping[0];
      $new_mapping['normalized'] = $mapping[1];
      $new_mapping['simplified'] = $mapping[1];
      $create_consumers = (boolean)($this->allowConsumerObjectCreation && $this->consumerConf->createConsumers);
      $new_mapping['valid'] = (boolean)(!$create_consumers && !empty($roles_by_name[$mapping[1]]));
      $new_mapping['error_message'] = ($new_mapping['valid']) ? '' : t("Role %role_name does not exist and role creation is not enabled.", array('%role' => $mapping[1]));
      $new_mappings[] = $new_mapping;
    }

    return $new_mappings;
  }

    /**
	 * @see ldapAuthorizationConsumerAbstract::convertToFriendlyAuthorizationIds
	 */
  public function convertToFriendlyAuthorizationIds($authorizations) {
    $authorization_ids_friendly = array();
    foreach ($authorizations as $authorization_id => $authorization) {
      $authorization_ids_friendly[] = $authorization['name'] . '  (' . $authorization_id . ')';
    }
    return $authorization_ids_friendly;
  }

}
