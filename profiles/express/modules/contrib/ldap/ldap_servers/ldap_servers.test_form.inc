<?php

module_load_include('inc', 'ldap_servers', 'ldap_servers.functions');

/**
 * @file
 */

/**
 * Implements the LDAP server test page.
 *
 * @param $form_state
 *   A form state array.
 * @param $op
 *   An operatin - add or edit.
 * @param $sid
 *   A LDAP server ID.
 *
 * @return
 *   The form structure.
 */

function ldap_servers_test_form($form, &$form_state, $op = NULL, $sid = NULL) {

  $ldap_server = ldap_servers_get_servers($sid, 'all', TRUE);

  drupal_set_title(t('Test LDAP Server Configuration: !server', array('!server' => $ldap_server->name)));

  $form['#prefix'] = t('This form tests an LDAP configuration to see if
    it can bind and basic user and group functions.  It also shows token examples
    and a sample user.  The only data this function will modify is the test LDAP group, which will be deleted and added');

  $variables = array(
    'ldap_server' => $ldap_server, 'actions' => FALSE, 'type' => 'detail'
  );

  $form['server_variables'] = array(
    '#markup' => theme('ldap_servers_server', $variables),
  );

  $form['sid'] = array(
    '#type' => 'hidden',
    '#default_value' => $sid,
  );

  $form['binding']['bindpw'] = array(
    '#type' => 'password',
    '#title' => t('Password for non-anonymous search'),
    '#size' => 20,
    '#maxlength' => 255,
    '#description' => t('Leave empty to test with currently stored password.'),
  );

  $form['testing_drupal_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Testing Drupal Username'),
    '#default_value' => $ldap_server->testingDrupalUsername,
    '#size' => 30,
    '#maxlength' => 255,
    '#description' => t('This is optional and used for testing this server\'s configuration against an actual username.  The user need not exist in Drupal and testing will not affect the user\'s LDAP or Drupal Account.'),
  );

  $form['testingDrupalUserDn'] = array(
    '#type' => 'textfield',
    '#title' => t('Testing Drupal DN'),
    '#default_value' => $ldap_server->testingDrupalUserDn,
    '#size' => 120,
    '#maxlength' => 255,
    '#description' => t('This is optional and used for testing this server\'s configuration against an actual username.  The user need not exist in Drupal and testing will not affect the user\'s LDAP or Drupal Account.'),
  );

  $form['grp_test_grp_dn'] = array(
    '#type' => 'textfield',
    '#title' => t('Testing Group DN'),
    '#default_value' => $ldap_server->groupTestGroupDn,
    '#size' => 120,
    '#maxlength' => 255,
    '#description' => t('This is optional and used for testing this server\'s group configuration.'),
  );

  $form['grp_test_grp_dn_writeable'] = array(
    '#type' => 'textfield',
    '#title' => t('Testing Group DN that is writeable. Warning!  In test, this group will be deleted, created, have members added to it!'),
    '#default_value' => $ldap_server->groupTestGroupDnWriteable,
    '#size' => 120,
    '#maxlength' => 255,
    '#description' => t('This is optional and used for testing this server\'s group configuration.'),
  );

  if ($ldap_server->bind_method ==  LDAP_SERVERS_BIND_METHOD_ANON_USER) {
    $form['testing_drupal_userpw'] = array(
      '#type' => 'password',
      '#title' => t('Testing Drupal User Password'),
      '#size' => 30,
      '#maxlength' => 255,
      '#description' => t('This is optional and used for testing this server\'s configuration against the username above.'),
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Test',
    '#weight' => 100,
  );

  if (!empty($form_state['ldap_server_test_data'])) {
    $test_data = $form_state['ldap_server_test_data'];

    if (isset($test_data['username']) && isset($test_data['ldap_user'])) {
      $form['#prefix'] = theme('ldap_server_ldap_entry_table',
        array(
          'entry' => $test_data['ldap_user']['attr'],
          'username' => $test_data['username'],
          'dn' => $test_data['ldap_user']['dn'],
        ));
    }

    $titles = array(
      'basic' => 'Test Results',
      'group1' => 'Group Create, Delete, Add Member, Remove Member Tests',
      'group2' => 'User Group Membership Functions Test',
      'tokens' => 'User Token Samples',
      'groupfromDN' => 'Groups Derived From User DN',
    );
    foreach ($test_data['results_tables'] as $table_name => $table_data) {
      $form['#prefix'] .= '<h2>' . $titles[$table_name] . '</h2>' . theme('table', array('header' => array('Test', 'Result'), 'rows' => $table_data));
    }

    if (function_exists('dpm') && !empty($test_data['username'])) {
      $user_name = $test_data['username'];
      if ($user = user_load_by_name($user_name)) {
        dpm("Corresponding Drupal user object for: $user_name");
        dpm($user);
        if (function_exists('entity_load_single')) {
          $user_entity = entity_load_single('user', $user->uid);
          dpm("Drupal user entity for: $user_name");
          dpm($user_entity);
        }
       dpm("Test Group LDAP Entry");
       dpm($test_data['group_entry'][0]);
      }
    }
  }
  return $form;
}

/**
 * Validate hook for the LDAP server form.
 */
function ldap_servers_test_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  if (!$values['sid']) {
    form_set_error(NULL, t('No server id found in form'));
  }
  elseif (!$ldap_server = ldap_servers_get_servers($values['sid'], 'all', TRUE)) {
    form_set_error(NULL, t('Failed to create server object for server with server id=%sid', array('%sid' => $values['sid'])));
  }
}

