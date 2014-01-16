<?php
/**
 * @file
 * Admin UI for Commerce cba
 */

function commerce_cba_admin_form($form, &$form_state) {
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

  return system_settings_form($form);
}
