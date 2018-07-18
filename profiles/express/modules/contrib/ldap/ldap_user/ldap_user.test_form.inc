<?php

/**
 * @file
 */

/**
 * Implements the LDAP user test page.
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

function ldap_user_test_form($form, &$form_state, $op = NULL) {

  $username = @$_SESSION['ldap_user_test_form']['testing_drupal_username'];

  $form['#prefix'] = t('<h1>Test LDAP User Configuration</h1>');

  $form['#prefix'] .= t('This form simply tests an LDAP User configuration against an individual ldap or drupal user.
    It makes no changes to the drupal or ldap user.');

  $form['testing_drupal_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Testing Drupal Username'),
    '#default_value' => $username,
    '#required' => 1,
    '#size' => 30,
    '#maxlength' => 255,
    '#description' => t('This is optional and used for testing this server\'s configuration against an actual username.  The user need not exist in Drupal and testing will not affect the user\'s LDAP or Drupal Account.'),
  );

  $form['test_mode'] = array(
      '#type' => 'radios',
      '#title' => t('Testing Mode'),
      '#required' => 0,
      '#default_value' => isset( $_SESSION['ldap_user_test_form']['test_mode']) ?  $_SESSION['ldap_user_test_form']['test_mode'] : 'query',
      '#options' => array(
        'query' => t('Test Query.  Will not alter anything in drupal or LDAP'),
        'execute' => t('Execute Action.  Will perform provisioning configured for events below.  If this is selected only one action should be selected below'),
      ),
    );

  $synch_trigger_options = ldap_user_synch_triggers_key_values();

  $selected_actions = isset($_SESSION['ldap_user_test_form']['action']) ? $_SESSION['ldap_user_test_form']['action'] : array();
  $form['action'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Actions/Event Handlers to Test'),
      '#required' => 0,
      '#default_value' => $selected_actions,
      '#options' => $synch_trigger_options,
      '#states' => array(
        'visible' => array(   // action to take.
          ':input[name="wsEnabled"]' => array('checked' => TRUE),
        ),
      ),
    );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'test',
    '#weight' => 100,
  );

  return $form;
}

function ldap_user_test_form_validate($form, &$form_state) {
  if ($form_state['values']['test_mode'] == 'execute'  &&
    count(array_filter($form_state['values']['action'])) > 1 ) {
    form_set_error('test_mode', t('Only one action may be selected for "Execute Action" testing mode.'));
  }


}

/**
 * Submit hook for the LDAP server form.
 */
