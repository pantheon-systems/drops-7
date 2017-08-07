<?php

/**
 * @file
 *  bulk of authorization code executed to determine a users authorizations
 */

function ldap_authorization_help_watchdog() {  // remove after testing

  $path =  drupal_get_path("module", "ldap_help");
  $_content = "";
  if (module_exists('dblog')) {
    include_once(drupal_get_path('module', 'dblog') . '/dblog.admin.inc');
    $_SESSION['dblog_overview_filter']['type'] = Array('ldap' => 'ldap');
    $_content .= "<h3>" . t('LDAP Watchdog Errors and Notifications') . "</h3>";
    $overview = dblog_overview();
    $_content .= render($overview);

    $_content .= l(t('...more watchdog'), 'admin/reports/dblog');
  }
  else {
    $_content .= "<h3>" . t('LDAP Help Watchdog Errors and Notifications') . "</h3>";
    $_content .= 'This feature requires <code>Database logging</code> module to be turned on. ';
    $_content .= l(t('Module enable page'), 'admin/build/modules');
  }


  return $_content;
}


/**
 * return all desired authorizations for a given user
 *
 * @param object $user
 *
 * @param string $op =
 *   set -- grant authorizations (store in db) and return authorizations
 *   test_query -- don't grant authorization, just query and return authorizations.  assume user is ldap authenticated and exists
 *   test_query_set -- do grant authorizations, but also log data for debugging
 *   query -- don't grant authorization, just query and return authorizations
 *
 * @param string $consumer_type e.g. drupal_roles
 * @param string $context  'logon', 'test_if_authorizations_granted'
 *
 * @return
 *
 *   LDAP_AUTHORIZATION_NO_LDAP_SERVERS if no servers configured
 *   LDAP_AUTHORIZATION_LDAP_ERROR if ldap error
 *   TRUE if servers configured but no roles derived from ldap
 *   array of potential authorizations (user may or may not already have these)
 *
 *   by reference $user->data[<consumer_type>][<authorization_id>] = array();
 *      e.g.   $var['drupal_role']['content_admin'] = array('rid' => 4)
 *      e.g.   $var['og_membership']['bakers club'] = array('expires' => '01/01/2012');
 *
 */


