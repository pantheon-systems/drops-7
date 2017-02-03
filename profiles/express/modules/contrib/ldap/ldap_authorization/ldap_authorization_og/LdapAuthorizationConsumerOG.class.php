<?php
// $Id: LdapAuthorizationConsumerOG.class.php,v 1.3.2.1 2011/02/08 20:05:42 johnbarclay Exp $



/**
 * @file
 * class for ldap authorization of organic groups
 *
 */

module_load_include('php', 'ldap_authorization', 'LdapAuthorizationConsumerAbstract.class');

class LdapAuthorizationConsumerOG extends LdapAuthorizationConsumerAbstract {

  public $consumerType = 'og_group';
  public $allowSynchBothDirections = FALSE;
  public $allowConsumerObjectCreation = FALSE;
  public $onlyApplyToLdapAuthenticatedDefault = TRUE;
  public $useMappingsAsFilterDefault = TRUE;
  public $synchOnLogonDefault = TRUE;
  public $synchManuallyDefault = TRUE;
  public $revokeLdapProvisionedDefault = TRUE;
  public $regrantLdapProvisionedDefault = TRUE;
  public $createContainersDefault = FALSE;
	public $ogRoles = array();
	public $ogRolesByName = array();
	public $ogVersion = NULL; // 1, 2, etc.
	public $ogs = array();  // array with keys of entity-type, entity-id, rid

 /**
   * Constructor Method
   *
   */
  function __construct($consumer_type = NULL) {

    $this->ogVersion = ldap_authorization_og_og_version();
    $params = ldap_authorization_og_ldap_authorization_consumer();
		if ($this->ogVersion == 1) {
			$this->ogRoles = og_roles(0);
			$this->ogRolesByName = array_flip($this->ogRoles);
		}
		else {
		  $this->_setConsumerIDs();
		}
    parent::__construct('og_group', $params['og_group']);
  }

  public function refreshConsumerIDs() {
		$this->_setConsumerIDs();
  }

  public function _setConsumerIDs() {
    $this->_availableConsumerIDs = array();

		if ($this->ogVersion == 1) { // og 7.1.x
			$groups = og_get_all_group();
			$og_entities = og_load_multiple($groups);
			foreach($og_entities as $group) {
				$this->ogs[$group->gid] = $group;
				foreach ($this->ogRoles as $rid => $role) {
					$auth_id = ldap_authorization_og_authorization_id($group->gid, $rid);
					$this->_availableConsumerIDs[$auth_id] = $group->label . ", $role";
				}
			}
		}
		else { // og 7.2.x
      list($this->ogs, $this->_availableConsumerIDs) = $this->og2Groups();
		//	dpm($this->ogs); dpm($this->_availableConsumerIDs);
		}
  }

  public static function og2Groups() {
		$ogs = array();
		$availableConsumerIDs = array();
		foreach (og_get_all_group_bundle() as $entity_type => $bundles) {
			$group_entity_ids = og_get_all_group($entity_type);
			$group_entities = entity_load($entity_type, $group_entity_ids);
			$ogs[$entity_type] = $group_entities;
			foreach ($group_entities as $entity_id => $group_entity) {
				$roles = og_roles($entity_type, $group_entity->type, $entity_id);
				$ogs[$entity_type][$entity_id] = array(
					'roles' => $roles,
					'entity' => $group_entity,
					'name' => isset($group_entity->title) ? $group_entity->title : '',
					);
				foreach ($roles as $rid => $role) {
					$auth_id = ldap_authorization_og_authorization_id($entity_id, $rid, $entity_type);
					$availableConsumerIDs[$auth_id] = $ogs[$entity_type][$entity_id]['name'] . " - $role";
				}
			}
		}
		return array($ogs, $availableConsumerIDs);


	}

