<?php
/**
 * @file
 * Administrative page callbacks and forms for Commerce Exactor EUC.
 */


/**
 * Builds the EUC shipping settings form.
 */
function commerce_exactor_euc_shipping_settings_form($form, &$form_state) {

  $shipping_services = commerce_shipping_services();

  $shipping_euc = variable_get('commerce_exactor_euc_shipping', array('default' => 'EUC-13010204'));

  $form['commerce_exactor_euc_shipping']['default'] = array(
    '#type' => 'select',

    '#title' => t('Default Exactor EUC'),
    '#description' => t('Select the default shipping EUC'),

    '#required' => TRUE,
    '#options' => commerce_exactor_euc_options_list(),
    '#default_value' => $shipping_euc['default'],
  );

  foreach ($shipping_services as $shipping_service) {

    if (isset($shipping_euc[$shipping_service['name']])) {
      $default_value = $shipping_euc[$shipping_service['name']];
    }
    else {
      $default_value = $shipping_euc['default'];
    }

    $form['commerce_exactor_euc_shipping'][$shipping_service['name']] = array(
      '#type' => 'select',

      '#title' => $shipping_service['title'],
      '#description' => t('Select the EUC for this shipping service'),

      '#required' => TRUE,
      '#options' => commerce_exactor_euc_options_list(),
      '#default_value' => $default_value,
    );

  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Form submit callback.
 */
function commerce_exactor_euc_shipping_settings_form_submit($form, &$form_state) {

  $shipping_services = commerce_shipping_services();

  $shipping_euc = variable_get('commerce_exactor_euc_shipping', array());

  $shipping_euc['default'] = $form_state['values']['default'];

  foreach ($shipping_services as $shipping_service) {
    $shipping_euc[$shipping_service['name']] = $form_state['values'][$shipping_service['name']];
  }

  variable_set('commerce_exactor_euc_shipping', $shipping_euc);
}
