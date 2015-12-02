<?php

/**
 * @file
 * provides a debug mode for module ThemeKey.
 * @see themekey.module
 *
 * @author Markus Kalkbrenner | Cocomore AG
 *   @see http://drupal.org/user/124705
 */


/**
 * Form builder for the form to enable ThemeKey debug mode.
 *
 * @ingroup forms
 */
function themekey_debug_settings_form() {
  $form = array();
  $form['themekey_debug'] = array(
    '#type' => 'fieldset',
    '#title' => t('Debug Settings'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#description' => t("Debug information will only be visible to the site's administrator."),
  );

  $form['themekey_debug']['themekey_debug_show_property_values'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show values of ThemeKey properties'),
    '#default_value' => variable_get('themekey_debug_show_property_values', FALSE),
    '#description' => t('Shows current values of ThemeKey properties at the bottom of every page. Additionally you can start creating a corresponding Theme Switching Rule by clicking on a value.'),
  );

  $form['themekey_debug']['themekey_debug_trace_rule_switching'] = array(
    '#type' => 'checkbox',
    '#title' => t('Trace ThemeKey rule switching'),
    '#default_value' => variable_get('themekey_debug_trace_rule_switching', FALSE),
    '#description' => t('Prints out detailed information about matching or non-matching rules.'),
  );

  $form['themekey_debug']['themekey_debug_non_admin_users'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show debug information to non-admin users'),
    '#default_value' => variable_get('themekey_debug_non_admin_users', FALSE),
    '#description' => t('All debug output will be shown to all users, including anonymous users. Be careful in production environments and turn off page caching first!'),
  );


  return system_settings_form($form);
}
