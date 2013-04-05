<?php

/**
 * @file
 * Contains the administrative page and form callbacks for the Kiala module.
 */

// -----------------------------------------------------------------------
// Method settings form

/**
 * Shipping method settings form
 */
function commerce_kiala_settings_form($form, &$form_state) {
  $kiala_contact_mail = 'e-commerce@kiala.com';
  $countries = commerce_kiala_country_get_list();

  $encrypted = variable_get('commerce_kiala_encrypt', FALSE);
  $default_values = commerce_kiala_settings(array(
    'include_passwords' => FALSE,
    'decrypt' => TRUE,
  ));
  $sensitive_settings = commerce_kiala_settings(array(
    'include_passwords' => TRUE,
    'conditions' => array('is_sensitive' => TRUE),
  ));

  $embed_options = commerce_kiala_embed_options();
  $embed_description = commerce_kiala_embed_description();

  $cache_options = array(
    'none' => t('No cache'),
  );
  $cache_options += drupal_map_assoc(array(60, 3600, 10800, 21600, 43200, 86400, 2*86400, 3*86400, 4*86400, 5*86400, 604800), 'format_interval');


  $form['#attached']['css'][] = drupal_get_path('module', 'commerce_kiala') . '/theme/commerce_kiala.admin.css';
  $form['#attributes']['class'][] = 'commerce-kiala-settings-form';


  // -----------------------------------------------------------------------
  // Encryption settings

  if (!empty($sensitive_settings)) {
    $form['encryption'] = array(
      '#type' => 'item',
      '#title' => t('Encryption'),
      'status' => array(
        '#type' => 'item',
        '#title' => FALSE,
      ),
      '#weight' => -10,
    );

    if (commerce_kiala_encryption_available(array('display_warnings' => TRUE))) {
      $form['encryption']['status']['#markup'] = t('Encryption is available and configured properly.');
      $form['encryption']['commerce_kiala_encrypt'] = array(
        '#type' => 'checkbox',
        '#title' => t('Encrypt Kiala credentials (HIGHLY RECOMMENDED)'),
        '#description' => t('Note: Enabling this setting automatically will encrypt your password even though you cannot see it. Disabling this checkbox requires that you re-enter passwords.'),
        '#default_value' => $encrypted,
      );
    }
    else {
      $aes_link = l(t('AES'), 'http://drupal.org/project/aes', array('attributes' => array('target' => '_blank')));
      $form['encryption']['status']['#markup'] = '<span class="error">' . t('!aes is not installed - your login credentials will not be encrypted.', array('!aes' => $aes_link)) . '</span>';
    }
  }


  // -----------------------------------------------------------------------
  // Form building

  // initialize vertical tab groups
  $form['groups'] = array('#type' => 'vertical_tabs');


  // -----------------------------------------------------------------------
  // Common Settings

  $form['commerce_kiala_common'] = array(
    '#type' => 'fieldset',
    '#title' => t('Common'),
    '#collapsible' => TRUE,
    '#group' => 'groups',
    '#weight' => -5,
  );
  $form['commerce_kiala_common']['dspid'] = array(
    '#type' => 'textfield',
    '#title' => t('Merchant ID (DSP ID)'),
    '#default_value' => isset($default_values['dspid']) ? $default_values['dspid'] : '',
    '#description' => t('Contact Kiala to obtain a DSP ID for locate & search maps. For testing, use "DEMO_DSP".'),
    '#required' => TRUE,
  );
  $form['commerce_kiala_common']['shipping_countries_enabled'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Enabled shipping countries'),
    '#default_value' => isset($default_values['shipping_countries_enabled']) ? $default_values['shipping_countries_enabled'] : NULL,
    '#options' => $countries,
    '#required' => TRUE,
    '#description' => t('The shipping countries allowed per your Kiala account.'),
  );
  $form['commerce_kiala_common']['country'] = array(
    '#type' => 'select',
    '#title' => t('Sender Country'),
    '#default_value' => isset($default_values['country']) ? $default_values['country'] : NULL,
    '#options' => $countries,
    '#required' => TRUE,
    '#description' => t('The country from which the parcels will be shipped.'),
  );
  $form['commerce_kiala_common']['preparationdelay'] = array(
    '#type' => 'textfield',
    '#title' => t('Average Preparation Delay'),
    '#field_suffix' => t('days'),
    '#default_value' => isset($default_values['preparationdelay']) ? $default_values['preparationdelay'] : '',
    '#description' => t('The days between selecting a Kiala Point for this order and the arrival of the parcel in the Kiala
                        sorting center. When this parameter is missing, the default value for your Kiala account will be used.'),
    '#element_validate' => array('element_validate_integer_positive'),
    '#size' => 10,
  );


  // -----------------------------------------------------------------------
  // Search settings

  $form['commerce_kiala_ls_search'] = array(
    '#type' => 'fieldset',
    '#title' => t('Kiala Point Search'),
    '#collapsible' => TRUE,
    '#group' => 'groups',
    '#weight' => 0,
  );

  $form['commerce_kiala_ls_search']['commerce_kiala_ls_ws_search'] = array(
    '#type' => 'fieldset',
    '#title' => t('Web Service'),
    '#collapsible' => TRUE,
    '#weight' => 0,
    '#description' => t('Settings for Kiala Locate & Select search web service. Kiala developer guide can be found at <a href=@url>@url</a>.',
                          array('@url' => 'http://locateandselect.kiala.com/info/ws')
                        )
  );
  $form_ls_ws_search = &$form['commerce_kiala_ls_search']['commerce_kiala_ls_ws_search'];

  $form_ls_ws_search['ls_ws_search_url'] = array(
    '#type' => 'textfield',
    '#title' => t('API URL'),
    '#default_value' => isset($default_values['ls_ws_search_url']) ? $default_values['ls_ws_search_url'] : '',
    '#disabled' => TRUE,
  );

  if (!empty($default_values['ls_ws_search_params'])) {
    $form_ls_ws_search['ls_ws_search_params'] = commerce_kiala_admin_settings_params_element($default_values['ls_ws_search_params'], 'http://locateandselect.kiala.com/info/ws');
  }


  $form['commerce_kiala_ls_search']['commerce_kiala_ls_map_search'] = array(
    '#type' => 'fieldset',
    '#title' => t('Map'),
    '#collapsible' => TRUE,
    '#description' => t('Settings for Kiala Locate & Select Search map API. Kiala developer guide can be found at <a href=@url>@url</a>.',
                          array('@url' => 'http://locateandselect.kiala.com/info/search')
                        )
  );
  $form_ls_map_search = &$form['commerce_kiala_ls_search']['commerce_kiala_ls_map_search'];

  $form_ls_map_search['ls_map_search_url'] = array(
    '#type' => 'textfield',
    '#title' => t('API URL'),
    '#default_value' => isset($default_values['ls_map_search_url']) ? $default_values['ls_map_search_url'] : '',
    '#disabled' => TRUE,
  );

  $form_ls_map_search['ls_map_search_embed_type'] = array(
    '#type' => 'select',
    '#title' => t('Embedding Type'),
    '#default_value' => isset($default_values['ls_map_search_embed_type']) ? $default_values['ls_map_search_embed_type'] : NULL,
    '#description' => t('Choose the method to embed the map.<br />Available Options: !embed_description', array('!embed_description' => $embed_description)),
    '#options' => $embed_options,
  );
  $form_ls_map_search['ls_map_search_modal_link_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Modal Link Text'),
    '#default_value' => isset($default_values['ls_map_search_modal_link_text']) ? $default_values['ls_map_search_modal_link_text'] : '',
    '#description' => t('The text to display for the modal link.'),
    '#states' => array(
      'invisible' => array(
        ':input[name*="ls_map_search_embed_type"]' => array('value' => 'inline'),
      ),
    ),
  );
  $form_ls_map_search['ls_map_search_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#default_value' => isset($default_values['ls_map_search_width']) ? $default_values['ls_map_search_width'] : '',
    '#element_validate' => array('element_validate_integer_positive'),
    '#description' => t('The width of the embedded map.'),
    '#size' => 10,
  );
  $form_ls_map_search['ls_map_search_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Height'),
    '#default_value' => isset($default_values['ls_map_search_height']) ? $default_values['ls_map_search_height'] : '',
    '#element_validate' => array('element_validate_integer_positive'),
    '#description' => t('The height of the embedded map.'),
    '#size' => 10,
  );

  if (!empty($default_values['ls_map_search_params'])) {
    $form_ls_map_search['ls_map_search_params'] = commerce_kiala_admin_settings_params_element($default_values['ls_map_search_params'], 'http://locateandselect.kiala.com/info/search');
  }


  // -----------------------------------------------------------------------
  // Point Details settings

  $form['commerce_kiala_ls_details'] = array(
    '#type' => 'fieldset',
    '#title' => t('Kiala Point Details'),
    '#collapsible' => TRUE,
    '#group' => 'groups',
  );

  $form['commerce_kiala_ls_details']['commerce_kiala_ls_ws_details'] = array(
    '#type' => 'fieldset',
    '#title' => t('Web Service'),
    '#collapsible' => TRUE,
    '#description' => t('Settings for Kiala Locate & Select details web service. Kiala developer guide can be found at <a href=@url>@url</a>.',
                          array('@url' => 'http://locateandselect.kiala.com/info/ws')
                        )
  );
  $form_ls_ws_details = &$form['commerce_kiala_ls_details']['commerce_kiala_ls_ws_details'];

  $form_ls_ws_details['ls_ws_details_url'] = array(
    '#type' => 'textfield',
    '#title' => t('API URL'),
    '#default_value' => isset($default_values['ls_ws_details_url']) ? $default_values['ls_ws_details_url'] : '',
    '#disabled' => TRUE,
  );

  $form_ls_ws_details['ls_ws_details_cache_lifetime'] = array(
    '#type' => 'select',
    '#title' => t('Response cache'),
    '#default_value' => isset($default_values['ls_ws_details_cache_lifetime']) ? $default_values['ls_ws_details_cache_lifetime'] : 'none',
    '#options' => $cache_options,
    '#description' => t('Cached responses will not be re-requested until at least this much time has elapsed.
                        This will help reduce the number of requests made to the Kiala web service.
                        If this is set to "No cache", then every view of a Kiala point\'s details will cause a request to the Kiala web service.'),
  );
  $form_ls_ws_details['ls_ws_details_image_style'] = array(
    '#type' => 'select',
    '#title' => t('Point Picture Image Style'),
    '#default_value' => isset($default_values['ls_ws_details_image_style']) ? $default_values['ls_ws_details_image_style'] : '',
    '#description' => t('Select an image style to be use for the Kiala Point picture during checkout.'),
    '#options' => image_style_options(),
  );

  if (!empty($default_values['ls_ws_details_params'])) {
    $form_ls_ws_details['ls_ws_details_params'] = commerce_kiala_admin_settings_params_element($default_values['ls_ws_details_params'], 'http://locateandselect.kiala.com/info/ws');
  }


  $form['commerce_kiala_ls_details']['commerce_kiala_ls_map_details'] = array(
    '#type' => 'fieldset',
    '#title' => t('Map'),
    '#collapsible' => TRUE,
    '#description' => t('Settings for Kiala Locate & Select details map API. Kiala developer guide can be found at <a href=@url>@url</a>.',
                          array('@url' => 'http://locateandselect.kiala.com/info/details')
                        ),
  );
  $form_ls_map_details = &$form['commerce_kiala_ls_details']['commerce_kiala_ls_map_details'];

  $form_ls_map_details['ls_map_details_url'] = array(
    '#type' => 'textfield',
    '#title' => t('API URL'),
    '#default_value' => isset($default_values['ls_map_details_url']) ? $default_values['ls_map_details_url'] : '',
    '#disabled' => TRUE,
  );
  $form_ls_map_details['ls_map_details_embed_type'] = array(
    '#type' => 'select',
    '#title' => t('Embedding Type'),
    '#default_value' => isset($default_values['ls_map_details_embed_type']) ? $default_values['ls_map_details_embed_type'] : NULL,
    '#description' => t('Choose the method to embed the map.<br />Available Options: !embed_description', array('!embed_description' => $embed_description)),
    '#options' => $embed_options,
  );
  $form_ls_map_details['ls_map_details_modal_link_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Modal Link Text'),
    '#default_value' => isset($default_values['ls_map_details_modal_link_text']) ? $default_values['ls_map_details_modal_link_text'] : '',
    '#description' => t('The text to display for the modal link.'),
    '#states' => array(
      'invisible' => array(
        ':input[name*="ls_map_details_embed_type"]' => array('value' => 'inline'),
      ),
    ),
  );
  $form_ls_map_details['ls_map_details_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#default_value' => isset($default_values['ls_map_details_width']) ? $default_values['ls_map_details_width'] : '',
    '#element_validate' => array('element_validate_integer_positive'),
    '#description' => t('The width of the embedded map.'),
    '#size' => 10,
  );
  $form_ls_map_details['ls_map_details_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Height'),
    '#default_value' => isset($default_values['ls_map_details_height']) ? $default_values['ls_map_details_height'] : '',
    '#element_validate' => array('element_validate_integer_positive'),
    '#description' => t('The height of the embedded map.'),
    '#size' => 10,
  );

  if (!empty($default_values['ls_map_details_params'])) {
    $form_ls_map_details['ls_map_details_params'] = commerce_kiala_admin_settings_params_element($default_values['ls_map_details_params'], 'http://locateandselect.kiala.com/info/details');
  }


  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Validate callback for commerce_kiala_settings_form()
 */
