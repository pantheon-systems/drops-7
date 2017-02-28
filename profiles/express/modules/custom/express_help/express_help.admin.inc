<?php
/**
 * @file
 * Functions for express_help that do not need to be loaded every single time.
 */

/**
 * Page callback for admin/config/development/module-manager.
 */
function express_help_admin_settings() {
  $form = array();

  $form['express_help_base_url'] = array(
    '#type' => 'textfield',
    '#size' => 100,
    '#default_value' => variable_get('express_help_base_url'),
    '#title' => t('Base Help URL'),
    '#description' => t('If your help fields are fragments, then this base url will be added to them. e.g "http://www.google.com" Don\'t add a trailing slash.'),
  );

  return system_settings_form($form);
}
