<?php

/**
 * @file
 * Theme settings.
 */

 /**
  * Implements hook_form_FORM_ID_alter().
  *
  * Add theme settings.
  */
function curise_form_system_theme_settings_alter(&$form, &$form_state) {
  $form['expressbase_theme_settings']['banner'] = array(
    '#type' => 'fieldset',
    '#title' => t('Banner Color'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['expressbase_theme_settings']['banner']['banner_color'] = array(
    '#type' => 'select',
    '#title' => t('Banner Color'),
    '#default_value' => theme_get_setting('banner_color', 'curise') ? theme_get_setting('banner_color', 'curise') : 'black',
    '#description' => t('Pick a banner color for your site.'),
    '#options' => array(
      'dark' => t('Dark'),
      'light' => t('Light'),
    ),
  );
}