function commerce_kiala_settings_form_validate($form, &$form_state) {
  $values = &$form_state['values'];
  $current_encrypted = variable_get('commerce_kiala_encrypt', FALSE) && _commerce_kiala_decrypt_handler();
  $new_encrypted = isset($values['commerce_kiala_encrypt']) && $values['commerce_kiala_encrypt'];

  // get the stored passwords untouched from the database
  $current_password_hashes = commerce_kiala_settings(array(
    'include_passwords' => TRUE,
    'decrypt' => FALSE,
    'conditions' => array('password' => TRUE),
  ));

  // Check all passwords if encryption turned off
  if (!$new_encrypted && $current_encrypted) {
    foreach ($current_password_hashes as $password_k => $current_password_value) {
      if (empty($values[$password_k])) {
        form_set_error($password_k, t('To turn off encryption, you must re-enter all passwords.'));
      }
    }
  }
}

/**
 * Submit callback for commerce_kiala_settings_form()
 */
function commerce_kiala_settings_form_submit($form, &$form_state) {
  form_state_values_clean($form_state);
  $values = &$form_state['values'];

  $current_encrypted = variable_get('commerce_kiala_encrypt', FALSE) && _commerce_kiala_decrypt_handler();
  $new_encrypted = isset($values['commerce_kiala_encrypt']) && $values['commerce_kiala_encrypt'];

  // get all current stored settings
  $current_settings = commerce_kiala_settings(array(
    'include_passwords' => TRUE,
    'decrypt' => FALSE,
  ));

  // get the stored decrypted passwords
  $current_passwords_plain = commerce_kiala_settings(array(
    'include_passwords' => TRUE,
    'decrypt' => TRUE,
    'conditions' => array('password' => TRUE),
  ));

  // Encrypt the credentials if available
  if ($new_encrypted) {
    // Process passwords since they do not need to be entered
    // If encryption is turned on but the password is already in the db.
    // Then we need to pull it from the db, not from the form.
    if (!$current_encrypted) {
      foreach ($current_passwords_plain as $password_k => $password_current_value) {
        if (empty($values[$password_k])) {
          $values[$password_k] = $password_current_value;
        }
      }
    }

    // encrypt sensitive data
    $values = commerce_kiala_encrypt_settings_values($values);
  }

  // Remove all empty passwords to not update current value
  foreach ($current_passwords_plain as $password_k => $password_current_value) {
    if (empty($values[$password_k])) {
      unset($values[$password_k]);
    }
  }


  // Save encryption status
  if (!isset($values['commerce_kiala_encrypt'])) {
    variable_set('commerce_kiala_encrypt', FALSE);
  }
  else {
    variable_set('commerce_kiala_encrypt', $values['commerce_kiala_encrypt']);
  }
  unset($values['commerce_kiala_encrypt']);


  // Save settings values
  // remove any values not in settings info
  $settings = array_intersect_key($values, $current_settings);

  // merge stored settings
  $settings += $current_settings;

  // save new merged settings
  variable_set('commerce_kiala_settings', $settings);


  drupal_set_message(t('The configuration options have been saved.'));
}


