<?php

/**
 * @file
 * Admin forms and pages for cu_logout_users module.
 */

/**
 * Form callback for admin/config/people/cu-logout-users.
 */
function cu_logout_users_admin_form() {
  $form = array();

  $form['cu_logout_users_text'] = array(
    '#title' => t('Logout Text'),
    '#description' => t('This text will appear as a message for the user when logging into the website has been disabled.'),
    '#type' => 'textfield',
    '#default_value' => variable_get('cu_logout_users_text', ''),
    '#required' => TRUE,
  );

  return system_settings_form($form);
}
