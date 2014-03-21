<?php

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Set 'Arapahoe' as default selected install profile.
 */
function system_form_install_select_profile_form_alter(&$form, $form_state) {
  foreach($form['profile'] as $key => $element) {
    $form['profile'][$key]['#value'] = 'arapahoe';
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Set default database options.
 */
//function system_form_install_settings_form_alter(&$form, $form_state) {
  //$form['settings']['mysql']['database']['#default_value'] = 'arapahoe_default';
  //$form['settings']['mysql']['username']['#default_value'] = 'root';
  //$form['settings']['mysql']['password']['#default_value'] = '';
//}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Set a bunch of defaults for error proof design.
 */
function arapahoe_form_install_configure_form_alter(&$form, $form_state) {
  // Set up the default site info.
  $form['site_information']['site_name']['#default_value'] = 'Arapahoe Ridge High School';
//  $form['site_information']['site_mail']['#default_value'] = 'you@yourdomain.com';
  
  // Set up some default account info.
// $form['admin_account']['account']['name']['#default_value'] = 'admin';
//  $form['admin_account']['account']['mail']['#default_value'] = 'you@yourdomain.com';
  
  // Set up the standard location/timezone.
  $form['server_settings']['site_default_country']['#default_value'] = 'US';
  $form['server_settings']['date_default_timezone']['#default_value'] = 'America/Denver';
  
  // Turn on clean URLs.
//  $form['server_settings']['clean_url']['#default_value'] = 1;
  
  // Turn off update checker.
//  $form['update_notifications']['update_status_module']['#default_value'] = array();
}
