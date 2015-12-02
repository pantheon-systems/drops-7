<?php
/**
 * @file
 * Administrative configurations for the cu_alerts module
 */

/**
 * Page callback for admin/config/system/cu_alerts.
 */
function cu_alerts_admin_settings_form() {
  $form = array();
  $form['cu_alerts_environment'] = array(
    '#type' => 'select',
    '#title' => 'Environment',
    '#default_value' => variable_get('cu_alerts_environment', 'production'),
    '#required' => TRUE,
    '#options' => array(
      'production' => t('Production'),
      'test' => t('Test'),
    ),
    '#description' => t('Test mode will pull from emergency-test.colorado.edu, which should have test alerts activated. Always set this back to production directly after testing.'),
  );

  $form['cu_alerts_use_js'] = array(
    '#type' => 'checkbox',
    '#title' => 'Enable JS alerts',
    '#default_value' => variable_get('cu_alerts_use_js', FALSE),
  );

  return system_settings_form($form);
}