// -----------------------------------------------------------------------
// Rate Service Form

/**
 * Builds the form for adding and editing services.
 */
function commerce_kiala_service_form($form, &$form_state, $shipping_service) {
  $form['#attached']['css'][] = drupal_get_path('module', 'commerce_kiala') . '/theme/commerce_kiala.admin.css';
  $form['#attributes']['class'][] = 'commerce-kiala-service-form';

  // Store the initial shipping service in the form state.
  $form_state['shipping_service'] = $shipping_service;

  $form['kiala'] = array(
    '#tree' => TRUE,
  );

  $form['kiala']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => $shipping_service['title'],
    '#description' => t('The administrative title of this Kiala rate. It is recommended that this title begin with a capital letter and contain only letters, numbers, and spaces.'),
    '#required' => TRUE,
    '#size' => 32,
    '#maxlength' => 255,
    '#field_suffix' => ' <small id="edit-kiala-title-suffix">' . t('Machine name: @name', array('@name' => $shipping_service['name'])) . '</small>',
  );

  if (empty($shipping_service['name'])) {
    $form['kiala']['name'] = array(
      '#type' => 'machine_name',
      '#title' => t('Machine name'),
      '#default_value' => $shipping_service['name'],
      '#maxlength' => 32,
      '#required' => TRUE,
      '#machine_name' => array(
        'exists' => 'commerce_shipping_service_load',
        'source' => array('kiala', 'title'),
      ),
      '#description' => t('The machine-name of this Kiala rate. This name must contain only lowercase letters, numbers, and underscores. It must be unique.'),
    );
  }
  else {
    $form['kiala']['name'] = array(
      '#type' => 'value',
      '#value' => $shipping_service['name'],
    );
  }

  $form['kiala']['display_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Display title'),
    '#default_value' => $shipping_service['display_title'],
    '#description' => t('The front end display title of this Kiala rate shown to customers. Leave blank to default to the <em>Title</em> from above.'),
    '#size' => 32,
  );

  $form['kiala']['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('Describe this Kiala rate if necessary. The text will be displayed in the Kiala rate services overview table.'),
    '#default_value' => $shipping_service['description'],
    '#rows' => 3,
  );

  $form['kiala']['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Base rate'),
    '#default_value' => commerce_currency_amount_to_decimal($shipping_service['base_rate']['amount'], $shipping_service['base_rate']['currency_code']),
    '#required' => TRUE,
    '#size' => 10,
    '#prefix' => '<div class="commerce-kiala-base-rate">',
    '#suffix' => '</div">',
  );

  // Build a currency options list from all enabled currencies.
  $options = commerce_currency_get_code(TRUE);

  // If the current currency value is not available, add it now with a
  // message in the help text explaining it.
  if (empty($options[$shipping_service['base_rate']['currency_code']])) {
    $options[$shipping_service['base_rate']['currency_code']] = check_plain($shipping_service['base_rate']['currency_code']);

    $description = t('The currency set for this rate is not currently enabled. If you change it now, you will not be able to set it back.');
  }
  else {
    $description = '';
  }

  // If only one currency option is available, don't use a select list.
  if (count($options) == 1) {
    $currency_code = key($options);

    $form['kiala']['amount']['#field_suffix'] = check_plain($currency_code);
    $form['kiala']['amount']['#suffix'] = '</div>';

    $form['kiala']['currency_code'] = array(
      '#type' => 'value',
      '#default_value' => $currency_code,
    );
  }
  else {
    $form['kiala']['currency_code'] = array(
      '#type' => 'select',
      '#description' => $description,
      '#options' => $options,
      '#default_value' => $shipping_service['base_rate']['currency_code'],
      '#suffix' => '</div>',
    );
  }

  // Add support for base rates including tax.
  if (module_exists('commerce_tax')) {
    // Build an array of tax types that are display inclusive.
    $inclusive_types = array();

    foreach (commerce_tax_types() as $name => $tax_type) {
      if ($tax_type['display_inclusive']) {
        $inclusive_types[$name] = $tax_type['title'];
      }
    }

    // Build an options array of tax rates of these types.
    $options = array();

    foreach (commerce_tax_rates() as $name => $tax_rate) {
      if (in_array($tax_rate['type'], array_keys($inclusive_types))) {
        $options[$inclusive_types[$tax_rate['type']]][$name] = t('Including @title', array('@title' => $tax_rate['title']));
      }
    }

    if (!empty($options)) {
      // Find the default value for the tax included element.
      $default = '';

      if (!empty($shipping_service['data']['include_tax'])) {
        $default = $shipping_service['data']['include_tax'];
      }

      $form['kiala']['currency_code']['#title'] = '&nbsp;';

      $form['kiala']['include_tax'] = array(
        '#type' => 'select',
        '#title' => t('Include tax in this rate'),
        '#description' => t('Saving a rate tax inclusive will bypass later calculations for the specified tax.'),
        '#options' => count($options) == 1 ? reset($options) : $options,
        '#default_value' => $default,
        '#required' => FALSE,
        '#empty_value' => '',
      );
    }
  }

  $form['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight' => 40,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save service'),
  );

  if (!empty($form_state['shipping_service']['name'])) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#suffix' => l(t('Cancel'), 'admin/commerce/config/shipping/services/kiala'),
      '#submit' => array('commerce_kiala_service_form_delete_submit'),
      '#weight' => 45,
    );
  }
  else {
    $form['actions']['submit']['#suffix'] = l(t('Cancel'), 'admin/commerce/config/shipping/services/kiala');
  }

  return $form;
}