	public function normalizeMappings($mappings) {

		if ($this->ogVersion == 2) {  // not relavant to og 2 mappings
			return $mappings;
		}

		foreach ($mappings as $i => $mapping) {
			$gid = NULL;
			$rid = NULL;

			$targets = explode(',', $mapping[1]);
			if (count($targets) != 2) {
				return FALSE;
			}

			$group_target_and_value =  explode('=', $targets[0]);
			if (count($group_target_and_value) != 2) {
				return FALSE;
			}
			list($group_target, $group_target_value) = $group_target_and_value;

			$role_target_and_value = explode('=', $targets[1]);
			if (count($role_target_and_value) != 2) {
				return FALSE;
			}
			list($role_target, $role_target_value) = $role_target_and_value;

			if ($group_target == 'gid') {
				$gid = $group_target_value;
			}
			elseif ($group_target == 'group-name') {
        list($og_group, $og_node) = ldap_authorization_og1_get_group($group_target_value, 'group_name', 'object');
				if (is_object($og_group) && property_exists($og_group, 'gid') && $og_group->gid) {
					$gid = $og_group->gid;
				}
			}
			else {
				$entity_type_and_field = explode('.', $group_target);
				if (count($entity_type_and_field) != 2) {
					return FALSE;
			  }
        list($entity_type, $field) = $entity_type_and_field;

				$query = new EntityFieldQuery();
				$query->entityCondition('entity_type', $entity_type)
					->fieldCondition($field, 'value', $group_target_value, '=')
					->addMetaData('account', user_load(1)); // run the query as user 1

				$result = $query->execute();
				if (is_array($result) && isset($result[$entity_type]) && count($result[$entity_type]) == 1) {
					$entities = array_keys($result[$entity_type]);
					$gid = ldap_authorization_og1_entity_id_to_gid($entities[0]);
				}
			}

			if ($role_target == 'rid') {
				$rid = $role_target_value;
			}
			elseif ($role_target == 'role-name') {
				$rid = ldap_authorization_og_rid_from_role_name($role_target_value);
			}

			if ($gid && $rid) {
				$mappings[$i][1] = ldap_authorization_og_authorization_id($gid, $rid);
			}
			else {
				$mappings[$i][1] = FALSE;
			}
		}
		return $mappings;

	}


  /**
	 * Return list of all available consumer ids/authorization ids
	 * @param boolean $reset whether to rebuild array
	 * @return array of consumer ids of form:
	 *   array([og-group-id]-[rid], ...)
	 *   such as array('7-2', '3-3')
	 */

  public function availableConsumerIDs($reset = FALSE) {
    if ($reset || ! is_array($this->_availableConsumerIDs)) {
      $this->refreshConsumerIDs();
    }
    return array_keys($this->_availableConsumerIDs);
  }

/**
 * some authorization schemes such as organic groups, require a certain order.  implement this method
 * to sort consumer ids/authorization ids before they are granted to the user
 *
 * @param string $op 'grant' or 'revoke' signifying what to do with the $consumer_ids
 *
 * alters $consumer_ids by reference
 *
 * in organic groups, consumer ids are in form gid-rid such as 3-2, 3-3.  We want highest authorization available granted.
 * But, granting member role (2), revokes other roles such as admin in OG.  So for granting we want the order:
 * 3-1, 3-2, 3-3 such that 3-3 is retained.  For revoking, the order should not matter, but reverse sorting makes
 * intuitive sense.
 */

  public function sortConsumerIds($op, &$consumer_ids) {
		if ($op == 'revoke') {
			arsort($consumer_ids, SORT_STRING);
		}
		else {
			asort($consumer_ids, SORT_STRING);
		}
	}

/**
	* revoke an authorization
	*
	* extends revokeSingleAuthorization()
	*
	* @param drupal user object $user
	* @param string $authorization_id (aka consumer id) in form organic group gid-rid such as 7-2
	* @param array $user_auth_data is array specific to this consumer_type.  Stored in $user->data['ldap_authorizations']['og_group']
	*
	* @return TRUE if revoked or user doesn't have role FALSE if not revoked or failed.
	*
  * this function does not save the user object or alter $user_auth_data.
  * this is handled in the abstract class.
  */

