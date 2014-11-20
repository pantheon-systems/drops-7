<?php

/**
 * @file
 * Admin related functionality for mailup module
 */

function mailup_settings_subform_signup(&$element, &$form_state) {

  $element['user_agent'] = array(
    '#type' => 'value',
    '#value' => $_SERVER['HTTP_USER_AGENT']
  );

  $element['ip_address'] = array(
    '#type' => 'value',
    '#value' => $_SERVER['REMOTE_ADDR']
  );

  global $base_url;
  $element['referrer'] = array(
    '#type' => 'value',
    '#value' => $base_url
  );

  // Determine cost centre
  $country = variable_get('site_default_country', '');

  $eurozone = array(
    'AT', 'BE', 'HR', 'CY', 'CZ', 'DK', 'EE', 'FI', 'FR',
    'DE', 'GR', 'HU', 'IE', 'IT', 'LV', 'LT', 'LU', 'MT',
    'NL', 'PL', 'PT', 'RO', 'SK', 'SI', 'ES', 'SE', 'GB'
  );

  $element['is_usa'] = array(
    '#type' => 'value',
    '#value' => !in_array($country, $eurozone)
  );

  $mailup_language = NULL;
  // If site language matches mailup supported language, use that
  $language = language_default();
  
  if (in_array($language->language, array('en', 'es', 'it'))) {
    $element['language'] = array(
      '#type' => 'value',
      '#value' => strtoupper($language->language)
    );
  }
  else {
    $element['language'] = array(
      '#type' => 'select',
      '#title' => t('Language'),
      '#options' => array('EN' => t('English'), 'ES' => t('Spanish'), 'IT' => t('Italian'))
    );
  }

  global $user;

  $element['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Full name'),
    '#default_value' => isset($form_state['values']['name']) ? $form_state['values']['name'] : '',
    '#required' => TRUE
  );

  $element['company'] = array(
    '#type' => 'textfield',
    '#title' => t('Company Name'),
    '#default_value' => isset($form_state['values']['company']) ? $form_state['values']['company'] : '',
    '#required' => TRUE
  );

  $element['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email address'),
    '#default_value' => isset($form_state['values']['email']) ? $form_state['values']['email'] : $user->mail,
    '#required' => TRUE
  );

  $element['telephone'] = array(
    '#type' => 'textfield',
    '#title' => t('Telephone number'),
    '#required' => TRUE
  );

  $element['newsletter'] = array(
    '#type' => 'checkbox',
    '#title' => t('Sign me up for the MailUp Newsletter')
  );

  $element['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Sign Up'),
    '#suffix' => '<p><em>' . t("By clicking this button, you agree to MailUp's Privacy Policy and <a href=\"@url\" onclick=\"window.open(this.href)\">Terms & Conditions.</a>", array('@url' => 'http://www.mailup.com/terms')) . '</em></p>',
  );
}

function mailup_settings_subform_credentials(&$element, &$form_state) {

  $element['mailup_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#default_value' => empty($form_state['values']['mailup_username']) ? variable_get('mailup_username', '') : $form_state['values']['mailup_username'],
    '#description' => t('MailUp account username'),
    '#required' => TRUE
  );

  $element['mailup_password'] = array(
    '#type' => 'textfield',
    '#title' => t('Password'),
    '#description' => t('MailUp account password'),
    '#required' => TRUE
  );

  $element['mailup_console_url'] = array(
    '#type' => 'textfield',
    '#title' => t('MailUp Console URL'),
    '#default_value' => empty($form_state['values']['mailup_console_url']) ? variable_get('mailup_console_url', '') : $form_state['values']['mailup_console_url'],
    '#required' => TRUE,
    '#description' => t('Log into your MailUp admin console and look at the first part of the browser address field. It will look something like a4h4i.s21.it (or alike). That is your console URL.'),
  );

  $element['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save')
  );

  // Allow user to check that the API credentials are correct
  if ($config = mailup_get_config()) {
    $element['check'] = array(
      '#type' => 'submit',
      '#value' => t('Test Connection'),
      '#submit' => array('mailup_settings_form_check_credentials'),
      // We don't want to validate, as we have a config already
      '#limit_validation_errors' => array(),
    );
  }

}

function mailup_settings_form_check_credentials($form, &$form_state) {
  
  $client = mailup_get_client();

  try {
    $client->checkAuth(TRUE);
    drupal_set_message(t('Successfully authenticated with MailUp API.'));
  }
  catch (MailupException $e) {
    drupal_set_message(t('Unable to authenticate with the MailUp API. Please check the settings below.'), 'warning');
    drupal_set_message($e->getMessage(), 'error');
  }
}

function mailup_settings_form_validate($form, &$form_state) {
  
  $values = $form_state['values'];

  // Trial signup validation
  if ($form_state['#initial_setup'] && $values['initial'] == 'new') {
    if (!valid_email_address($values['email'])) {
      form_set_error('email', t('Please provide a valid email address.'));
    }
  }
  else {
    $console_url = $values['mailup_console_url'];

    if (!valid_url($console_url, TRUE)) {
      form_set_error('mailup_console_url', t('Please provide a valid URL.'));
    }
    else {
      // Normalize URL so we can pass it straight to API Client
      $parsed_url = parse_url($console_url);
      $hostname = empty($parsed_url['host']) ? $console_url : $parsed_url['host'];
      form_set_value($form['details']['mailup_console_url'], 'http://' . $hostname . '/', $form_state);
    }
  }
  
}

