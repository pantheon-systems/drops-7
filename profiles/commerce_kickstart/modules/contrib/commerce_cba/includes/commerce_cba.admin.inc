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
  );

  $form['cba_public_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Amazon public key'),
    '#description' => t('Amazon public key could be referred to as \'AWS Access Key ID\''),
    '#default_value' => variable_get('cba_public_key', ''),
  );

  $form['cba_secret_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Amazon secret key'),
    '#default_value' => variable_get('cba_secret_key', ''),
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

  return system_settings_form($form);
}