  public function revokeSingleAuthorization(&$user, $authorization_id, &$user_auth_data) {

		if ($this->ogVersion == 1) {
			list($gid, $rid) = @explode('-', $authorization_id);
		}
		else {
			list($group_type, $gid, $rid) = @explode(':', $authorization_id);
		}

		// CASE 1: Bad Parameters
		if (!$authorization_id || !$gid || !$rid || !is_object($user) || ($this->ogVersion == 2  && !$group_type)) {
      watchdog('ldap_authorization_og', 'LdapAuthorizationConsumerOG.grantSingleAuthorization()
                improper parameters.',
                array(),
                WATCHDOG_ERROR);
			return FALSE;
		}

		$ldap_granted = $this->hasLdapGrantedAuthorization($user, $authorization_id);
		$granted = $this->hasAuthorization($user, $authorization_id);
		if ($this->ogVersion == 1) { // og 7.x-1.x
			$users_group_roles = og_get_user_roles($gid, $user->uid);
		}
		else { // og 7.x-2.x
			$users_group_roles = og_get_user_roles($group_type, $gid, $user->uid);
		}

    // CASE 2: user doesnt have grant to revoke
		if (!$granted || ($granted && !$ldap_granted)) {
			return TRUE; // don't do anything.  don't log since non-event
		}

    // CASE 3: revoke


		if (count($users_group_roles) == 1) {  // ungroup if only single role left
			if ($this->ogVersion == 1) { // og 7.x-1.x
				$entity = og_ungroup($gid, 'user', $user->uid, TRUE);
			}
			else { // og 7.x-2.x
				$entity = og_ungroup($group_type, $gid, 'user', $user->uid);
			}
			$result = (boolean)($entity);
			$watchdog_tokens['%action'] = 'og_ungroup';
		}
		else { // if more than one role left, just revoke single role.
			if ($this->ogVersion == 1) { // og 7.x-1.x
				og_role_revoke($gid, $user->uid, $rid);
			}
			else { // og 7.x-2.x
				og_role_revoke($group_type, $gid, $user->uid, $rid);
			}
			$watchdog_tokens['%action'] = 'og_role_revoke';
			return TRUE;
		}

    if ($this->detailedWatchdogLog) {
      watchdog('ldap_authorization_og', 'LdapAuthorizationConsumerOG.revokeSingleAuthorization()
        revoked:  gid=%gid, rid=%rid, action=%action for username=%username',
        $watchdog_tokens, WATCHDOG_DEBUG);
    }

    return $result;

  }

