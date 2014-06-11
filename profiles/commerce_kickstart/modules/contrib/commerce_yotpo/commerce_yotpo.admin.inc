<?php
/**
 * @file
 * commerce_yotpo.admin.inc
 *   Administrtive forms for Commerce Yotpo module
 */

/**
 * Menu callback. Redirect the user to the right page.
 */
function commerce_yotpo_select_page() {
  if (variable_get('commerce_yotpo_id', FALSE) === FALSE) {
    drupal_goto('admin/commerce/config/yotpo/register');
  }
  else {
    drupal_goto('admin/commerce/config/yotpo/settings');
  }
}

/**
 * Settings form.
 */
function commerce_yotpo_settings_form($form_state) {
  $form['commerce_yotpo_id'] = array(
    '#type' => 'textfield',
    '#title' => t('API Key'),
    '#default_value' => variable_get('commerce_yotpo_id', ''),
    '#description' => t('The Yotpo API Key'),
    '#required' => TRUE,
  );

  $form['commerce_yotpo_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('API Secret'),
    '#default_value' => variable_get('commerce_yotpo_secret', ''),
    '#description' => t('The Yotpo API Secret'),
    '#required' => TRUE,
  );

  $node_fields = field_info_instances('node');
  $product_field_options = array();
  foreach ($node_fields as $field_instances) {
    foreach ($field_instances as $field) {
      $field_info = field_info_field($field['field_name']);
      if ($field_info['type'] == 'commerce_product_reference' || $field_info['type'] == 'entityreference') {
        $product_field_options[$field['field_name']] = $field['label'] . " (" . $field['field_name'] . ")";
      }
    }
  }

  $form['commerce_yotpo_product_field'] = array(
    '#type' => 'select',
    '#title' => t('Product field'),
    '#description' => t('Select the field that will be used as the product on nodes for Yotpo.'),
    '#default_value' => variable_get('commerce_yotpo_product_field', 'field_product'),
    '#options' => $product_field_options,
  );

  $product_fields = field_info_instances('commerce_product');
  $image_field_options = array();
  $image_field_types = array('image', 'text', 'file');

  foreach ($product_fields as $field_instances) {
    foreach ($field_instances as $field) {
      $field_info = field_info_field($field['field_name']);
      if (in_array($field_info['type'], $image_field_types)) {
        $image_field_options[$field['field_name']] = $field['label'] . " (" . $field['field_name'] . ")";
      }
    }
  }
  if (empty($image_field_options)) {
    drupal_set_message(t('Your product needs an image field'), 'warning');
  }
  $form['commerce_yotpo_image_field'] = array(
    '#type' => 'select',
    '#title' => t('Image field'),
    '#description' => t('Select the field that will be used as the image for Yotpo.'),
    '#default_value' => variable_get('commerce_yotpo_image_field', 'field_images'),
    '#options' => $image_field_options,
  );

  $form = system_settings_form($form);

  if (variable_get('commerce_yotpo_utoken', FALSE)) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'commerce_order')
    ->propertyCondition('created', strtotime('-90 days'), '>=')
    ->propertyCondition('state', 'completed')
    ->fieldCondition('commerce_customer_billing', 'profile_id', 'NULL', 'IS NOT NULL')
    ->count();

    $result = $query->execute();

    if (count($result > 0)) {
      $form['actions']['sync_orders'] = array(
        '#type' => 'submit',
        '#submit' => array('commerce_yotpo_settings_form_send_orders'),
        '#value' => t('Send historic orders'),
      );
    }
  }

  $form['#submit'][] = 'commerce_yotpo_settings_form_token';

  return $form;
}

/**
 * Submit callback.
 */
function commerce_yotpo_settings_form_token($form, &$form_state) {
  if ($yotpo = commerce_yotpo_new()) {
    // Retrieving the utoken - will be valid for 24 hours.
    $credentials = $yotpo->get_oauth_token();
    if (isset($credentials->access_token) && $utoken = $credentials->access_token) {
      variable_set('commerce_yotpo_utoken', $utoken);
      drupal_set_message(t('Yotpo API Connected.'));
    }
  }
  else {
    variable_set('commerce_yotpo_utoken', FALSE);
    form_set_error('commerce_yotpo', t('There was an error getting the token. Please check your API details.'));
  }
}

/**
 * Send Orders callback.
 */