/**
 * Submit hook for the LDAP server form.
 */
function ldap_servers_test_form_submit($form, &$form_state) {

  //Pass data back to form builder
  $form_state['rebuild'] = TRUE;

  ldap_servers_module_load_include('inc', 'ldap_servers', 'ldap_servers.functions');
  $errors = FALSE;
  $has_errors = FALSE;
  $values = $form_state['values'];
  $sid = $values['sid'];
  $ldap_server = ldap_servers_get_servers($sid, 'all', TRUE);

  //$result = t('<h1>Test of name </h2>',$server_conf);
  $results = array();
  $results_tables = array();
  if ($values['bindpw']) {
    $bindpw = $values['bindpw'];
    $bindpw_type = t('entered in form.');
  }
  else {
    $bindpw = NULL;
    $bindpw_type = t('stored in configuration');
  }

  if ($ldap_server->bind_method == LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT) {
    $results_tables['basic'][] = array(t('Binding with DN for non-anonymous search (%bind_dn).  Using password ',
      array('%bind_dn' =>  $ldap_server->binddn)) . ' ' . $bindpw_type);
  }
  else {
    $results_tables['basic'][] = array(t('Binding with null DN for anonymous search.'));
  }

  if (@$values['grp_test_grp_dn_writeable'] && @$values['grp_test_grp_dn']) {
    $user_test_dn = @$values['grp_test_grp_dn'];
    $group_create_test_dn = $values['grp_test_grp_dn_writeable'];
    $group_create_test_attr = array(
      'objectClass' => array($ldap_server->groupObjectClass, 'top'),
    );

    // 1. delete test group if it exists
    if ($ldap_server->dnExists($group_create_test_dn, 'ldap_entry', array('cn', 'member'))) {
      $result = $ldap_server->groupRemoveGroup($group_create_test_dn, FALSE);
    }

    $group_exists = $ldap_server->dnExists($group_create_test_dn, 'ldap_entry', array('cn', 'member'));
    $result = ($group_exists === FALSE) ? "PASS" : "FAIL";
    $results_tables['group1'][] = array("Starting test without group: $group_create_test_dn ", $result);

    // 2. make sure call to members in empty group returns false
    $result = $ldap_server->groupAllMembers($group_create_test_dn);
    $result = ($result === FALSE) ? "PASS" : 'FAIL';
    $results_tables['group1'][] = array("LdapServer::groupAllMembers($group_create_test_dn) call on nonexistent group returns FALSE", $result);

    // 3. add group
    $result = $ldap_server->groupAddGroup($group_create_test_dn, $group_create_test_attr);
    $result = ($result)  ? "PASS" : 'FAIL';
    $attr = serialize($group_create_test_attr);
    $results_tables['group1'][] = array("LdapServer::groupAddGroup($group_create_test_dn, $attr)", $result);

    // 4. call to all members in an empty group returns emtpy array, not FALSE
    $result = $ldap_server->groupAllMembers($group_create_test_dn);
    $result = (is_array($result) && count($result) == 0)  ? 'PASS' : 'FAIL';
    $results_tables['group1'][] = array("LdapServer::groupAllMembers($group_create_test_dn) returns empty array for empty group ", $result);

    // 5. add member to group
    $result = $ldap_server->groupAddMember($group_create_test_dn, $user_test_dn);
    $result = is_array($ldap_server->groupAllMembers($group_create_test_dn)) ? 'PASS' : 'FAIL';
    $results_tables['group1'][] = array("LdapServer::groupAddMember($group_create_test_dn, $user_test_dn)", $result);

   // 6. try to remove group with member in it
    $only_if_group_empty = TRUE;
    $result = $ldap_server->groupRemoveGroup($group_create_test_dn, $only_if_group_empty);
    $result = ($result) ? 'FAIL' : 'PASS';
    $results_tables['group1'][] = array("LdapServer::groupRemoveGroup($group_create_test_dn, $only_if_group_empty)", $result);

   // 7. remove group member
    $result = $ldap_server->groupRemoveMember($group_create_test_dn, $user_test_dn);
    $result = $ldap_server->groupAllMembers($group_create_test_dn);
    $result = (is_array($result) && count($result) == 0) ? 'PASS' : 'FAIL';
    $results_tables['group1'][] = array("LdapServer::groupRemoveMember($group_create_test_dn, $user_test_dn)", $result);

    $only_if_group_empty = TRUE;
    $result = $ldap_server->groupRemoveGroup($group_create_test_dn, $only_if_group_empty);
    $result = ($ldap_server->dnExists($group_create_test_dn, 'ldap_entry', array('cn', 'member'))) ?  "FAIL" : 'PASS';
    $results_tables['group1'][] = array("LdapServer::groupRemoveGroup($group_create_test_dn, $only_if_group_empty)", $result);
  }

  // connect to ldap
  list($has_errors, $more_results) = ldap_servers_test_binding_credentials($ldap_server, $bindpw, $results_tables);
  $results = array_merge($results, $more_results);
  if ($ldap_server->bind_method == LDAP_SERVERS_BIND_METHOD_ANON_USER) {
    list($has_errors, $more_results, $ldap_user) = ldap_servers_test_user_mapping($values['testing_drupal_username'], $ldap_server);
    $results = array_merge($results, $more_results);
    if (!$has_errors) {
      $mapping[] = "dn = " . $ldap_user['dn'];
      foreach ($ldap_user['attr'] as $key => $value) {
        if (is_array($value)) {
          $mapping[] = "$key = " . $value[0];
        }
      }
      $results_tables['basic'][] =  array(theme_item_list(array('items' => $mapping, 'title' => t('Attributes available to anonymous search', array('%bind_dn' =>  $ldap_server->binddn)) , 'type' => 'ul', 'attributes' => array())));
    }
    $results_tables['basic'][] = array(t('Binding with DN (%bind_dn).  Using supplied password ',
      array('%bind_dn' =>  $ldap_user['dn'])));
    $result = $ldap_server->bind($ldap_user['dn'], $values['testing_drupal_userpw'], FALSE);
    if ($result == LDAP_SUCCESS) {
      $results_tables['basic'][] = array(t('Successfully bound to server'), 'PASS');
    }
    else {
      $results_tables['basic'][] = array(t('Failed to bind to server. ldap error #') . $result . ' ' . $ldap_server->errorMsg('ldap'), 'FAIL') ;
    }
  }

  if (!$has_errors && isset($values['grp_test_grp_dn'])) {
    $group_dn = $values['grp_test_grp_dn'];

    $result = @ldap_read($ldap_server->connection, $group_dn, 'objectClass=*');
    if (is_array($result)) {
      $group_entry = ldap_get_entries($ldap_server->connection, $result);
    }
    $user = isset($values['testing_drupal_username']) ? $values['testing_drupal_username'] : NULL;

    foreach (array(FALSE, TRUE) as $nested) { //FALSE
      $nested_display = ($nested) ? 'Yes' : 'No';
      if ($user) {
        // this is the parent function that will call FromUserAttr or FromEntry
        $memberships = $ldap_server->groupMembershipsFromUser($user, 'group_dns', $nested);
        $result = theme('item_list', array('items' => $memberships, 'type' => 'ul'));
        $results_tables['group2'][] = array("ldap_server->groupMembershipsFromUser($user, 'group_dns', nested=$nested_display)<br/>count=" . count($memberships), $result);

        $result = ($ldap_server->groupIsMember($group_dn, $user, $nested)) ? 'Yes' : 'No';
        $group_results[] = array("ldap_server->groupIsMember($group_dn, $user, nested=$nested_display)", $result);

        if ($ldap_server->groupUserMembershipsConfigured) {
          $groupusermembershipsfromuserattr = $ldap_server->groupUserMembershipsFromUserAttr($user, $nested);
          $count =  count($groupusermembershipsfromuserattr);
          $result = theme('item_list', array('items' => $groupusermembershipsfromuserattr, 'type' => 'ul'));
        }
        else {
          $groupusermembershipsfromuserattr = array();
          $result = "'A user LDAP attribute such as memberOf exists that contains a list of their group' is not configured.";
        }
        $results_tables['group2'][] = array("ldap_server->groupUserMembershipsFromUserAttr($user, nested=$nested_display)<br/> count=" .
            count($groupusermembershipsfromuserattr) , $result);

        if ($ldap_server->groupGroupEntryMembershipsConfigured) {
          $groupusermembershipsfromentry = $ldap_server->groupUserMembershipsFromEntry($user, $nested);
          $result = theme('item_list', array('items' => $groupusermembershipsfromentry, 'type' => 'ul'));
        }
        else {
          $groupusermembershipsfromentry = array();
          $result = "Groups by entry not configured.";
        }
        $results_tables['group2'][] = array("ldap_server->groupUserMembershipsFromEntry($user, nested=$nested_display)<br/>count=" .
          count($groupusermembershipsfromentry) , $result);

        if (count($groupusermembershipsfromentry) && count($groupusermembershipsfromuserattr)) {
          $diff1 = array_diff($groupusermembershipsfromuserattr, $groupusermembershipsfromentry);
          $diff2 = array_diff($groupusermembershipsfromentry, $groupusermembershipsfromuserattr);
          $result1 = theme('item_list', array('items' => $diff1, 'type' => 'ul'));
          $result2 = theme('item_list', array('items' => $diff2, 'type' => 'ul'));
          $results_tables['group2'][] = array("groupUserMembershipsFromEntry and FromUserAttr Diff)", $result1);
          $results_tables['group2'][] = array("FromUserAttr and groupUserMembershipsFromEntry Diff)", $result2);
        }
      }
    }


    if ($groups_from_dn = $ldap_server->groupUserMembershipsFromDn($user)) {
      $results_tables['groupfromDN'][] = array("Groups from DN", theme('item_list', array('items' => $groups_from_dn, 'type' => 'ul')));
    }

  }

  list($has_errors, $more_results, $ldap_user) = ldap_servers_test_user_mapping($values['testing_drupal_username'], $ldap_server);

  $tokens = ($ldap_user && isset($ldap_user['attr'])) ? ldap_servers_token_tokenize_entry($ldap_user['attr'], 'all') : array();
  foreach ($tokens as $key => $value) {
    $results_tables['tokens'][] = array($key, $value);
  }
  $form_state['ldap_server_test_data'] = array(
    'username' => $values['testing_drupal_username'],
    'results_tables' => $results_tables,
  );

  if (isset($ldap_user)) {
    $form_state['ldap_server_test_data']['ldap_user'] = $ldap_user;
  }

  if (isset($group_entry) ) {
    $form_state['ldap_server_test_data']['group_entry'] = $group_entry;
  }
  
}