  /**
   * add user to group and grant a role.
   *
   * extends grantSingleAuthorization()
   *
   * @param drupal user objet $user
   * @param string $authorization_id in form organic group gid-rid such as 7-2
   * @param array $user_auth_data is array specific to this consumer_type.  Stored in $user->data['ldap_authorizations']['og_group']
   *
   * @return TRUE if granted or grant exists, FALSE if not grantable or failed.
   */
  public function grantSingleAuthorization(&$user, $authorization_id, &$user_auth_data) {
    $result = FALSE;
    $watchdog_tokens =  array('%authorization_id' => $authorization_id, '%username' => $user->name, '%ogversion' => $this->ogVersion);
		if ($this->detailedWatchdogLog) {
			watchdog('ldap_auth_og',
						 'LdapAuthorizationConsumerOG.grantSingleAuthorization()
                beginning to grant authorization for $group_name=%group_name to user %username',
              $watchdog_tokens,
							WATCHDOG_DEBUG);
		}
		if ($this->ogVersion == 1) {
			list($gid, $rid) = @explode('-', $authorization_id);
		}
		else {
			list($group_type, $gid, $rid) = @explode(':', $authorization_id);
	    $watchdog_tokens['%group_type'] = $group_type;
		}
		$watchdog_tokens['%gid'] = $gid;
		$watchdog_tokens['%rid'] = $rid;
		$watchdog_tokens['%uid'] = $user->uid;
		$available_consumer_ids = $this->availableConsumerIDs(TRUE);

		// CASE 1: Bad Parameters
		if (!$authorization_id || !$gid || !$rid || !is_object($user) || ($this->ogVersion == 2  && !$group_type)) {
      watchdog('ldap_auth_og', 'LdapAuthorizationConsumerOG.grantSingleAuthorization()
                improper parameters.',
                $watchdog_tokens,
                WATCHDOG_ERROR);
			return FALSE;
		}

		// CASE 2: gid-rid does not exist
		if (!in_array($authorization_id, $available_consumer_ids)) {
			$result = FALSE;
      watchdog('ldap_authorization_og', 'LdapAuthorizationConsumerOG.grantSingleAuthorization()
                failed to grant %username the group-role %authorization_id because group-role does not exist',
                $watchdog_tokens,
                WATCHDOG_ERROR);
			return FALSE;
		}

		$ldap_granted = $this->hasLdapGrantedAuthorization($user, $authorization_id);
		$granted = $this->hasAuthorization($user, $authorization_id);

    // CASE 3: user already granted permissions via ldap grant
		if ($ldap_granted && $granted) {
			watchdog('ldap_auth_og', 'LdapAuthorizationConsumerOG.grantSingleAuthorization()
								<hr />not granted: gid=%gid, for username=%username,
								<br />because user already belongs to group',
								$watchdog_tokens, WATCHDOG_DEBUG);
			return TRUE;
		}

    // CASE 4:  user already granted permissions, but NOT via ldap grant
		if ($granted && !$ldap_granted) { // need to make ldap granted
			watchdog('ldap_authorization_og', 'LdapAuthorizationConsumerOG.grantSingleAuthorization()
								<hr />membership already exists for: gid=%gid, rid=%rid, for username=%username,
								<br />but made ldap granted.',
								$watchdog_tokens, WATCHDOG_DEBUG);
			return TRUE; // return true so is made ldap granted, even though membership is not created.
		}

		// CASE 5:  grant role
		if ($this->detailedWatchdogLog) {
			watchdog('ldap_auth_og',
						 'LdapAuthorizationConsumerOG.grantSingleAuthorization()
                calling og_role_grant(%group_type, %gid, %uid, %rid).
								og version=%ogversion',
              $watchdog_tokens,
							WATCHDOG_DEBUG);
		}
		if ($this->ogVersion == 2) {
			$values = array(
				'entity_type' => 'user',
				'entity' => $user->uid,
				'field_name' => FALSE,
				'state' => OG_STATE_ACTIVE,
			);
			$og_membership = og_group($group_type, $gid, $values);
			og_role_grant($group_type, $gid, $user->uid, $rid);
		}
		else {
			$values = array(
				'entity type' => 'user',
				'entity' => $user,
				'state' => OG_STATE_ACTIVE,
				'membership type' => OG_MEMBERSHIP_TYPE_DEFAULT,
			);
			watchdog('ldap_auth_og', 'og_group1', $watchdog_tokens, WATCHDOG_DEBUG);
			$user_entity = og_group($gid, $values);
			watchdog('ldap_auth_og', 'og_role_grant1', $watchdog_tokens, WATCHDOG_DEBUG);
			og_role_grant($gid, $user->uid, $rid);
		}

		if ($this->detailedWatchdogLog) {
			watchdog('ldap_auth_og', 'LdapAuthorizationConsumerOG.grantSingleAuthorization()
								<hr />granted: group_type=%group_type gid=%gid, rid=%rid for username=%username',
								$watchdog_tokens, WATCHDOG_DEBUG);
		}
		return TRUE;

  }

  /**
	 * Return all user authorization ids (group x role) in form gid-rid such as 2-1.
	 *   regardless of it they were granted by this module, any authorization ids should be returned.
	 *
	 * @param user object $user
	 * @return array such as array('3-2','7-2')
	 */

