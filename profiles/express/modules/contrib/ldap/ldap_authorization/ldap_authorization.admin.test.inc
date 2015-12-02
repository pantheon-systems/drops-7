<?php
// $Id: ldap_authorization.admin.test.inc,v 1.3 2010/12/29 01:37:46 johnbarclay Exp $

/**
 * @file
 * form to test a ldap authorization consumer configuration
 */

/**
 * form for adding, updating, and deleting a single ldap authorization consumer configuration
 *
 * @param <type> $form
 * @param <type> $form_state
 * @return array drupal form array
 */


function ldap_authorization_test_form($form, &$form_state, $consumer_type) {
  $consumer = ldap_authorization_get_consumer_object($consumer_type);
  $consumer_tokens = ldap_authorization_tokens($consumer);

  if (isset($_SESSION['ldap_authorization_test_query']['result'])) {
    $form['result'] = array(
      '#type' => 'item',
      '#markup' => $_SESSION['ldap_authorization_test_query']['result'],
      );

    $form['pre-filtered'] = array(
      '#type' => 'fieldset',
      '#title' => t('Prefiltered and Final Mappings'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['pre-filtered']['overview'] = array(
     '#type' => 'item',
     '#markup' => t('Below are the "raw authorizations" and which strategy they are derived from.  If enabled, the mapping filter is applied to these.'),
    );

    if (isset($_SESSION['ldap_authorization_test_query']['maps'])) {
      $i = 0;
      foreach ($_SESSION['ldap_authorization_test_query']['maps'] as $map => $data) {
        $i++;
        $form['pre-filtered']['map' . $i] = array(
         '#type' => 'item',
         '#markup' => is_scalar($data) ?  "<h2>$map (without filter)</h2>" . $data : theme('item_list', array('items' => $data, 'type' => 'ul', 'title' => "$map (without filter)")),
        );
      }
    }
  }

  $form['intro'] = array(
      '#type' => 'item',
      '#markup' => t('<h1>Test LDAP to !consumer_name Configuration</h1>
      <p>This form will not actually grant any authorizations, its just to show
      what authorizations would be granted with this configuration.</p>', $consumer_tokens),
  );

  $form['consumer_type'] = array(
    '#type' => 'hidden',
    '#default_value' => $consumer_type,
  );

  $form['usernames'] = array(
    '#type' => 'textarea',
    '#title' => t('Drupal usernames to test !consumer_shortName authorizations results for.  One per line.', $consumer_tokens),
    '#default_value' => @$_SESSION['ldap_authorization_test_query']['usernames'] ,
    '#cols' => 50,
    '#rows' => 6,
    '#description' => t('', $consumer_tokens),
  );

  $form['random_users'] = array(
    '#type' => 'checkbox',
    '#default_value' => @$_SESSION['ldap_authorization_test_form']['random_users'],
    '#title' => t('Use 10 random users', $consumer_tokens),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'test',
  );
  unset($_SESSION['ldap_authorization_test_query']);
  return $form;
}


/**
 * validate handler for the ldap_authorization_test
 */
function ldap_authorization_test_form_validate($form, &$form_state) {

  $values = $form_state['values'];
  if (!($values['usernames'] || $values['random_users'])) {
    form_set_error('No options chosen', t('Usernames must be given or random users checked.'));
  }

}

/**
 * submit handler function for ldap_authorization_test
 */


function ldap_authorization_test_form_submit($form, &$form_state) {

  $consumer_type = $form_state['values']['consumer_type'];
  $consumer = ldap_authorization_get_consumer_object($consumer_type);
  $consumer_tokens = ldap_authorization_tokens($consumer);

  $results = array();
  $users_listed = $array = preg_split('/[\n\r]+/', $form_state['values']['usernames']);
  $random_users = array();
  $_SESSION['ldap_authorization_test_query']['random_users'] = $form_state['values']['random_users'];
  $_SESSION['ldap_authorization_test_query']['usernames'] = $form_state['values']['usernames'];
  if ($form_state['values']['random_users']) {  // add 10 random usernames to test
    // not using user_load_multiple because need randomness outside of query
    $select = db_select('users', 'u');
    $select->fields('u');

    try {
      $random_users = $select->execute()->fetchAllAssoc('name',  PDO::FETCH_ASSOC);
    }
    catch (Exception $e) {
      drupal_set_message(t('db users query failed. Message = %message, query= %query',
        array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
      return "";
    }

  }


  $user_names = array_unique(array_merge(array_keys($random_users), $users_listed));
  $i = 0;
  foreach ($user_names as $username) {
    if ($username) {
      if (!$user = user_load_by_name($username)) {
        // if not existing user, create fake user assumed to be ldap authenticated
        $user = new stdClass();
        $user->name = $username;
        $user->ldap_test = TRUE;
        $user->ldap_authenticated = TRUE;
      }
      else {
        if (function_exists('dpm')) {
          dpm("user: $username"); dpm($user);
        }
      }
      list($results[$username], $notifications[$username]) = ldap_authorizations_user_authorizations($user, 'test_query', $consumer_type, 'logon');
      // remove authorizations from other consumer types
      $results[$username] = array($consumer_type => $results[$username][$consumer_type]);
      $i++;
      if ($i == 10) {
        break;
      }

    }
  }

  $table =  theme('ldap_authorization_test_results', array('results' => $results, 'consumer' => $consumer, 'notifications' => $notifications));
  $_SESSION['ldap_authorization_test_query']['result'] = $table;
  $form_state['redirect'] = LDAP_SERVERS_MENU_BASE_PATH . '/authorization/test/' . $consumer->consumerType;

}
