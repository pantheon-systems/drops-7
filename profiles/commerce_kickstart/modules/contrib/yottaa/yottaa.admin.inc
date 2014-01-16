<?php
/**
 * @file
 * Administrative forms for Yottaa module.
 */

/**
 * Settings form.
 */
function yottaa_setup_form($form_state) {
  $form['introduction'] = array(
    '#type' => 'markup',
    '#markup' => '<p>' .
    t('Commerce Guys and Yottaa have partnered to bring fast, scalable and secure eCommerce acceleration
service to Drupal Commerce. This partnership helps turn Drupal’s already robust web platform into an even
more powerful—and fast—foundation for eCommerce success. The partnership ensures that merchants
can continue to operate and make money online without worrying if their site is ready for holiday traffic,
marketing campaigns and other performance and scalability challenges.') . '</p>',
  );

  $form['yottaa_site_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Yottaa Site ID'),
    '#default_value' => variable_get('yottaa_site_id', 'NONE'),
    '#description' => t('Your alphanumeric site ID.'),
    '#required' => TRUE,
  );

  $form['yottaa_user_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Yottaa User ID'),
    '#default_value' => variable_get('yottaa_user_id', 'NONE'),
    '#description' => t('Your alphanumeric user ID.'),
    '#required' => TRUE,
  );

  $form['yottaa_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Yottaa API key'),
    '#default_value' => variable_get('yottaa_api_key', 'NONE'),
    '#description' => t('All APIs request need to specify your API KEY, you must register for a Yottaa account, and within the user account API Key & Authorized Apps (@url) page you will see the "Your API Key" field, click the "Get new API Key" button to get your API key.', array('@url' => url('https://apps.yottaa.com/dashboard/settings/api_key'))),
    '#required' => TRUE,
  );

  return system_settings_form($form);
}

/**
 * Register form.
 */
function yottaa_register_form($form_state) {

  $form['introduction'] = array(
    '#type' => 'markup',
    '#markup' => '<p>' .
    t('Commerce Guys and Yottaa have partnered to bring fast, scalable and secure eCommerce acceleration
service to Drupal Commerce. This partnership helps turn Drupal’s already robust web platform into an even
more powerful—and fast—foundation for eCommerce success. The partnership ensures that merchants
can continue to operate and make money online without worrying if their site is ready for holiday traffic,
marketing campaigns and other performance and scalability challenges.') . '</p>',
  );

  $form['yottaa_first_name'] = array(
    '#type' => 'textfield',
    '#title' => t('First Name'),
    '#required' => TRUE,
  );

  $form['yottaa_last_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Last Name'),
    '#required' => TRUE,
  );

  $form['yottaa_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email Address'),
    '#description' => t('Enter the email address you wish to register with.'),
    '#required' => TRUE,
  );

  $form['yottaa_phone'] = array(
    '#type' => 'textfield',
    '#title' => t('Phone Number'),
    '#description' => t('Please use international format e.g. +44 1234 123456'),
    '#required' => TRUE,
  );

  $form['yottaa_domain'] = array(
    '#type' => 'textfield',
    '#title' => t('Domain'),
    '#description' => t('Enter the domain of the first site you would like to activate. e.g. example.com (do not include http://)'),
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Register'),
  );

  // Set form redirect.
  $form['#redirect'] = 'admin/config/services/yottaa';

  return $form;
}

/**
 * Impliments hook_form_validate().
 */
function yottaa_register_form_validate($form, &$form_state) {
  // Validation logic.
}

/**
 * Impliments hook_form_submit().
 */
function yottaa_register_form_submit($form, &$form_state) {

  $data = array(
    'email' => $form_state['values']['yottaa_email'],
    'site' => $form_state['values']['yottaa_domain'],
    'first_name' => $form_state['values']['yottaa_first_name'],
    'last_name' => $form_state['values']['yottaa_last_name'],
    'phone' => $form_state['values']['yottaa_phone'],
  );

  $headers = array(
    'Content-Type' => 'application/x-www-form-urlencoded',
  );

  $url = 'https://marketplace.commerceguys.com/api/yottaa/accounts/add';
  $options = array(
    'method' => 'POST',
    'data' => http_build_query($data),
    'headers' => $headers,
  );

  watchdog('yottaa', print_r($options, TRUE));

  $result = drupal_http_request($url, $options);

  watchdog('yottaa', print_r($result, TRUE));

  $data = drupal_json_decode($result->data);

  if (isset($data['site_id']) && $data['user_id']) {
    variable_set('yottaa_site_id', $data['site_id']);
    variable_set('yottaa_user_id', $data['user_id']);
    variable_set('yottaa_api_key', $data['api_key']);
    variable_set('yottaa_assessment_url', $data['assessment_url']);
    variable_set('yottaa_preview_url', $data['preview_url']);

    $message = '<p>' . t('Congratulations - Your sign up to Yottaa was successful!') . '</p>' .

    '<p>' . t('We’ve sent you an email with your password, You can log into your Yottaa account here: ') . l(t('https://apps.yottaa.com'), 'https://apps.yottaa.com') .
    '</p> <p>' . t('To get started you can:') .
    '</p><ol><li>' . t('Check out the Optimizer Preview: To illustrate what Yottaa Site Optimizer can do for yoursite, we have created a version of your site running on the Yottaa network and ran the Before/After sites through a battery of real browser tests.  You can check out the preview here: ') . l($data['preview_url'], $data['preview_url']) .
    '</li><li>' . t('Free Trial: The best way to check out Yottaa is to simply activate the free trial, through a straightforward CName or DNS change.  There are two ways to do this:') . '</li>' .
    '<ol><li>' . t('We are happy to walk you through the process (contact us via Live Chat once you log into Yottaa, or at info@yottaa.com)') .
    '</li><li>' . t('Or, give it a shot yourself, using the Activation Wizard inside the portal.') .
    '</li></ol><li>' . t('Schedule a Live Demo: If at any point you have questions, and/or would like to understand better how Site Optmizer works, we are happy to help – simply reach out via info@yottaa.com)') . '</li></ol>';

    drupal_set_message($message);
    drupal_goto('admin/config/services/yottaa/settings');
  }

  elseif (isset($data['error'])) {
    form_set_error('form', print_r($data['error'], TRUE));
    $form_state['redirect'] = FALSE;
  }

  else {
    form_set_error('form', t('There was an error contacting the registration service'));
    $form_state['redirect'] = FALSE;
  }

}