  public function usersAuthorizations(&$user) {
    $authorizations = array();
		if ($this->ogVersion == 1) {
			$groups = og_load_multiple(og_get_all_group());
			$authorizations = array();
			if (is_object($user) && is_array($groups)) {
				foreach ($groups as $gid => $discard) {
					$roles = og_get_user_roles($gid, $user->uid);
					foreach ($roles as $rid => $discard) {
						$authorizations[] = ldap_authorization_og_authorization_id($gid, $rid);
					}
				}
			}
		}
		else { // og 7.x-2.x
			$user_entities = entity_load('user', array($user->uid));
			$memberships = og_get_entity_groups('user', $user_entities[$user->uid]);
			foreach ($memberships as $entity_type => $entity_memberships) {
				foreach ($entity_memberships as $og_membership_id => $gid) {
					$roles = og_get_user_roles($entity_type, $gid, $user->uid);
					foreach ($roles as $rid => $discard) {
						$authorizations[] =  ldap_authorization_og_authorization_id($gid, $rid, $entity_type);
					}
				}
			}
		}
    return $authorizations;
  }

  /**
	 * @param array authorization ids in "normalized" format of 2-2, 3-2, etc.
	 * @return array friendly authorization is names such as Bakers Groups Member, or Knitters Groups Admin Member
	 */
	public function convertToFriendlyAuthorizationIds($authorizations) {
		$authorization_ids_friendly = array();
		$this->refreshConsumerIDs();
		foreach ($authorizations as $i => $authorization_id) {

			if ($this->ogVersion == 1) {
				list($gid, $rid) = explode('-', $authorization_id);
				$authorization_ids_friendly[] = 'Group: '. $this->ogs[$gid]->label  . ', Role: ' . $this->ogRoles[$rid] . " ($authorization_id) ";
			}
			else { // @todo make this fiendly authorization ids work\
				list($entity_type, $gid, $rid) = explode(':', $authorization_id);
				$authorization_ids_friendly[] = 'Group: '. $this->ogs[$entity_type][$gid]['name'] . ', Role: ' . $this->ogs[$entity_type][$gid]['roles'][$rid] . " ($authorization_id) ";
			}
		}
		return $authorization_ids_friendly;
	}

  /**
	 * Validate authorization mappings on LDAP Authorization OG Admin form.
	 *
	 * @param string $map_to from mapping tables in authorization configuration form
	 * @param array $form_values from authorization configuration form
	 * @param boolean $clear_cache
	 *
	 * @return array of form array($message_type, $message_text) where message type is status, warning, or error
	 *   and $message_text is what the user should see.
	 *
	 */
  public function validateAuthorizationMappingTarget($map_to, $form_values = NULL, $clear_cache = FALSE) {
    $has_form_values = is_array($form_values);
		$message_type = NULL;
		$message_text = NULL;
    $tokens = array('!map_to' => $map_to);
		$available_authorization_ids = $this->availableConsumerIDs($clear_cache);
		$pass = FALSE;
		if ($this->ogVersion == 1) {
			$normalized = $this->normalizeMappings(array(array('placeholder', $map_to)));
			if (is_array($normalized) && isset($normalized[0][1]) && $normalized[0][1] !== FALSE ) {
				list($gid, $rid) = explode('-', $normalized[0][1]);
				$pass = (in_array($normalized[0][1], $available_authorization_ids));
			}
		}
		else {
			$normalized = TRUE; // not relevant to og 2
			$parts = explode(':', $map_to);
			if (count($parts) == 3) {
				list($entity_type, $entity_id, $rid) = $parts;
				$pass = isset($this->ogs[$entity_type][$entity_id]['roles'][$rid]);
			}
		}

		if (!$pass) {
			$message_text = '<code>"' . t('!map_to', $tokens) . '"</code> ' . t('does not map to any existing organic groups and roles. ');

      if ($has_form_values) {
        $create_consumers = (isset($form_values['synchronization_actions']['create_consumers']) && $form_values['synchronization_actions']['create_consumers']);
      }
      else {
        $create_consumers = $this->consumerConf->create_consumers;
      }
		  if ($normalized === FALSE) {
				$message_type = 'error';
				$message_text .= t('Can not normalize mappings.  Please check the syntax in Mapping of LDAP to OG Group', $tokens);
		  }
			elseif ($create_consumers && $this->allowConsumerObjectCreation) {
				$message_type = 'warning';
				$message_text .= t('It will be created when needed.  If "!map_to" is not intentional, please fix it', $tokens);
			}
			elseif (!$this->allowConsumerObjectCreation) {
				$message_type = 'error';
				$message_text .= t('Since automatic organic group creation is not possible with this module, an existing group must be mapped to.');
			}
			elseif (!$create_consumers) {
				$message_type = 'error';
				$message_text .= t('Since automatic organic group creation is disabled, an existing group must be mapped to.  Either enable organic group creation or map to an existing group.');
			}
		}
		return array($message_type, $message_text);
	}

