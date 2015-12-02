<?php
// $Id: ldap_authorization.inc,v 1.3.2.3 2011/02/18 15:06:09 johnbarclay Exp $

/**
 * @file
 *  bulk of authorization code executed to determine a users authorizations
 */

function ldap_authorization_help_watchdog() {  // remove after testing

  drupal_add_css(drupal_get_path('module', 'ldap_help') . '/ldap_help.css', 'module', 'all', FALSE);
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
  if ($context == 'test_if_authorizations_granted' || ($op == 'test_query' && @$user->ldap_test == TRUE)) {
    $ldap_authenticated = $user->ldap_authenticated;  // property 'ldap_authenticated' only exists for fake user objects
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
    }

    if ($context == 'logon' && !$consumer->consumerConf->synchOnLogon) {
      $notifications[$consumer_type][] = LDAP_AUTHORIZATION_MAP_NOT_CONF_FOR_LOGON;
      if ($detailed_watchdog_log) {
        watchdog('ldap_authorization', '%username : %consumer_type not set to run on user logon.', $watchdog_tokens, WATCHDOG_DEBUG);
      }
      continue;
    }

    if ($consumer->consumerConf->onlyApplyToLdapAuthenticated  && !$ldap_authenticated  && $op != 'test_query') {
      if ($detailed_watchdog_log) {
        watchdog('ldap_authorization', '%username : not used because it is set to be applied only to ldap authenticated users.
            %username  is not ldap authenticated.', $watchdog_tokens, WATCHDOG_DEBUG);
      }
      $notifications[$consumer_type][] = LDAP_AUTHORIZATION_USER_NOT_LDAP_AUTHENTICATED;
      continue;
    }

    $consumer_sid = $consumer->consumerConf->deriveFromEntrySearchAll ? NULL : $consumer->consumerConf->sid;
    if (! ($user_ldap_entry = ldap_servers_get_user_ldap_data($user, $consumer_sid))) {
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

    $ldap_server = $servers[$consumer->consumerConf->sid];

    /**
     * 1. first just need to figure out what authz_ids are generated for this consumer type/mapping configuration
     *
     * goal here is simply to build an array of authorizations for this ldap authz mapping
     * $proposed_ldap_authorizations[<authorization id>] = properties associative array or empty array
     *  e.g.  $proposed_ldap_authorizations['admin'] = array()
     *
     * the authorization ids may represent drupal roles, organic groups, civicrm groups, etc.
     * these mappings are a function of:
     *   -  drupal user entry, $user
     *   -  a user ldap entry, $user_ldap_entry
     *   -  an ldap server configuration, $ldap_server
     *   -  a mapping configuration ($consumer_conf)
     */

    if ($detailed_watchdog_log || $debug) {
      $_proposed_ldap_authorizations_pre_hook_maps_alter = is_array($proposed_ldap_authorizations) ? $proposed_ldap_authorizations : array();
      $watchdog_tokens['%proposed_authorizations_pre_hook'] = join(', ', $_proposed_ldap_authorizations_pre_hook_maps_alter);
      watchdog('ldap_authorization', '%username : initial proposed authorization before mapps_alter_invoke %consumer_type: %proposed_authorizations_pre_hook.',
        $watchdog_tokens, WATCHDOG_DEBUG);
    }

    ldap_authorization_maps_alter_invoke($user, $user_ldap_entry, $ldap_server, $consumer->consumerConf, $proposed_ldap_authorizations, $op);

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

    /** make sure keys of array are lower case and values are mixed case **/
    foreach ($proposed_ldap_authorizations as $key => $value) {
      if ($key != drupal_strtolower($key)) {
        $proposed_ldap_authorizations[drupal_strtolower($key)] = $value;
        unset($proposed_ldap_authorizations[$key]);
      }
    }
   // debug('proposed_ldap_authorizations3'); debug($proposed_ldap_authorizations);
    /**
     * 2.  filter can be both a whitelist and a mapping of an ldap results to an authorization id.
     * goal of this step is to generate $filtered_ldap_authorizations[$consumer_type]
     * an array of filtered and mapped authorization ids
     */


    if ($consumer->consumerConf->useMappingsAsFilter) { // filter + map
      $filtered_ldap_authorizations = array();
     // debug('useMappingsAsFilter');
      foreach ($consumer->consumerConf->normalizedMappings as $mapping_filter) {
        $map_from = $mapping_filter[0];
        $map_to = $mapping_filter[1];
       // debug("from:$map_from to:$map_to");
        if (isset($proposed_ldap_authorizations[drupal_strtolower($map_from)])) {
          $filtered_ldap_authorizations[] = $map_to;
        }
      }
    }
    else { // only map
     // debug('not useMappingsAsFilter');
      $filtered_ldap_authorizations = array_values($proposed_ldap_authorizations);
      if (is_array($consumer->consumerConf->mappings) && is_array($proposed_ldap_authorizations)) {
        foreach ($consumer->consumerConf->mappings as $mapping_filter) {
          $map_from = $mapping_filter[0];
          $map_to = $mapping_filter[1];
         // debug("from:$map_from to:$map_to");
          $map_from_key = array_search(drupal_strtolower($map_from), array_keys($proposed_ldap_authorizations));
          if ($map_from_key !== FALSE) {
            // remove non mapped authorization
            $filtered_ldap_authorizations = array_diff($filtered_ldap_authorizations, array($map_from));
            $filtered_ldap_authorizations = array_diff($filtered_ldap_authorizations, array(drupal_strtolower($map_from)));
            // add mapped authorization
            $filtered_ldap_authorizations[] = $map_to;
            // remove map from;
          }
        }
      }
    }

    $filtered_ldap_authorizations = array_unique($filtered_ldap_authorizations);
   // debug('filtered_ldap_authorizations'); debug($filtered_ldap_authorizations);
    $watchdog_tokens['%filtered_ldap_authorizations'] = join(', ', $filtered_ldap_authorizations);
    if ($detailed_watchdog_log) {
      watchdog('ldap_authorization', '%username : filtered authorization for %consumer_type: %filtered_ldap_authorizations.',
        $watchdog_tokens, WATCHDOG_DEBUG);
    }
    if ($debug) {
      debug(t('%username : filtered authorization for %consumer_type: %filtered_ldap_authorizations.',
        $watchdog_tokens));
    }

    /**
     * 3. third, grant any proposed authorizations not already granted
     */

    if ($op == 'test_query') {
      $_SESSION['ldap_authorization_test_query']['tokens'] = $watchdog_tokens;
    }
    if ($op == 'set') {
      _ldap_authorizations_user_authorizations_set($user, $consumer, $filtered_ldap_authorizations, $user_ldap_entry, $watchdog_tokens);
    }
   // debug('filtered,'. $consumer_type); debug($authorizations[$consumer_type]);
    $authorizations[$consumer_type] = $filtered_ldap_authorizations;
  } //  end foreach $consumers

  return array($authorizations, $notifications);

}
/**
 * @param object $user is a drupal user account object, need not be current user
 * @param object $consumer is instance of an authorization consumer class such as LdapAuthorizationConsumerDrupalRole
 * @param array $filtered_ldap_authorizations all authorization ids a user is granted via ldap authorization configuration
 * @param object $ldap_entry is users ldap entry.  mapping of drupal user to ldap entry is stored in ldap_server configuration
 *
 * returns nothing
 */

function _ldap_authorizations_user_authorizations_set(&$user, $consumer, $filtered_ldap_authorizations, &$ldap_entry, $watchdog_tokens) {
//  debug('filtered ldap authorizations'); debug($filtered_ldap_authorizations);
  $detailed_watchdog_log = variable_get('ldap_help_watchdog_detail', 0);

  ldap_authorization_cleanse_empty_og_fields($user);

  /**
   * A.  Determine what authorizations have been granted in the past by ldap authorization
   */

  if (isset($user->data['ldap_authorizations'][$consumer->consumerType]) && is_array($user->data['ldap_authorizations'][$consumer->consumerType])) {
    $user_auth_data = $user->data['ldap_authorizations'][$consumer->consumerType];
    $initial_existing_ldap_authorizations = array_keys($user_auth_data);
  }
  else {
    $user_auth_data = array();
    $initial_existing_ldap_authorizations = array();
  }

  $watchdog_tokens['%initial'] = join(', ', $initial_existing_ldap_authorizations);
  $grants = $filtered_ldap_authorizations;
  $watchdog_tokens['%filtered_ldap_authorizations'] = join(', ', $filtered_ldap_authorizations);
   /**
   * B. if regrantLdapProvisioned is false, $grants array should only be new authorizations
   */
  // if regranting disabled, filter off previously granted roles
  if ($consumer->consumerConf->regrantLdapProvisioned === FALSE) {
    $grants = array_diff($filtered_ldap_authorizations, $initial_existing_ldap_authorizations);
  }
  $watchdog_tokens['%grants1'] = join(', ', $grants);


   /**
   * C.  query or create existing authorization consumer ids (drupal roles, og groups etc.)
   */
  $consumer_containers_existing = $consumer->availableConsumerIDs();
  $containers_needed = array_diff($grants, $consumer_containers_existing);
  $watchdog_tokens['%consumer_containers_initial'] = (count($consumer_containers_existing)) ? join(', ', $consumer_containers_existing) :  t('none');
  $watchdog_tokens['%consumer_containers_needed'] =  (count($containers_needed)) ?  join(', ', $containers_needed) :  t('none');

  if (count($containers_needed) > 0) {
    if ($consumer->consumerConf->createConsumers) {
      $consumer->createConsumers($containers_needed);
      $consumer_containers_existing = $consumer->availableConsumerIDs();  // requery in case of failure
    }
    else {
      $grants = array_diff($grants, $containers_needed);  // filter off consumer ids that don't exist and can't be created
    }
  }

  $watchdog_tokens['%consumer_containers_final'] = join(', ', $consumer_containers_existing);

   /**
   * D.  Only grant authorization consumer ids that exist
   */


  $watchdog_tokens['%consumer_containers_existing'] = (count($consumer_containers_existing)) ? join(', ', $consumer_containers_existing) :  t('none');
  $watchdog_tokens['%grants_pre_intersect'] = (count($grants)) ? join(', ', $grants) :  t('none');
  $grants = array_intersect($consumer_containers_existing, $grants);
  $watchdog_tokens['%grants_post_intersect'] = (count($grants)) ? join(', ', $grants) :  t('none');

  /**
   * E. Do grants
   */
  $consumer->authorizationGrant($user, $user_auth_data, $grants, $ldap_entry, FALSE);

  /**
   *  3.F take away any authorizations not in proposed authorization,
   *      but previously granted by ldap
   */
  $watchdog_tokens['%revokes'] = t('none');
  if ($consumer->consumerConf->revokeLdapProvisioned) {
    $revokes = array_diff($initial_existing_ldap_authorizations, $filtered_ldap_authorizations);
    if (count($revokes)) {
      $consumer->authorizationRevoke($user, $user_auth_data, $revokes, $ldap_entry, FALSE);
      $watchdog_tokens['%revokes'] = join(', ', $revokes);
    }
  }

  $watchdog_tokens['%user_data'] = print_r($user_auth_data, TRUE);


  /**
   *  3.G  save user object and user data
   */

  $user = user_load($user->uid, TRUE);
  $user->data['ldap_authorizations'][$consumer->consumerType] = $user_auth_data; // not a merge here.
  $user_edit['data'] = $user->data;
  $user = user_save($user, $user_edit);
  $user = user_load($user->uid, TRUE);

  $watchdog_tokens['%user_obj_data_ldap_authorizations'] = print_r($user->data, TRUE);

  if ($detailed_watchdog_log) {
    watchdog('ldap_authorization', '%username : user_authorizations_set results for %consumer_type:
      <hr/>1. Initial existing authorizations:  %initial
      <hr/>1. Filtered Authorizations: %filtered_ldap_authorizations
      <hr/>2. After filtering off previously granted authorizations: %grants1
      <hr/>3. All available existing authorization ids: %consumer_containers_initial
      <hr/>4. authorization ids that need to be created: %consumer_containers_needed
      <hr/>5. consumer containers existing after create call (or non-call if og): %consumer_containers_final
      <hr/>6a. consumer_containers_existing: %consumer_containers_existing
      <hr/>6b. grants_pre_intersect: %grants_pre_intersect
      <hr/>6c. grants_post_intersect: %grants_post_intersect
      <hr/>7. revokes passed to authorizationRevoke(): %revokes
      <hr/>8. user auth data after save for %consumer_type: %user_data
      <hr/>9. user->data[ldap_authorizations] after save: <pre>%user_obj_data_ldap_authorizations</pre>
      ', $watchdog_tokens, WATCHDOG_DEBUG);
  }

}

function _ldap_authorization_ldap_authorization_maps_alter(&$user, &$user_ldap_entry, &$ldap_server, &$consumer_conf, &$authz_ids, $op) {

  $detailed_watchdog_log = variable_get('ldap_help_watchdog_detail', 0);
  $watchdog_tokens = array();

  // Strategy 1: group extracted from user's DN.
  $derive_from_dn_authorizations = array();
  if ($consumer_conf->deriveFromDn) {
   // debug('deriveFromDn');
    $pairs = ldap_explode_dn($user_ldap_entry['dn'], 0); // escapes attribute values, need to be unescaped later
    $count = array_shift($pairs);
    foreach ($pairs as $p) {
      $pair = explode('=', $p);
      if (drupal_strtolower(trim($pair[0])) == drupal_strtolower($consumer_conf->deriveFromDnAttr)) {
        $authorization_id = ldap_pear_unescape_dn_value(trim($pair[1]));
        $derive_from_dn_authorizations[drupal_strtolower($authorization_id)] = (string)$authorization_id;
      }
    }
  //  debug($derive_from_dn_authorizations);
  }
  if ($op == 'test_query') {
    $_SESSION['ldap_authorization_test_query']['maps']['Strategy 1. Derive from DN'] = ($consumer_conf->deriveFromDn) ? $derive_from_dn_authorizations : t('disabled');
  }

  // Strategy 2: groups in user attributes
  $derive_from_attr_authorizations = array();
  if ($consumer_conf->deriveFromAttr) {
   // debug('consumer_conf->deriveFromAttr');
    foreach ($consumer_conf->deriveFromAttrAttr as $derive_from_attribute_name) {
      $authorizations = $ldap_server->deriveFromAttrGroups($derive_from_attribute_name, $user_ldap_entry, $consumer_conf->deriveFromAttrNested);
     //debug('authorizations'); debug($authorizations);
      foreach ($authorizations as $id => $authorization) {
        if ($consumer_conf->deriveFromAttrUseFirstAttr) {
         // debug('authorization'); debug($authorization);
          $attr_parts = ldap_explode_dn($authorization, 0); // explode_dn escapes attribute values, so must be unescaped later!
         // debug('attr_parts'); debug($attr_parts);
          $first_part = explode('=', $attr_parts[0]);
         // debug('first_part'); debug($first_part); debug(ldap_pear_unescape_filter_value($first_part));
          $authorization_id = ldap_pear_unescape_filter_value(trim($first_part[1]));
        }
        else {
          $authorization_id = $authorization;
        }
        $derive_from_attr_authorizations[drupal_strtolower($authorization_id)] = $authorization_id;
      }
    }
  }
  if ($op == 'test_query') {
    $_SESSION['ldap_authorization_test_query']['maps']['Strategy 2. Groups in User Attributes'] = ($consumer_conf->deriveFromAttr) ? $derive_from_attr_authorizations : t('disabled');
  }

/**
 *
 *  Strategy 3: groups as entries.
 *
 *  given:
 *  - user dn = cn=jkool,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu
 *  - deriveFromEntryEntries = array(cn=content editors,ou=groups,dc=ad,dc=myuniversity,dc=edu)
 *  - deriveFromEntryMembershipAttr = 'member'
 *
 * search on member=cn=jkool,ou=guest accounts,dc=ad,dc=myuniversity,dc=edu within basedn cn=content editors,ou=groups,dc=ad,dc=myuniversity,dc=edu
 *
 * returned entries dn or cn should be used to derive authorization mappings
 *
 */

  $derive_from_entry_authorizations = array();
  if ($consumer_conf->deriveFromEntry) {
    if ($consumer_conf->deriveFromEntryAttrMatchingUserAttrUndefined) {
      // this condition will be removed in the 7.x-2.x branch,  its here to avoid breaking 7.x-1.x sites
      // that use this somewhat flawed approach successfully.  see: http://drupal.org/node/1412076
      foreach ($consumer_conf->deriveFromEntryEntries as $branch) {
        $filter = '(' . $consumer_conf->deriveFromEntryMembershipAttr . '=' . $user_ldap_entry['dn'] . ')';
        $entries = $ldap_server->search($branch, $filter, array('cn'));
        if ($entries === FALSE || empty($entries) || $entries['count'] == 0) {
          $filter = '(' . $consumer_conf->deriveFromEntryMembershipAttr . '=' . $user->name . ')';
          $entries = $ldap_server->search($branch, $filter, array('cn'));
        }
        if ($entries !== FALSE) {
          foreach ($entries as $entry) {
            if (isset($entry['cn'])) {
              $authorization_id = $entry['cn'][0];
            }
            elseif (isset($entry['dn'])) {
              $authorization_id = (string)$entry['dn'];
            }
            $derive_from_entry_authorizations[drupal_strtolower($authorization_id)] = $authorization_id;
          }
        }
      }
    }
    elseif (isset($user_ldap_entry[$consumer_conf->deriveFromEntryAttrMatchingUserAttr]) ||
      isset($user_ldap_entry['attr'][ldap_server_massage_text($consumer_conf->deriveFromEntryAttrMatchingUserAttr, 'attr_name', LDAP_SERVER_MASSAGE_QUERY_ARRAY)])) {
      // $derive_from_entries_entries, $derive_from_entry_attr, $derive_from_entry_user_ldap_attr, $user_ldap_entry, $nested = FALSE
      $derive_from_entry_authorizations = $ldap_server->deriveFromEntryGroups(
        $consumer_conf->deriveFromEntryEntries,
        $consumer_conf->deriveFromEntryEntriesAttr,
        $consumer_conf->deriveFromEntryMembershipAttr,
        $consumer_conf->deriveFromEntryAttrMatchingUserAttr,
        $user_ldap_entry,
        $consumer_conf->deriveFromEntryNested
      );
      //  deriveFromEntryGroups($entries, $entries_attr
      if (count($derive_from_entry_authorizations)) {
        foreach ($derive_from_entry_authorizations as $i => $authorization) {
          if ($consumer_conf->deriveFromEntryUseFirstAttr) {
            $attr_parts = ldap_explode_dn($authorization, 0); // escapes attribute values, need to be unescaped later
            $first_part = explode('=', $attr_parts[0]);
            $authorization_id = ldap_pear_unescape_dn_value(trim($first_part[1]));
          }
          else {
            $authorization_id = $authorization;
          }
          $derive_from_entry_authorizations[drupal_strtolower($authorization_id)] = $authorization_id;
        }
      }
    }
  }
  if ($op == 'test_query') {
    $_SESSION['ldap_authorization_test_query']['maps']['Strategy 3. groups  as entries'] = ($consumer_conf->deriveFromEntry) ? $derive_from_entry_authorizations  : t('disabled');
  }

  $values = array_merge(array_values($derive_from_dn_authorizations), array_values($derive_from_attr_authorizations), array_values($derive_from_entry_authorizations));
  $values = array_unique($values);
 // debug('values'); debug($values);
  $authz_ids = (count($values)) ? array_combine($values, $values) : array();
 // debug('authz_ids'); debug($authz_ids);
  if ($detailed_watchdog_log) {
    $watchdog_tokens['%username'] = $user->name;
    $watchdog_tokens['%ldap_server'] = $ldap_server->sid;
    $watchdog_tokens['%deriveFromDn'] = join(', ', array_keys($derive_from_dn_authorizations));
    $watchdog_tokens['%deriveFromAttr'] = join(', ', array_keys($derive_from_attr_authorizations));
    $watchdog_tokens['%deriveFromEntry'] = 'authorizations: ' . join(', ', array_keys($derive_from_entry_authorizations));
    $watchdog_tokens['%authz_ids'] =  join(', ', array_keys($authz_ids));

    watchdog('ldap_authorization', '%username :_ldap_authorization_ldap_authorization_maps_alter:
      <hr/>deriveFromDn authorization ids: %deriveFromDn
      <hr/>deriveFromAttr authorization ids: %deriveFromAttr
      <hr/>deriveFromEntry authorization ids: %deriveFromEntry
      <hr/>merged authz_ids authorization ids: %authz_ids
      ',
      $watchdog_tokens, WATCHDOG_DEBUG);
  }

}
