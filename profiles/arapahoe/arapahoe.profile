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
 * Set a bunch of defaults for error proof design.
 */
function arapahoe_form_install_configure_form_alter(&$form, $form_state) {
  // Set up the default site info.
  $form['site_information']['site_name']['#default_value'] = 'Arapahoe Ridge High School';
//$form['site_information']['site_mail']['#default_value'] = 'you@yourdomain.com';
  
  // Set up some default account info.
// $form['admin_account']['account']['name']['#default_value'] = 'admin';
// $form['admin_account']['account']['mail']['#default_value'] = 'you@yourdomain.com';
  
  // Set up the standard location/timezone.
  $form['server_settings']['site_default_country']['#default_value'] = 'US';
  $form['server_settings']['date_default_timezone']['#default_value'] = 'America/Denver';
  
  // Turn off update checker.
  $form['update_notifications']['update_status_module']['#default_value'] = array();
}

function arapahoe_install_tasks($install_state) {
  $tasks = array (
    'arapahoe_configure' => array(),
  );
  return $tasks;
}

/**
 * Set up base config
 */
function arapahoe_configure() {
  // Set default Pantheon variables
  variable_set('cache', 1);
  variable_set('block_cache', 1);
  variable_set('cache_lifetime', '0');
  variable_set('page_cache_maximum_age', '900');
  variable_set('page_compression', 0);
  variable_set('preprocess_css', 1);
  variable_set('preprocess_js', 1);
  $search_active_modules = array(
    'apachesolr_search' => 'apachesolr_search',
    'user' => 'user',
    'node' => 0
  );
  variable_set('search_active_modules', $search_active_modules);
  variable_set('search_default_module', 'apachesolr_search');
  drupal_set_message(t('Pantheon defaults configured.'));
}