/**
 * Validate handler: ensures a valid base rate was entered for the service.
 */
function commerce_kiala_service_form_validate($form, &$form_state) {
  // Ensure the rate amount is numeric.
  if (!is_numeric($form_state['values']['kiala']['amount'])) {
    form_set_error('kiala][amount', t('You must enter a numeric value for the base rate amount.'));
  }
  else {
    // Convert the decimal amount value entered to an integer based amount value.
    form_set_value($form['kiala']['amount'], commerce_currency_decimal_to_amount($form_state['values']['kiala']['amount'], $form_state['values']['kiala']['currency_code']), $form_state);
  }
}

/**
 * Submit handler: saves the new or updated service.
 */
function commerce_kiala_service_form_submit($form, &$form_state) {
  $shipping_service = $form_state['shipping_service'];

  // Update the shipping service array with values from the form.
  foreach (array('name', 'title', 'display_title', 'description', 'amount', 'currency_code') as $key) {
    $shipping_service[$key] = $form_state['values']['kiala'][$key];
  }

  // If a tax was specified for inclusion, add it to the data array.
  if (!empty($form_state['values']['kiala']['include_tax'])) {
    $shipping_service['data']['include_tax'] = $form_state['values']['kiala']['include_tax'];
  }
  elseif (!empty($shipping_service['data']['include_tax'])) {
    unset($shipping_service['data']['include_tax']);
  }

  // Save the shipping service.
  unset($shipping_service['base_rate']);
  $op = commerce_kiala_service_save($shipping_service);

  if (!$op) {
    drupal_set_message(t('The Kiala rate service failed to save properly. Please review the form and try again.'), 'error');
    $form_state['rebuild'] = TRUE;
  }
  else {
    drupal_set_message(t('Kiala rate service saved.'));
    $form_state['redirect'] = 'admin/commerce/config/shipping/services/kiala';
  }
}

