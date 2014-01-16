<?php

/**
 * @file
 * Administrative forms and functionality for Kameleoon module.
 */

/**
 * Form builder: Kameleoon administration.
 */
function kameleoon_admin_form($form, &$form_state) {
  $form = array();
  $form['general_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
  );
  $form['general_settings']['kameleoon_site_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Site code'),
    '#description' => t('Code for this site generated with Kameleoon. After <a href="!sign_up">signing up with Kameleoon</a> you can obtain a code by visiting the "Technical" and "Sites" tab in the administration interface.',
      array(
        '!sign_up' => 'http://www.kameleoon.com/en/sign-up.html',
      )),
    '#default_value' => variable_get('kameleoon_site_code', ''),
    '#required' => TRUE,
  );

  $form['general_settings']['kameleoon_password'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
    '#maxlength' => 60,
    '#required' => TRUE,
    '#attributes' => array('class' => array('password-field')),
  );

  $form['#validate'][] = 'kameleoon_admin_form_validate';
  $form['#submit'][] = 'kameleoon_admin_form_submit_invalidate_page_cache';

  return system_settings_form($form);
}


/**
 * Validate handler: Kameleoon administration.
 *
 * Validates the communication between Drupal Commerce and Kameleoon.
 */
function kameleoon_admin_form_validate($form, &$form_state) {
  // Performs a request to Kameleoon api to validate the communication.
  $response = NULL;
  try {
    $request_options = array(
      'task' => 'assignAcquisitionSource',
      'siteCode' => $form_state['values']['kameleoon_site_code'],
      'password' => $form_state['values']['kameleoon_password'],
      'financialData.acquisitionSource' => 'COMMERCE_GUYS',
    );
    $response = drupal_http_request(url('https://api.kameleoon.com/api', array("query" => $request_options)));
    if (isset($response->code) && $response->code != 200) {
      drupal_set_message(t('An error occured: %error.', array('%error' => $response->data)), 'error');
      watchdog('kameleoon', 'Erro to connect the API. Error information: <pre>@debug</pre>', array('@debug' => print_r($response, TRUE),), WATCHDOG_ERROR);
    }
  }
  catch (Exception $e) {
    watchdog('kameleoon', 'Unable to connect the API. Error information: <pre>@debug</pre>', array('@debug' => print_r($e->getMessage(), TRUE),), WATCHDOG_ERROR);
  }
}

/**
 * Submit handler: Kameleoon administration. Invalidates page cache.
 */
function kameleoon_admin_form_submit_invalidate_page_cache($form, &$form_state) {
  // Invalidate page cache only if it's enabled and anonymous users can access
  // to Kameleoon tools.
  $anonymous_permissions = user_role_permissions(array('1' => 'anonymous user'));
  if (!variable_get('cache') || !isset($anonymous_permissions[1]['access kameleoon tools'])) {
    return;
  }
  // Check whether the site code has been changed.
  if ($form['general_settings']['kameleoon_site_code']['#default_value'] !== $form_state['values']['kameleoon_site_code']) {
    // Clear the page cache, so that anonymous users will get the JS included
    // with using the new site code right away.
    cache_clear_all('*', 'cache_page', TRUE);
    drupal_set_message('Page cache has been cleared.');
  }
}
