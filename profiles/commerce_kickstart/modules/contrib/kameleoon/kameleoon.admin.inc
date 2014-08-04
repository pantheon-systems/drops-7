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
    '#size' => 10,
    '#maxlength' => 10,
    '#description' => t('<p>After <a href="!sign_up" target="_blank">signing-up</a>, Kameleoon will generate a small JS snippet that contains a unique key code (alphanumeric number) for your website (in blue below). You can find it by clicking on the "Parameters" tab in the administration interface and by choosing your site.</p><p>You can also read our <a href="!how_to_install" target="_blank">documentation</a> to get started.</p><code><i>&lt;script type="text/javascript" src="//static.kameleoon.com/css/customers/<span style="color:#3192dc;"><b>XXXXXXXXXX</b></span>/0/kameleoon.js"&gt;&lt;/script &gt;</i></code>',
        array(
          '!sign_up' => 'http://www.kameleoon.com/en/pricing.html',
          '!how_to_install' => 'http://www.kameleoon.com/documentation/en/How_to_install_the_Kameleoon_plugin_on_a_Commerce_Guys_website%3F',
        )),
    '#default_value' => variable_get('kameleoon_site_code', ''),
    '#required' => TRUE,
    '#attributes' => array('class' => array('site-code-field')),
  );

  $form['general_settings']['kameleoon_password'] = array(
    '#type' => 'textfield',
    '#title' => t('Password'),
    '#maxlength' => 60,
    '#default_value' => variable_get('kameleoon_password', ''),
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
  $siteCode = $form_state['values']['kameleoon_site_code'];

  if (strlen($siteCode) != 10){
    form_set_error('kameleoon_site_code', t('The Site Code must be 10 characters'));
  }

  if (!preg_match('/[!@#$%*a-zA-Z0-9]{8,}/', $siteCode)){
    form_set_error('kameleoon_site_code', t('The Site Code must have letters and numbers'));
  }

  try {
    $request_options = array(
      'task' => 'assignAcquisitionSource',
      'siteCode' => $form_state['values']['kameleoon_site_code'],
      'password' => $form_state['values']['kameleoon_password'],
      'financialData.acquisitionSource' => 'COMMERCE_GUYS',
    );

    $response = drupal_http_request(url('https://api.kameleoon.com/api', array("query" => $request_options)));

    if ($response->code != 200) {
      form_set_error('general_settings', t('@response', array('@response' => $response->data)));
      watchdog('kameleoon', 'Error to connect the API. Error information: <pre>@debug</pre>', array('@debug' => print_r($response, TRUE),), WATCHDOG_ERROR);
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