/**
 * Submit handler: redirects to the ervice delete confirmation form.
 *
 * @see commerce_kiala_service_form()
 */
function commerce_kiala_service_form_delete_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/commerce/config/shipping/services/kiala-' . strtr($form_state['shipping_service']['name'], '_', '-') . '/delete';
}

/**
 * Displays the edit form for an existing service.
 *
 * @param $name
 *   The machine-name of the service to edit.
 */
function commerce_kiala_service_edit_page($name) {
  return drupal_get_form('commerce_kiala_service_form', commerce_shipping_service_load($name));
}

/**
 * Builds the form for deleting services.
 */
function commerce_kiala_service_delete_form($form, &$form_state, $shipping_service) {
  $form_state['shipping_service'] = $shipping_service;

  $form = confirm_form($form,
    t('Are you sure you want to delete the <em>%title</em> Kiala rate service?', array('%title' => $shipping_service['title'])),
    'admin/commerce/config/shipping/services/kiala',
    '<p>' . t('This action cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for commerce_kiala_service_delete_form().
 */
function commerce_kiala_service_delete_form_submit($form, &$form_state) {
  $shipping_service = $form_state['shipping_service'];

  commerce_kiala_service_delete($shipping_service['name']);

  drupal_set_message(t('The Kiala rate service <em>%title</em> has been deleted.', array('%title' => $shipping_service['title'])));
  watchdog('commerce_kiala', 'Deleted Kiala rate service <em>%title</em>.', array('%title' => $shipping_service['title']), WATCHDOG_NOTICE);

  $form_state['redirect'] = 'admin/commerce/config/shipping/services/kiala';
}

/**
 * Displays the delete confirmation form for an existing service.
 *
 * @param $name
 *   The machine-name of the service to delete.
 */
function commerce_kiala_service_delete_page($name) {
  return drupal_get_form('commerce_kiala_service_delete_form', commerce_shipping_service_load($name));
}