/**
 * Submit hook for the LDAP server form.
 */


function ldap_servers_test_binding_credentials(&$ldap_server, $bindpw, &$results_tables) {

  $errors = FALSE;
  $results = array();

  $ldap_result = $ldap_server->connect();
  if ($ldap_result != LDAP_SUCCESS) {
    $results_tables['basic'][] = array(t('Failed to connect to LDAP server.  See watchdog error logs for details.') .
                                    $ldap_server->errorMsg('ldap'));
    $errors = TRUE;
  }

  if (!$errors) {
    $bind_result = $ldap_server->bind($ldap_server->binddn, $bindpw, FALSE);
    if ($bind_result == LDAP_SUCCESS) {
      $results_tables['basic'][] =  array(t('Successfully bound to server'));
    }
    else {
      $results_tables['basic'][] = array(t('Failed to bind to server. ldap error #') . $bind_result . ' ' . $ldap_server->errorMsg('ldap')) ;
      $errors = TRUE;
    }
  }
  return array($errors, $results);
}

function ldap_servers_test_user_mapping($drupal_username, &$ldap_server, $ldap_context = NULL) {

  $ldap_user = $ldap_server->userUserNameToExistingLdapEntry($drupal_username, $ldap_context);
  $errors = FALSE;
  if (!$ldap_user ) {
    $results[] = t('Failed to find test user %username by searching on  %user_attr = %username.',
      array(
        '%username' => $drupal_username,
        '%user_attr' => $ldap_server->user_attr )
      )
      . ' ' . t('Error Message:') . ' ' . $ldap_server->errorMsg('ldap');
    $errors = TRUE;
  }
  else {
    $results[] = t('Found test user %username by searching on  %user_attr = %username.',
      array('%username' => $drupal_username, '%user_attr' => $ldap_server->user_attr ));
  }
  return array($errors, $results, $ldap_user);
}
