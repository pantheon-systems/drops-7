<?php

/**
 * Implements hook_features_export_options().
 */
function commerce_payment_method_features_export_options() {
  $options = array();

  $payment_methods = commerce_payment_methods();
  foreach ($payment_methods as $method) {
    $options[$method['method_id']] = $method['title'];
  }

  return $options;
}

/**
 * Implements hook_features_export().
 */
function commerce_payment_method_features_export($data, &$export, $module_name = '') {
  $pipe = array();

  $export['dependencies']['features'] = 'features';
  $export['dependencies']['commerce_payment'] = 'commerce_payment';
  $export['dependencies']['rules'] = 'rules';

  $payment_methods = commerce_payment_methods();
  foreach ($data as $method) {
    if (isset($payment_methods[$method]['module'])) {
      $export['dependencies'][$payment_methods[$method]['module']] = $payment_methods[$method]['module'];
    }

    $export['features']['commerce_payment_method'][$method] = $method;
    $export['features']['rules_config']['commerce_payment_' . $method] = 'commerce_payment_' . $method;
  }

  return $pipe;
}
