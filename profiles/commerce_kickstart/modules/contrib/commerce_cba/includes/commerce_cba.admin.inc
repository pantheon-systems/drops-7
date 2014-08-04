<?php
/**
 * @file
 * Admin UI for Commerce cba
 */

function commerce_cba_settings_form($form, &$form_state) {
  // Checkout by Amazon available only for EUR and GBP currencies.
  // Check enabled currencies.
  $enabled_currencies = commerce_currencies(TRUE);
  $amazon_currencies = array (
    'EUR' => TRUE,
    'GBP' => TRUE,
  );
  // Notification to enable the corresponding currency for the Merchant account.
  if (!array_intersect_key($amazon_currencies, $enabled_currencies)) {
    drupal_set_message(t('You should have EUR or GBP currency enabled to use Checkout by Amazon payment.'));
    $content = t('Please enable your Amazon Merchant account currency : !currency_page. ', array('!currency_page' => l(t('Currency settings page'), 'admin/commerce/config/currency')));
    $help_items[] = t('EUR for the German account.');
    $help_items[] = t('GBP for the UK account.');
    $content .= theme('item_list', array('items' => $help_items));
    $form['content']['#markup'] = $content;
    return $form;
  }
  // Checkout by Amazon settings form.
  $form['cba_merchant_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Merchant ID'),
    '#default_value' => variable_get('cba_merchant_id', ''),
    '#required' => TRUE,
  );

  $form['cba_public_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Amazon public key'),
    '#description' => t('Amazon public key could be referred to as \'AWS Access Key ID\''),
    '#default_value' => variable_get('cba_public_key', ''),
    '#required' => TRUE,
  );

  $form['cba_secret_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Amazon secret key'),
    '#default_value' => variable_get('cba_secret_key', ''),
    '#required' => TRUE,
  );

  $form['cba_method'] = array(
    '#type' => 'select',
    '#title' => t('Method for the requests'),
    '#options' => drupal_map_assoc(array('GET', 'POST')),
    '#default_value' => variable_get('cba_method', 'POST'),
  );

  $form['cba_country'] = array(
    '#type' => 'select',
    '#title' => t('Country'),
    '#options' => array('gb' => t('United Kingdom'), 'de' => t('Germany'), 'us' => t('United States')),
    '#default_value' => variable_get('cba_country', 'gb'),
  );

  $form['cba_mode'] = array(
    '#type' => 'select',
    '#title' => t('Mode'),
    '#options' => array('sandbox' => t('Sandbox'), 'live' => t('Live')),
    '#default_value' => variable_get('cba_mode', 'sandbox'),
  );

  $form['cba_checkout_type'] = array(
    '#type' => 'select',
    '#title' => t('Checkout type'),
    '#options' => array('inline' => t('Inline'), 'express' => t('Express')),
    '#default_value' => variable_get('cba_checkout_type', 'inline'),
  );

  // Amazon widgets settings.
  $form['cba_iopn_help'] = array(
    '#type' => 'fieldset',
    '#tree' => 1,
    '#title' => t('Amazon IOPN Help'),
    '#description' => t('How to set Amazon Instant Order Processing Notification Service.'),
  );

  // Built the Amazon IOPN help text.
  $cba_country = variable_get('cba_country', 'gb');
  switch ($cba_country) {
    case 'gb':
      $tld = 'co.uk';
      break;
    case 'de':
      $tld = 'de';
      break;
    case 'us':
      $tld = 'com';
      break;
  }
  $help = '<strong>' . t('A valid SSL certificate is required to use the IOPN Service.') . '</strong>';
  $help .= '<br />';
  $help .= t('You have to configure the Merchant URL for Amazon IOPN Service in Amazon Seller Central under Settings > Checkout Pipeline Settings, Instant Order Processing Notification Settings.');
  $help .= '<br />';
  $help .= l(t('Link to Amazon Seller Central - Checkout Pipeline Settings page'), 'https://sellercentral.amazon.' . $tld . '/gp/cba/seller/account/settings/user-settings-edit.html', array('attributes' => array('target' => '_blank')));
  $help .= '<br />';
  $merchant_url = url('commerce-cba-iopn', array('absolute' => TRUE));
  $help .= t('!url is the current store URL that should be use there.', array('!url' => '<strong>' . $merchant_url . '</strong>'));

  $form['cba_iopn_help']['help'] = array(
    '#markup' => $help,
  );

  // Amazon widgets settings.
  $form['cba_widget_settings'] = array(
    '#type' => 'fieldset',
    '#tree' => 1,
    '#title' => t('Amazon widget settings'),
  );

  $default_cba_widget_settings = array(
    'cba_button_size' => 'large',
    'cba_button_color' => 'orange',
    'cba_button_background' => 'white',
    'cba_widget_width' => NULL,
    'cba_widget_height' => NULL,
  );
  $cba_widget_settings = variable_get('cba_widget_settings', $default_cba_widget_settings);

  $form['cba_widget_settings']['cba_button_size'] = array(
    '#type' => 'select',
    '#title' => t('Size of the button'),
    '#options' => array('medium' => t('Medium (126x24)'), 'large' => t('Large (151x27)'), 'x-large' => t('Extra large (173x27)')),
    '#default_value' => $cba_widget_settings['cba_button_size'],
  );
  $form['cba_widget_settings']['cba_button_color'] = array(
    '#type' => 'select',
    '#title' => t('Color of the button'),
    '#options' => array('orange' => t('Orange'), 'tan' => t('Tan')),
    '#default_value' => $cba_widget_settings['cba_button_color'],
  );
  $form['cba_widget_settings']['cba_button_background'] = array(
    '#type' => 'select',
    '#title' => t('Background of the button'),
    '#options' => array('white' => t('White'), 'light' => t('Light'), 'dark' => t('Dark')),
    '#default_value' => $cba_widget_settings['cba_button_background'],
  );

  $form['cba_widget_settings']['cba_widget_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#default_value' => $cba_widget_settings['cba_widget_width'],
    '#description' => 'Width in pixels for Amazon data widgets (address, wallet, order details) - between 150px and 600px.',
    '#size' => 10,
    '#element_validate' => array('element_validate_number'),
  );

  $form['cba_widget_settings']['cba_widget_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Height'),
    '#default_value' => $cba_widget_settings['cba_widget_height'],
    '#description' => 'Height in pixels for Amazon data widgets (address, wallet, order details) - between 180px and 400px.',
    '#size' => 10,
    '#element_validate' => array('element_validate_number'),
  );

  // Option to hide the regular checkout cart button.
  $form['cba_hide_regular_checkout'] = array(
    '#type' => 'select',
    '#title' => t('Hide the regular checkout cart button.'),
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#default_value' => variable_get('cba_hide_regular_checkout', 0),
  );

  $form['debug'] = array(
    '#type' => 'fieldset',
    '#title' => t('Debugging'),
    '#collapsible' => TRUE,
    '#collapsed' => !(variable_get('cba_debug_log', 0) || variable_get('cba_debug', 0)),
  );
  $form['debug']['cba_debug_log'] = array(
    '#type' => 'checkbox',
    '#title' => t('Log debug information to the system log'),
    '#default_value' => variable_get('cba_debug_log', 0),
  );
  $form['debug']['cba_debug'] = array(
    '#type' => 'radios',
    '#title' => t('Show debug information'),
    '#default_value' => variable_get('cba_debug', 0),
    '#options' => array(
      0 => t('Never'),
      2 => t('In case of errors'),
      1 => t('Always'),
    ),
    '#description' => t('Debug information is only shown for Amazon requests and is visible for users having the permission <a href="!url">%link</a>.', array('%link' => t('Access the Commerce Checkout by Amazon debug log'), '!url' => url('admin/people/permissions', array('fragment' => 'module-commerce_cba')))),
  );
  $form['#validate'][] = 'commerce_cba_settings_form_validate';

  return system_settings_form($form);
}

/**
* Form validation handler for commerce_cba_settings_form().
*
* @see commerce_cba_settings_form()
*/
function commerce_cba_settings_form_validate($form, &$form_state) {
  $amazon_currency = commerce_cba_currency($form_state['values']['cba_country']);
  $enabled_currencies = commerce_currencies(TRUE);
  if (!in_array($amazon_currency, array_keys($enabled_currencies))) {
    form_set_error('cba_country', t('Please enable your Amazon Merchant account currency !currency : !currency_page.', array('!currency' => $amazon_currency, '!currency_page' => l(t('Currency settings page'), 'admin/commerce/config/currency'))));
  }
}