function ldap_user_test_form_submit($form, &$form_state) {

  $username = $form_state['values']['testing_drupal_username'];
  $selected_actions = $form_state['values']['action'];

  if ($username && count($selected_actions) > 0) {

    $synch_trigger_options = ldap_user_synch_triggers_key_values();

    $user_object = user_load_by_name($username);
    if ($user_object) {
      $user_entities = entity_load('user', array($user_object->uid));
      $user_entity = $user_entities[$user_object->uid];
    }
    else {
      $user_entity = NULL;
    }

    $ldap_user_conf = ldap_user_conf();
    $test_servers = array();
    $user_ldap_entry = FALSE;
    if ($ldap_user_conf->drupalAcctProvisionServer) {
      $test_servers[LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER] = $ldap_user_conf->drupalAcctProvisionServer;
      $user_ldap_entry = ldap_servers_get_user_ldap_data($username, $ldap_user_conf->drupalAcctProvisionServer);
    }
    if ($ldap_user_conf->ldapEntryProvisionServer) {
      $test_servers[LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY] = $ldap_user_conf->ldapEntryProvisionServer;
      if (!$user_ldap_entry) {
        $user_ldap_entry = ldap_servers_get_user_ldap_data($username, $ldap_user_conf->ldapEntryProvisionServer);
      }
    }
    $results = array();
    $results['username'] = $username;
    $results['user object (before provisioning or synching)'] = $user_object;
    $results['user entity (before provisioning or synching)'] = $user_entity;
    $results['related ldap entry (before provisioning or synching)'] = $user_ldap_entry;
    $results['ldap_user_conf'] = $ldap_user_conf;

    if (is_object($user_object)) {
      $authmaps = db_query("SELECT aid, uid, module, authname FROM {authmap} WHERE uid = :uid", array(':uid' => $user_object->uid))->fetchAllAssoc('aid',  PDO::FETCH_ASSOC);
    }
    else {
      $authmaps = 'No authmaps available.  Authmaps only shown if user account exists beforehand';
      $user_object = new stdClass(); // need for testing.
      $user_object->name = $username;
    }
    $results['User Authmap'] = $authmaps;
    $results['LDAP User Configuration Object'] = $ldap_user_conf;

    $save = ($form_state['values']['test_mode'] == 'execute');
    $test_query = ($form_state['values']['test_mode'] != 'execute');
    $user_edit = array('name' => $username);

    foreach (array_filter($selected_actions) as $i => $synch_trigger) {
      $synch_trigger_description = $synch_trigger_options[$synch_trigger];
      foreach (array(LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER, LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY) as $direction) {
        if ($ldap_user_conf->provisionEnabled($direction, $synch_trigger)) {
          if ($direction == LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER) {
            $discard = $ldap_user_conf->provisionDrupalAccount(NULL, $user_edit, NULL, $save);
            $results['provisionDrupalAccount method results']["context = $synch_trigger_description"]['proposed'] = $user_edit;
          }
          else {
            $provision_result = $ldap_user_conf->provisionLdapEntry($user_object, NULL, $test_query);
            $results['provisionLdapEntry method results']["context = $synch_trigger_description"] = $provision_result;
          }
        }
        else {
          if ($direction == LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER) {
            $results['provisionDrupalAccount method results']["context = $synch_trigger_description"] = 'Not enabled.';
          }
          else {
            $results['provisionLdapEntry method results']["context = $synch_trigger_description"] = 'Not enabled.';
          }
        }
      }
    }
    // do all synchs second, in case logic of form changes to allow executing mulitple events
    foreach (array_filter($selected_actions) as $i => $synch_trigger) {
      $synch_trigger_description = $synch_trigger_options[$synch_trigger];
      foreach (array(LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER, LDAP_USER_PROV_DIRECTION_TO_LDAP_ENTRY) as $direction) {
        if ($ldap_user_conf->provisionEnabled($direction, $synch_trigger)) {
          if ($direction == LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER) {
            $discard = $ldap_user_conf->synchToDrupalAccount(NULL, $user_edit, NULL, $test_query);
            $results['synchToDrupalAccount method results']["context = $synch_trigger_description"]['proposed'] = $user_edit;
          }
          else { // to ldap
            $provision_result = $ldap_user_conf->synchToLdapEntry($user_object, $user_edit, array(), $test_query);
            $results['synchToLdapEntry method results']["context = $synch_trigger_description"] = $provision_result;
          }
        }
        else {
          if ($direction == LDAP_USER_PROV_DIRECTION_TO_DRUPAL_USER) {
            $results['synchToDrupalAccount method results']["context = $synch_trigger_description"] = 'Not enabled.';
          }
          else { // to ldap
            $results['synchToLdapEntry method results']["context = $synch_trigger_description"] = 'Not enabled.';
          }
        }
      }
    }


    if (function_exists('dpm')) {
      dpm($results);
    }
    else {
      drupal_set_message(t('This form will not display results unless the devel module is enabled.'), 'warning');
    }
  }

  $_SESSION['ldap_user_test_form']['action'] = $form_state['values']['action'];
  $_SESSION['ldap_user_test_form']['test_mode'] = $form_state['values']['test_mode'];
  $_SESSION['ldap_user_test_form']['testing_drupal_username'] = $username;

  $form_state['redirect'] =  LDAP_USER_TEST_FORM_PATH;

}
