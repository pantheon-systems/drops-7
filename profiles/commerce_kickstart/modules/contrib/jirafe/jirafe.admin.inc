<?php

/**
 * @file
 * Configuration for Jirafe module.
 */

/**
 * Displays Jirafe configuration.
 */
function jirafe_dashboard_admin_page($form, $form_state) {
  // TODO: Add API application and user managing.
  $form = array();

  $params = array(
    'app_id' => '',
    'token' => '',
    'sites' => array(),
    'users' => array(),
  );

  $config = variable_get('jirafe_config', $params);

  $form['app_info'] = array(
    '#title' => t('Application information'),
    '#type' => 'fieldset',
  );
  $form['app_info']['app_id'] = array(
    '#title' => t('Application ID'),
    '#type' => 'textfield',
    '#default_value' => $config['app_id'],
    '#required' => TRUE,
  );
  $form['app_info']['token'] = array(
    '#title' => t('Application token'),
    '#type' => 'textfield',
    '#default_value' => $config['token'],
    '#required' => TRUE,
  );

  if (!empty($config['sites'])) {
    $form['site_info'] = array(
      '#title' => t('Site information'),
      '#type' => 'fieldset',
    );
    foreach ($config['sites'] as $site) {
      $form['site_info'][$site['site_id']] = array(
        '#title' => $site['description'],
        '#type' => 'fieldset',
      );
      $form['site_info'][$site['site_id']]['site_id'] = array(
        '#title' => t('Site ID'),
        '#type' => 'item',
        '#markup' => check_plain($site['site_id']),
        '#attributes' => array('readonly' => 'readonly'),
      );
      $form['site_info'][$site['site_id']]['site_url'] = array(
        '#title' => t('Site URL'),
        '#type' => 'item',
        '#markup' => check_plain($site['url']),
        '#attributes' => array('readonly' => 'readonly'),
      );
    }
  }

  if (!empty($config['users'])) {
    $form['user_info'] = array(
      '#title' => t('User information'),
      '#type' => 'fieldset',
    );
    foreach ($config['users'] as $index => $user) {
      $form['user_info']['user_' . $index] = array(
        '#title' => "{$user['first_name']} {$user['last_name']}",
        '#type' => 'fieldset',
      );
      $form['user_info']['user_' . $index]['user_mail'] = array(
        '#title' => t('User mail'),
        '#type' => 'item',
        '#markup' => check_plain($user['email']),
        '#attributes' => array('readonly' => 'readonly'),
      );
    }
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  if (!empty($config['app_id'])) {
    $form['actions']['reset'] = array(
      '#type' => 'submit',
      '#value' => t('Reset'),
      '#submit' => array('jirafe_dashboard_admin_page_reset'),
    );
  }

  return $form;
}

/**
 * Submit callback for the Jirafa settings reset button.
 *
 * Nullify the Jirafe configuration.
 */
function jirafe_dashboard_admin_page_reset($form, $form_state) {
  variable_set('jirafe_config', NULL);
}

/**
 * Submit callback for the Jirafa settings form.
 *
 * Overwrite the Jirafe token and application ID.
 */
function jirafe_dashboard_admin_page_validate($form, $form_state) {
  if (!is_numeric(trim($form_state['values']['app_id']))) {
    form_set_error('app_id', t("The 'Application ID' should only contains numeric characters."));
  }
}

/**
 * Submit callback for the Jirafa settings form.
 *
 * Overwrite the Jirafe token and application ID.
 */
function jirafe_dashboard_admin_page_submit($form, $form_state) {
  $config = variable_get('jirafe_config');
  $config['token'] = trim($form_state['values']['token']);
  $config['app_id'] = trim($form_state['values']['app_id']);
  variable_set('jirafe_config', $config);

  // Try creating a request with the new configuration to verify it.
  $client = new Jirafe_Client($config['token']);
  $result = $client->applications($config['app_id']);
  try {
    // This should trigger an exception if the token and app_id don't match.
    $result->fetch();

    // If the request was successful, redirect to the dashboard.
    drupal_set_message(t('Jirafe configuration successfully updated.'), 'status');

    // This will synchronize users and sites between the actual site and
    // Jirafe's servers.
    jirafe_synchronize();

    // Redirect user to the dashboard.
    drupal_goto('admin/commerce/jirafe');
  }
  catch (Exception $e) {
    drupal_set_message(t('The credentials could not be verfied, please try again.'), 'error');
  }

}