function commerce_yotpo_settings_form_send_orders($form, &$form_state) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'commerce_order')
    ->propertyCondition('created', strtotime('-90 days'), '>=')
    ->propertyCondition('state', 'completed')
    ->fieldCondition('commerce_customer_billing', 'profile_id', 'NULL', 'IS NOT NULL');

  $result = $query->execute();

  $order_ids = array_keys($result['commerce_order']);
  $commerce_orders = commerce_order_load_multiple($order_ids);
  $response = commerce_yotpo_bulk_order_post($commerce_orders);

  if (isset($response->code) && $response->code == 200) {
    drupal_set_message(t('Orders posted to Yotpo.'));
  }
  else {
    drupal_set_message(t('There was an error posting orders to Yotpo.'), 'error');
  }
}

/**
 * Register form.
 */
function commerce_yotpo_register_form($form_state) {

  $library = libraries_detect('yotpo-php');
  // Check if the expected library path exists.
  if (!empty($library['installed'])) {

    $form['yotpo_name'] = array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#description' => t('Your Name'),
      '#required' => TRUE,
    );

    $form['yotpo_email'] = array(
      '#type' => 'textfield',
      '#title' => t('Email Address'),
      '#description' => t('Enter the email address you wish to register with.'),
      '#required' => TRUE,
    );

    $form['yotpo_domain'] = array(
      '#type' => 'textfield',
      '#title' => t('Domain'),
      '#description' => t('Enter the main domain of the site. e.g. http://www.example.com (do include http://)'),
      '#required' => TRUE,
    );

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Register'),
    );

    $form['yotpo_markup'] = array(
      '#markup' => t('Already registered? <a href="@already_registered">Click here to enter your account information and get started!</a>',
        array('@already_registered' => url('admin/commerce/config/yotpo/settings'))));

    $form['introduction'] = array(
      '#type' => 'markup',
      '#markup' => '<p>' .
      t('By clicking register you agree to the Yotpo Terms & Conditions.') . '</p>',
      '#weight' => 10,
    );

    // Set form redirect.
    $form['#redirect'] = 'admin/commerce/config/yotpo';

    // If we already have an account.
    if (variable_get('commerce_yotpo_id', FALSE) !== FALSE) {
      $form['submit']['#disabled'] = TRUE;
      drupal_set_message(t('Your account is already configured. You can check your account settings <a href="@already_registered">on the settings page</a>.',
        array('@already_registered' => url('admin/commerce/config/yotpo/settings'))));
    }
  }

  // If no library.
  else {
    drupal_set_message(t('Yotpo PHP library is required.'), 'error');

    $form['help'] = array(
      '#type' => 'markup',
      '#markup' => '<p>' .
      t('
          See the <a href="@help_url">project page</a> for install instructions.
          <br/>
          Download the required libraries from
          <a href="@library_url">the Yotpo GitHub project</a>
          and place it in <code>@install_path</code>',
          array(
            '@library_url' => $library['download url'],
            '@help_url' => url('https://drupal.org/project/commerce_yotpo'),
            '@install_path' => 'sites/all/libraries/yotpo-php',
          )) . '</p>',
      '#weight' => 10,
    );
  }

  return $form;
}

/**
 * Implements hook_form_submit().
 */
function commerce_yotpo_register_form_submit($form, &$form_state) {

  if ($yotpo = commerce_yotpo_new()) {

    $data = array();
    $data['email'] = $form_state['values']['yotpo_email'];
    $data['display_name'] = $form_state['values']['yotpo_name'];
    $data['first_name'] = '';
    $data['last_name'] = '';
    $data['website_name'] = '';
    $data['password'] = '';
    $data['support_url'] = '';
    $data['callback_url'] = '';
    $data['url'] = $form_state['values']['yotpo_domain'];
    $data['install_step'] = 'done';

    $response = $yotpo->create_user($data);

  }
  else {
    watchdog('commerce_yotpo', 'Could not load Yotpo library');
  }

  if (isset($response->status->code) && $response->status->code == 200) {
    variable_set('commerce_yotpo_id', $response->response->app_key);
    variable_set('commerce_yotpo_secret', $response->response->secret);
    variable_set('commerce_yotpo_utoken', $response->response->token);

    if ($yotpo = commerce_yotpo_new()) {
      $credentials = $yotpo->get_oauth_token();
      $utoken = $credentials->access_token;

      $data = array();
      $data['utoken'] = $utoken;
      $data['platform_type_id'] = 20;
      $data['shop_domain'] = $form_state['values']['yotpo_domain'];

      $platform_responce = $yotpo->create_account_platform($data);

    }

    $message = t('Congratulations - Your sign up to Yotpo was successful!');
    drupal_set_message($message);
    drupal_goto('admin/commerce/config/yotpo/settings');
  }

  else {
    form_set_error('form', t('There was an error contacting the registration service'));
    $form_state['redirect'] = FALSE;
  }

}