  /**
	 * Get list of mappings based on existing Organic Groups and roles
	 *
	 * @param associative array $tokens of tokens and replacement values
	 * @return html examples of mapping values
	 */

	public function mappingExamples($tokens) {

		if ($this->ogVersion == 1) {
			$groups = og_get_all_group();
			$ogEntities = og_load_multiple($groups);
			$OGroles = og_roles(0);

			$rows = array();
			foreach($ogEntities as $group) {
				foreach ($OGroles as $rid => $role) {
					$example =   "<code>ou=IT,dc=myorg,dc=mytld,dc=edu|gid=" . $group->gid . ',rid=' . $rid . '</code><br/>' .
						'<code>ou=IT,dc=myorg,dc=mytld,dc=edu|group-name=' . $group->label . ',role-name=' . $role . '</code>';
					$rows[] = array(
						$group->label,
						$group->gid,
						$role,
						$example,
					);
				}
			}

			$variables = array(
			'header' => array('Group Name', 'OG Group ID', 'OG Membership Type', 'example'),
			'rows' => $rows,
			'attributes' => array(),
			);
		}
		else {

			/**
			 * OG 7.x-2.x mappings:
			 * $entity_type = $group_type,
			 * $bundle = $group_bundle
			 * $etid = $gid where edid is nid, uid, etc.
			 *
			 * og group is: entity_type (eg node) x entity_id ($gid) eg. node:17
			 * group identifier = group_type:gid; aka entity_type:etid e.g. node:17
			 *
			 * membership identifier is:  group_type:gid:entity_type:etid
			 * in our case: group_type:gid:user:uid aka entity_type:etid:user:uid e.g. node:17:user:2
			 *
			 * roles are simply rids ((1,2,3) and names (non-member, member, and administrator member) in og_role table
			 * og_users_roles is simply uid x rid x gid
			 *
			 * .. so authorization mappings should look like:
			 *    <ldap group>|group_type:gid:rid such as staff|node:17:2
			 */

			$rows = array();
			foreach ($this->ogs as $entity_type => $entities) {
				foreach ($entities as $entity_id => $entity) {
					foreach ($entity['roles'] as $rid => $role) {
						$group_role_identifier = ldap_authorization_og_authorization_id($entity_id, $rid, $entity_type);
						$example =   "<code>ou=IT,dc=myorg,dc=mytld,dc=edu|$group_role_identifier</code>";
						$rows[] = array($entity['name'] . ' - ' . $role, $example);
					}
			  }
			}

			$variables = array(
				'header' => array('Group Name - OG Membership Type', 'example'),
				'rows' => $rows,
				'attributes' => array(),
			);
		}




		$table = theme('table', $variables);
		$link = l('admin/config/people/ldap/authorization/test/og_group','admin/config/people/ldap/authorization/test/og_group');

$examples =
<<<EOT

<br/>
Examples for some (or all) existing OG Group IDs can be found in the table below.
This is complex.  To test what is going to happen, uncheck "When a user logs on" in IV.B.
and use $link to see what memberships sample users would receive.

$table

EOT;
		$examples = t($examples, $tokens);
		return $examples;
	}

}