function mailup_settings_form_submit($form, &$form_state) {

  // Handle demo signup
  if ($form_state['#initial_setup'] && $form_state['values']['initial'] == 'new') {

    $values = $form_state['values'];

    $data = array();
    $data['RegisterToNewsletter'] = $values['newsletter'] ? 'true' : 'false';
    $data['IsUSA'] = $values['is_usa'] ? 'true' : 'false';

    // Map the rest of the variables directly
    $variable_map = array(
      'ip_address' => 'IpAddress',
      'company' => 'Company',
      'language' => 'Language',
      'name' => 'NameAndSurname',
      'telephone' => 'PhoneNumber',
      'email' => 'Email',
      'user_agent' => 'UserAgent',
      'referrer' => 'Referer'
    );

    foreach ($variable_map as $local_name => $remote_name) {
      $data[$remote_name] = $form_state['values'][$local_name];
    }

    if ($client = mailup_get_client(FALSE)) {
      try {
        if ($result = $client->requestTrialActivation($data)) {
          variable_set('mailup_trial_request', array('id' => $result['Id'], 'hash' => $result['Hash']));
        }
      }
      catch (MailupException $e) {
        drupal_set_message($e->getMessage(), 'error');
        $form_state['rebuild'] = TRUE;
      }
    }

  }
  else {
    // Clear any existing tokens when changing details
    // Otherwise MailUp will continue to auth on stored token
    MailUp::clearSessionTokens();

    // When changing mailup account
    if ($form_state['values']['mailup_username'] != variable_get('mailup_username', '')) {
      // Delete all existing subscriptions
      $mailup_subscriptions = mailup_subscription_load_multiple(FALSE);
      foreach ($mailup_subscriptions as $mailup_subscription) {
        $mailup_subscription->delete();
      }

      // Delete all existing lists
      $mailup_lists = mailup_list_load_multiple(FALSE);
      foreach ($mailup_lists as $mailup_list) {
        $mailup_list->delete();
      }

      // And truncate the subscription data for the existing account
      db_truncate('mailup_users_recipients')->execute();
      db_truncate('mailup_recipient_subscriptions')->execute();

      // Clear cached remote lists
      variable_set('mailup_remote_lists', array());
      // Field mapping
      variable_set('mailup_subscribe_recipient_field_map', array());
      // And remote field list
      variable_set('mailup_subscribe_recipient_fields', array());
    }

    system_settings_form_submit($form, $form_state);
  }
}

/**
 * Callback for ajax_form_callback() in mailup_settings_form
 */
function mailup_settings_form_ajax_callback($form, $form_state) {
  // The details fieldset will always contain the relevant subform
  return $form['details'];
}

/**
 * Main configuration form for MailUp
 */
function mailup_settings_form($form, &$form_state) {

  $username = variable_get('mailup_username');
  $trial_request = _mailup_get_trial_request_id();
  $form_state['#initial_setup'] = FALSE;

  if (empty($username)) {

    $form_state['#initial_setup'] = empty($trial_request);

    if (!$form_state['#initial_setup']) {

      $form['trial_info'] = array(
        '#type' => 'fieldset',
        '#title' => t('Your Trial Activation Status')
      );

      // Query MailUp for trial status and display message
      if ($client = mailup_get_client(FALSE)) {
        $activation_status = $client->getTrialActivationStatus($trial_request['id'], $trial_request['hash']);

        $form['trial_info']['message'] = array(
          '#type' => 'markup',
          '#prefix' => '<p>',
          '#markup' => t($activation_status['message']),
          '#suffix' => '</p>'
        );
      }
    }
  }

  $form['details'] = array(
    '#type' => 'fieldset',
    '#title' => t('API Credentials'),
    '#prefix' => '<div id="mailup-details">',
    '#suffix' => '</div>',
    '#weight' => 1
  );

  // Show initial setup options
  if ($form_state['#initial_setup']) {

    $form['overview'] = array(
      '#type' => 'fieldset',
      '#title' => t('Initial setup'),
      '#weight' => 0
    );

    $form['overview']['initial'] = array(
      '#type' => 'radios',
      '#title' => t('How would you like to configure MailUp?'),
      '#options' => array(
        'new' => t('Create a new demo account.'),
        'existing'   => t('Use an existing MailUp account.')
      ),
      '#default_value' => empty($form_state['values']['initial']) ? 'new' : $form_state['values']['initial'],
      '#ajax' => array(
        'callback' => 'mailup_settings_form_ajax_callback',
        'wrapper' => 'mailup-details'
      ),
    );

    // Get value from element (Safe as it reflects the form state).
    if ($form['overview']['initial']['#default_value'] == 'new') {
      $form['details']['#title'] = t('Sign up for a free 30 day trial');
      mailup_settings_subform_signup($form['details'], $form_state);
    }
    else {
      mailup_settings_subform_credentials($form['details'], $form_state);
    }
  }
  else {
    mailup_settings_subform_credentials($form['details'], $form_state);
  }

  return $form;
}



