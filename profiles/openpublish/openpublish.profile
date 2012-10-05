<?php
// $Id$

/**
 * Implements hook_form_alter().
 *
 * Allows the profile to alter the site configuration form. Adds a default
 * value for site name during the configure step of site install.
 */
function openpublish_form_alter(&$form, $form_state, $form_id) {
  if ($form_id == 'install_configure_form') {
    // Set default for site name field.
    $form['site_information']['site_name']['#default_value'] = $_SERVER['SERVER_NAME'];
  }
}

/**
 * Implements hook_appstore_stores_info().
 *
 * Used to get an apps manifest from the Opish apps server.
 * Identifies current site to server for voting and other functionality.
 */
function openpublish_apps_servers_info() {
 $info =  drupal_parse_info_file(dirname(__file__) . '/openpublish.info');
 return array(
   'openpublish' => array(
     'title' => 'Openpublish',
     'description' => "Apps for the OpenPublish distribution",
     'manifest' => variable_get('openpublish_apps_server_url', 'http://appserver.openpublishapp.com/app/query/openpublish'),
     'profile' => 'Openpublish',
     'profile_version' => isset($info['version']) ? $info['version'] : '7.x-1.0-alpha2',
     'server_name' => isset($GLOBALS['base_url']) ? $GLOBALS['base_url'] : '',
     'server_ip' => isset($_SERVER['SERVER_ADDR']) ? $_SERVER['SERVER_ADDR'] : '',
   ),
 );
}

/**
 * Implements hook_init()
 *
 * Adds data to the js settings array about the profile.
 */
function openpublish_init() {
 $cache = cache_get("openpublish_info");
 if (isset($cache->data)) {
   $data = $cache->data;
 }
 else {
   $info =  drupal_parse_info_file(dirname(__file__) . '/openpublish.info');
   $version = array_key_exists('version', $info) ? $info['version'] : '7.x-1.x';
   $data = array("profile" => "openpublish", "profile_version" => $version);
   cache_set("openpublish_info", $data);
 }
 drupal_add_js($data, 'setting');
}