function _ldap_authorizations_user_authorizations(&$user, $op, $consumer_type, $context) {
  $debug = FALSE;
  $detailed_watchdog_log = variable_get('ldap_help_watchdog_detail', 0);
  $authorizations = array();
  $notifications = array();
  $watchdog_tokens = array('%username' => $user->name);
  $consumers = ldap_authorization_get_consumers($consumer_type, TRUE, FALSE);

  $servers = ldap_servers_get_servers(NULL, 'enabled', TRUE);

  /**
   * user 1 not used in ldap authorization.  this is a design decision.
   */
  if (property_exists($user, 'uid') && $user->uid == 1) {
    if ($detailed_watchdog_log) {
      watchdog('ldap_authorization', '%username : ldap_authorization not applied to user 1', $watchdog_tokens, WATCHDOG_DEBUG);
    }
    $notifications['all'] = LDAP_AUTHORIZATION_NOT_APPLY_USER_1;
    foreach ($consumers as $consumer_type => $consumer) {
      $authorizations[$consumer_type] = array();
    }
    return array($authorizations, $notifications);
  }

  /**
   * determine if user is ldap authenticated
   */
  if ($context == 'test_if_authorizations_granted' || (($op == 'test_query_set' || $op == 'test_query') && @$user->ldap_test == TRUE)) {
    $ldap_authenticated = $user->ldap_authenticated;  // property 'ldap_authenticated' only exists for fake user objects submitted from testing form
  }
  else {
    $ldap_authenticated = (boolean)(module_exists('ldap_authentication') && ldap_authentication_ldap_authenticated($user));
  }
  $watchdog_tokens['%ldap_authenticated'] = ($ldap_authenticated) ? 'yes' : 'no';

  foreach ($consumers as $consumer_type => $consumer) {
    $authorizations[$consumer_type] = array();
    /**
    * each consumer type has only one consumer conf and each consumer conf has only one ldap server id (sid)
    * so there is a one-to-one-to-one relationship between:
    *   - consumer object ($consumer),
    *   - server object ($ldap_server),
    *   - and consumer conf object.
    *
    */

    $consumer = ldap_authorization_get_consumer_object($consumer_type);
    if (!$consumer->consumerConf->status) {
      continue;
    }
    $proposed_ldap_authorizations = array();
    $watchdog_tokens['%consumer_type'] = $consumer_type;
    $watchdog_tokens['%sid'] = $consumer->consumerConf->sid;

    if (! is_object($consumer->consumerConf)) {
      if ($detailed_watchdog_log) {
        watchdog('ldap_authorization', '%username : consumer type  %consumer_type has no
          configuration set.', $watchdog_tokens, WATCHDOG_DEBUG);
      }
      continue;
    }

    if ($detailed_watchdog_log) {
      watchdog('ldap_authorization', '%username : testing with
         consumer type %consumer_type. ldap authenticated=%ldap_authenticated', $watchdog_tokens, WATCHDOG_DEBUG);
    }
    if ($debug) {
      debug(t('%username : testing with consumer type %consumer_type. ldap authenticated=%ldap_authenticated'), $watchdog_tokens);
      debug("op=$op,ldap_authenticated=$ldap_authenticated $consumer_type context=$context, consumer->consumerConf->synchOnLogon=" . (int)$consumer->consumerConf->synchOnLogon); //$debug = TRUE;
    }

    if ($context == 'logon' && !$consumer->consumerConf->synchOnLogon) {
      $notifications[$consumer_type][] = LDAP_AUTHORIZATION_MAP_NOT_CONF_FOR_LOGON;
      if ($detailed_watchdog_log) {
        watchdog('ldap_authorization', '%username : %consumer_type not set to run on user logon.', $watchdog_tokens, WATCHDOG_DEBUG);
      }
      continue;
    }

    if ($consumer->consumerConf->onlyApplyToLdapAuthenticated && !$ldap_authenticated  && $op != 'test_query' && $op != 'test_query_set') {
      if ($detailed_watchdog_log) {
        watchdog('ldap_authorization', '%username : not used because it is set to be applied only to ldap authenticated users.
            %username  is not ldap authenticated.', $watchdog_tokens, WATCHDOG_DEBUG);
      }
      $notifications[$consumer_type][] = LDAP_AUTHORIZATION_USER_NOT_LDAP_AUTHENTICATED;
      continue;
    }

    $ldap_user = ldap_servers_get_user_ldap_data($user, $consumer->consumerConf->sid, 'ldap_authorization__' . $consumer_type);

    if (!$ldap_user) {
      $notifications[$consumer_type][] = LDAP_AUTHORIZATION_USER_LDAP_NOT_FOUND;
      if ($detailed_watchdog_log) {
        watchdog('ldap_authorization', '%username : %consumer_type ldap user not found.', $watchdog_tokens, WATCHDOG_DEBUG);
      }
      continue;
    }

    if (! isset($servers[$consumer->consumerConf->sid])) {
      $notifications[$consumer_type][] = LDAP_AUTHORIZATION_SERVER_CONFIG_NOT_FOUND;
      if ($detailed_watchdog_log) {
        watchdog('ldap_authorization', '%username : %consumer_type ldap server %sid not enabled or found.', $watchdog_tokens, WATCHDOG_DEBUG);
      }
      continue;
    }

    $ldap_server = $consumer->consumerConf->server;

    /**
     * 1. first just need to figure out what authz_ids are generated for this consumer type/mapping configuration
     *
     * goal here is simply to build an array of authorizations for this ldap authz mapping
     * $proposed_ldap_authorizations[<authorization id>] = properties associative array or empty array
     *  e.g.  $proposed_ldap_authorizations['admin'] = array()
     *
     * @see ldap_authorization.api.php hook_ldap_authorization_maps_alter() notes
     */

    ldap_authorization_maps_alter_invoke($user, $ldap_user, $ldap_server, $consumer->consumerConf, $proposed_ldap_authorizations, $op);

    /** make sure keys of array are lower case and values are mixed case
      and strip to first attribute is configured
    */

    foreach ($proposed_ldap_authorizations as $key => $authorization_id) {
      if ($consumer->consumerConf->useFirstAttrAsGroupId) {
        $attr_parts = ldap_explode_dn($authorization_id, 0);
        if (count($attr_parts) > 0) {
          $first_part = explode('=', $attr_parts[0]);
          if (count($first_part) > 1) {
            $authorization_id = ldap_pear_unescape_dn_value(trim($first_part[1]));
          }
        }
        $new_key = drupal_strtolower($authorization_id);
      }
      else {
        $new_key = drupal_strtolower($key);
      }
      $proposed_ldap_authorizations[$new_key] = $authorization_id;
      if ($key != $new_key) {
        unset($proposed_ldap_authorizations[$key]);
      }
    }

    if ($op == 'test_query' || $op == 'test_query_set') {
      $_SESSION['ldap_authorization_test_query']['useFirstAttrAsGroupId'] = $proposed_ldap_authorizations;
    }

    if ($detailed_watchdog_log || $debug) {
      $_proposed_ldap_authorizations = is_array($proposed_ldap_authorizations) ? $proposed_ldap_authorizations : array();
      $watchdog_tokens['%proposed_authorizations'] = join(', ', $_proposed_ldap_authorizations);
    }
    if ($detailed_watchdog_log) {
      watchdog('ldap_authorization', '%username : initial proposed authorization for %consumer_type: %proposed_authorizations.',
        $watchdog_tokens, WATCHDOG_DEBUG);
    }
    if ($debug) {
      debug(t('%username : initial proposed authorization for %consumer_type: %proposed_authorizations.',
        $watchdog_tokens));
    }

    /**
     * 2.  filter can be both a whitelist and a mapping of an ldap results to an authorization id.
     * goal of this step is to generate $filtered_ldap_authorizations[$consumer_type]
     * an array of filtered and mapped authorization ids
     */

    $filtered_ldap_authorizations = array();
    if ($consumer->consumerConf->useMappingsAsFilter) { // filter + map
      foreach ($consumer->consumerConf->mappings as $mapping_filter) {
        $map_from = $mapping_filter['from'];
        $map_to = $mapping_filter['normalized'];
        if (isset($proposed_ldap_authorizations[drupal_strtolower($map_from)])) {
          $filtered_ldap_authorizations[drupal_strtolower($map_to)] = array('map_to_string' => $map_to, 'exists' => NULL);
        }
      }
    }
    else { // only map, don't filter off authorizations that have no mapping
      $_authorizations = array_values($proposed_ldap_authorizations);
      if (is_array($consumer->consumerConf->mappings) && is_array($proposed_ldap_authorizations)) {
        foreach ($consumer->consumerConf->mappings as $mapping_filter) {
          $map_from = $mapping_filter['from'];
          $map_to = $mapping_filter['normalized'];
          $map_from_key = array_search(drupal_strtolower($map_from), array_keys($proposed_ldap_authorizations));
          if ($map_from_key !== FALSE) {
            // remove non mapped authorization
            $_authorizations = array_diff($_authorizations, array($map_from));
            $_authorizations = array_diff($_authorizations, array(drupal_strtolower($map_from)));
            // add mapped authorization
            $_authorizations[] = $map_to;
            // remove map from;
          }
        }
      }
      foreach ($_authorizations as $i => $authorization_id) {
        $filtered_ldap_authorizations[drupal_strtolower($authorization_id)] = array(
          'map_to_string' => $authorization_id,
          'exists' => NULL,
          'value' => $authorization_id
        );
      }
    }

    $consumer->populateConsumersFromConsumerIds($filtered_ldap_authorizations, $consumer->consumerConf->createConsumers); // set values of $filtered_ldap_authorizations to consumers
    /**
     * now that we have list of consumers that are to be granted, give other modules a chance to alter it
     *
     * @see ldap_authorization.api.php hook_ldap_authorization_authorizations_alter() notes
     */
    $params = array(
      'user' => $user,
      'ldap_user' => $ldap_user,
      'ldap_server' => $ldap_server,
      'consumer' => $consumer,
    );

    drupal_alter('ldap_authorization_authorizations', $filtered_ldap_authorizations, $params);

    $watchdog_tokens['%filtered_ldap_authorizations'] = join(', ', array_keys($filtered_ldap_authorizations));
    if ($detailed_watchdog_log) {
      watchdog('ldap_authorization', '%username : filtered authorization for %consumer_type: %filtered_ldap_authorizations.',
        $watchdog_tokens, WATCHDOG_DEBUG);
    }
    if ($debug) {
      debug(t('%username : filtered authorization for %consumer_type: %filtered_ldap_authorizations.',
        $watchdog_tokens));
    }

    if ($op == 'test_query' || $op == 'test_query_set') {
      $display_authorizations = array();
      foreach ($filtered_ldap_authorizations as $consumer_id => $_consumer) {
        $display_authorizations[] = $_consumer['map_to_string'];
      }
      $_SESSION['ldap_authorization_test_query']['post mappings'] = $display_authorizations;
      $data = property_exists($user, 'data') ? $user->data['ldap_authorizations'][$consumer->consumerType] : array();
      $_SESSION['ldap_authorization_test_query']['user data'] = $data;
    }

    /**
     * 3. third, grant any proposed authorizations not already granted
     */

    if ($op == 'test_query' || $op == 'test_query_set') {
      $_SESSION['ldap_authorization_test_query']['tokens'] = $watchdog_tokens;
    }
    if ($op == 'set' || $op == "test_query_set") {
      $test = ($op == "test_query_set");
      _ldap_authorizations_user_authorizations_set($user, $consumer, $filtered_ldap_authorizations, $ldap_user, $watchdog_tokens, $test);
    }

    $authorizations[$consumer_type] = $filtered_ldap_authorizations;
  } //  end foreach $consumers

  return array($authorizations, $notifications);

}
/**
 * @param object $user is a drupal user account object, need not be current user
 * @param object $consumer is instance of an authorization consumer class
 *   such as LdapAuthorizationConsumerDrupalRole
 * @param associative array of lower case authorization ids as keys and
 *    mixed case strings as values $filtered_ldap_authorizations
 *   all authorization ids a user is granted via ldap authorization configuration
 * @param object $ldap_entry is users ldap entry.  mapping of drupal user to
 *   ldap entry is stored in ldap_server configuration
 *
 * returns nothing
 */

function _ldap_authorizations_user_authorizations_set(&$user, $consumer, $filtered_ldap_authorizations, &$ldap_entry, $watchdog_tokens, $test) {

  $detailed_watchdog_log = variable_get('ldap_help_watchdog_detail', 0);
  ldap_authorization_cleanse_empty_og_fields($user);

  /**
   * A.  Determine what authorizations have been granted in the past by ldap authorization
   */

  if (isset($user->data['ldap_authorizations'][$consumer->consumerType])
      && is_array($user->data['ldap_authorizations'][$consumer->consumerType])) {
    $user_auth_data = $user->data['ldap_authorizations'][$consumer->consumerType];
    $initial_existing_ldap_authorizations = array_keys($user_auth_data);
  }
  else {
    $user_auth_data = array();
    $initial_existing_ldap_authorizations = array();
  }

  if ($test) {
    $_SESSION['ldap_authorization_test_query']['setting_data']['Pre Grant/Revokes $user->data[ldap_authorizations][' . $consumer->consumerType . ']'] = $user_auth_data;
    $_SESSION['ldap_authorization_test_query']['setting_data']['Pre Grant/Revokes authorizations user has'] = $consumer->usersAuthorizations($user);
  }

  $watchdog_tokens['%initial'] = join(', ', $initial_existing_ldap_authorizations);
  $watchdog_tokens['%filtered_ldap_authorizations'] = join(', ', array_keys($filtered_ldap_authorizations));
   /**
   * B. if regrantLdapProvisioned is false, $grants_lcase array should only be new authorizations
   */

  if (!$consumer->consumerConf->regrantLdapProvisioned) {
    // if regranting disabled, filter off previously granted roles
    $grants = array_diff(array_keys($filtered_ldap_authorizations), $initial_existing_ldap_authorizations);
    if ($test) {
      $_SESSION['ldap_authorization_test_query']['setting_data']['Grants after regrantLdapProvisioned filter'] = $grants;
    }
  }
  else {
    $grants = array_keys($filtered_ldap_authorizations);
  }

  $watchdog_tokens['%grants1'] = join(', ', $grants);

   /**
   * D.  Only grant authorization consumer ids that exist
   */

  $existing_grants = array(); // keys are lcase, values are mixed case
  foreach ($grants as $i => $grant) {
    if (!empty($filtered_ldap_authorizations[$grant]['exists'])) {
      $existing_grants[$grant] = $filtered_ldap_authorizations[$grant];
    }
  }
  /**
   * E. Do grants
   */
  $watchdog_tokens['%filtered_grants_full_array'] = print_r($existing_grants, TRUE);
  $consumer->authorizationGrant($user, $user_auth_data, $existing_grants, $ldap_entry, FALSE);
  $watchdog_tokens['%user_auth_data_post_grants'] = print_r($user_auth_data, TRUE);
  $watchdog_tokens['%user_data_post_grants'] = print_r($user->data, TRUE);

  /**
   *  3.F take away any authorizations not in proposed authorization,
   *      but previously granted by ldap
   */
  $watchdog_tokens['%revokes'] = t('none');

  if ($consumer->consumerConf->revokeLdapProvisioned) {
    $revokes_lcase = $consumer->authorizationDiff($initial_existing_ldap_authorizations, array_keys($filtered_ldap_authorizations));
    if (count($revokes_lcase)) {
      $revokes = array(); // keys are lcase, values are mixed case
      foreach ($revokes_lcase as $i => $revoke_lcase) {
        $revokes[$revoke_lcase] = array(
          'value' => NULL,
          'map_to_string' => NULL,
          'exists' => TRUE,
        );
      }
      if ($test) {
        $_SESSION['ldap_authorization_test_query']['setting_data']['Revokes'] = $revokes;
      }
      $consumer->authorizationRevoke($user, $user_auth_data, $revokes, $ldap_entry, FALSE);
      $watchdog_tokens['%revokes'] = join(', ', array_keys($revokes));
    }
  }

  $watchdog_tokens['%user_auth_data_post_revokes'] = print_r($user_auth_data, TRUE);
  $watchdog_tokens['%user_data_post_revokes'] = print_r($user->data, TRUE);

  /**
   *  3.G  save user object and user data
   *  note $user object is passed by reference into this function
   */

  $uid = $user->uid;
  $user_edit = array('data' => $user->data);
  $user_edit['data']['ldap_authorizations'] = empty($user->data['ldap_authorizations']) ? array() : $user->data['ldap_authorizations'];
  $consumer->sortConsumerIds('grant', $user_auth_data);  // keep in good display order
  $user_edit['data']['ldap_authorizations'][$consumer->consumerType] = $user_auth_data;
  $watchdog_tokens['%user_edit_presave'] = print_r($user_edit, TRUE);
  if (empty($user->picture->fid)) { // see #1973352 and #935592
    $user2 = user_load($user->uid);
    $user->picture = $user2->picture;
  }
  $user = user_save($user, $user_edit);

  $watchdog_tokens['%user_data_post_save'] = print_r($user->data, TRUE);
  $watchdog_tokens['%user_roles_post_save'] = print_r($user->roles, TRUE);

  if ($detailed_watchdog_log) {
    watchdog('ldap_authorization',
      '%username : user_authorizations_set results for %consumer_type:
      <hr/>1. Initial existing authorizations:  %initial
      <hr/>2. Filtered Authorizations: %filtered_ldap_authorizations
      <hr/>3. After filtering off previously granted authorizations (if regrant is false): %grants1

      <hr/>4a. full consumers array, pre grants: %filtered_grants_full_array
      <hr/>4b. user auth data after grants %consumer_type: <pre>%user_auth_data_post_grants</pre>
      <hr/>4c. user data after grants for %consumer_type: <pre>%user_data_post_grants</pre>

      <hr/>5a. revokes passed to authorizationRevoke(): %revokes
      <hr/>5b. user auth data after revokes %consumer_type: <pre>%user_auth_data_post_revokes</pre>
      <hr/>5c. user data after revokes for %consumer_type: <pre>%user_data_post_revokes</pre>

      <hr/>6a. user_edit pre save : <pre>%user_edit_presave</pre>
      <hr/>6b. user data post save: <pre>%user_data_post_save</pre>
      <hr/>6c. user roles post save: <pre>%user_roles_post_save</pre>
      ',
      $watchdog_tokens, WATCHDOG_DEBUG);
  }

  if ($test) {
    $_SESSION['ldap_authorization_test_query']['setting_data']['Post Grant/Revokes authorizations user has'] = $consumer->usersAuthorizations($user);
    $_SESSION['ldap_authorization_test_query']['setting_data']['Post Grant/Revokes $user->data[ldap_authorizations][' . $consumer->consumerType . ']'] = $user->data['ldap_authorizations'][$consumer->consumerType];
  }

}

function _ldap_authorization_ldap_authorization_maps_alter(&$user, &$user_ldap_entry, &$ldap_server, &$consumer_conf, &$authz_ids, $op) {

  $detailed_watchdog_log = variable_get('ldap_help_watchdog_detail', 0);
  $watchdog_tokens = array();

  // groups extracted from user's DN. such as ou=IT => group = "IT"
  $derive_from_dn_authorizations = array();
  if ($rdn_values = $consumer_conf->server->groupUserMembershipsFromDn($user)) {
    $derive_from_dn_authorizations = array_combine($rdn_values, $rdn_values);
  }
  else {
    $derive_from_dn_authorizations = array();
  }
  if ($op == 'test_query' || $op == 'test_query_set') {
    $_SESSION['ldap_authorization_test_query']['maps']['Derive from DN'] = ($rdn_values) ? $derive_from_dn_authorizations : t('disabled');
  }

  // traditional groups (dns)
  $group_dns = $consumer_conf->server->groupMembershipsFromUser($user, 'group_dns');
  if (!$group_dns) {
    $group_dns = array();
  }
  elseif (count($group_dns)) {
    $group_dns = array_unique($group_dns);
  }
  if ($op == 'test_query' || $op == 'test_query_set') {
    $_SESSION['ldap_authorization_test_query']['maps']['Groups DNs'] = $group_dns;
  }

  $values = array_merge($derive_from_dn_authorizations, $group_dns);
  $values = array_unique($values);
  $authz_ids = (count($values)) ? array_combine($values, $values) : array();
  if ($detailed_watchdog_log) {
    $watchdog_tokens['%username'] = $user->name;
    $watchdog_tokens['%ldap_server'] = $ldap_server->sid;
    $watchdog_tokens['%deriveFromDn'] = join(', ', $derive_from_dn_authorizations);
    $watchdog_tokens['%deriveFromGroups'] = join(', ', $group_dns);
    $watchdog_tokens['%authz_ids'] =  join(', ', array_keys($authz_ids));

    watchdog('ldap_authorization', '%username :_ldap_authorization_ldap_authorization_maps_alter:
      <hr/>deriveFromDn authorization ids: %deriveFromDn
      <hr/>deriveFromAttr authorization ids: %deriveFromGroups
      <hr/>merged authz_ids authorization ids: %authz_ids
      ',
      $watchdog_tokens, WATCHDOG_DEBUG);
  }

}
