<?php

/**
 * @file
 * Performance for open_data_schema_map.
 */

$module_path = drupal_get_path('module', 'open_data_schema_map');
require_once $module_path . '/open_data_schema_map.output.inc';

/**
 * Helper to build odsm file cache file names.
 */
function open_data_schema_map_file_cache_name_helper($api, $arguments = array()) {
  $name = 'public://odsm_cache_' . $api->machine_name;
  foreach ($arguments as $argument) {
    $name .= '_' . $argument;
  }
  return $name;
}

/**
 * Generate a link to an api endpoint file cache admin page.
 */
function open_data_schema_map_file_cache_admin_link($api) {
  $label = open_data_schema_map_file_cache_admin_link_label($api);
  return l($label, OPEN_DATA_SCHEMA_MAP_ADMIN_PATH . '/cache/' . $api->machine_name);
}

/**
 * Get cache age in hours or false if none exists.
 */
function open_data_schema_map_file_cache_age($api) {
  if (open_data_schema_map_file_cache_exits($api)) {
    $filename = open_data_schema_map_file_cache_name_helper($api);
    $stat = stat($filename);
    $seconds_in_hour = 3600;
    $time = time();
    $elapsed_time = $time > $stat['ctime'] ? $time - $stat['ctime'] : 0;
    return floor($elapsed_time / $seconds_in_hour);
  }

  return FALSE;
}

/**
 * Generate a label for api endpoint file cache admin page link.
 */
function open_data_schema_map_file_cache_admin_link_label($api) {
  $label = 'none';
  $age = open_data_schema_map_file_cache_age($api);

  if (is_numeric($age)) {
    $label = "$age " . t('hrs');
  }
  return $label;
}

/**
 * Generate a status for api endpoint file cache admin page.
 */
function open_data_schema_map_file_cache_status($api) {
  $status = t('Not cached');
  $age = open_data_schema_map_file_cache_age($api);

  if (is_numeric($age)) {
    $status = "$age " . t('hours ago');
  }
  return $status;
}

/**
 * Check existance of a file_cache for an api endpoint.
 */
function open_data_schema_map_file_cache_exits($api, $arguments = array()) {
  return file_exists(open_data_schema_map_file_cache_name_helper($api, $arguments));
}

/**
 * File caches a specific odsm endpoint.
 */
function open_data_schema_map_file_cache_endpoint($machine_name) {
  $api = open_data_schema_map_api_load($machine_name);
  if (isset($api)) {
    watchdog('open_data_schema_map', 'api present and loaded', WATCHDOG_INFO);
    $filename = open_data_schema_map_file_cache_name_helper($api);
    // This will only work for data.json v1.1 right now.
    watchdog('open_data_schema_map', 'Beginning processing for endpoint (this could take a while)', WATCHDOG_INFO);
    $render = open_data_schema_map_render_api($api);
    $result = $render['result'];

    // Load the correct output format, render results, and write to a file.
    if ($output_format = open_data_schema_map_output_format_load($api->outputformat)) {
      $response = $output_format['callback']($api, $result);
      watchdog('open_data_schema_map', "Saving $filename", WATCHDOG_INFO);
      file_save_data($response, $filename, FILE_EXISTS_REPLACE);
      watchdog('open_data_schema_map', "$filename saved", WATCHDOG_INFO);
    }
  }
}

/**
 * API wrapper for open_data_schema_map_file_cache_endpoint.
 */
function open_data_schema_map_file_cache_create($api) {
  open_data_schema_map_file_cache_endpoint($api->machine_name);
}

/**
 * Delete a specific api endpoint file cache.
 */
function open_data_schema_map_file_cache_delete($api) {
  $filename = open_data_schema_map_file_cache_name_helper($api);
  if (file_exists($filename)) {
    file_unmanaged_delete($filename);
  }
}

/**
 * Callback for the api endpoint file cache admin page.
 */
function open_data_schema_map_cache_manage(array $form, array &$form_state, $api = NULL) {
  $name = $api->name;
  $title = "$name Cache";
  drupal_set_title($title, FALSE);
  $form['help'] = array(
    '#type' => 'item',
    '#markup' => "The Open Data Schema Map filecache writes an open data endpoint such as data.json to disk, so that it does not need to be generated upon every request. See the <a href='http://docs.getdkan.com/en/latest/components/open-data-schema.html#the-odsm-file-cache'>documentation</a> for instructions on automating this process. Use the controls on this page to manually generate or remove this API's filecache.",
  );

  $status_label = t("Filecache status");
  $status = open_data_schema_map_file_cache_status($api);

  $form['status'] = array(
    '#type' => 'item',
    '#markup' => "<strong>$status_label:</strong> $status",
  );

  if (open_data_schema_map_file_cache_exits($api)) {
    $form['rebuild'] = array('#type' => 'submit', '#value' => t('Rebuild'));
    $form['delete'] = array('#type' => 'submit', '#value' => t('Delete'));
  }
  else {
    $form['create'] = array('#type' => 'submit', '#value' => t('Create'));
  }
  $form['#api'] = $api;
  return $form;
}

/**
 * Submit function for managing the api endpoint file caches.
 */
function open_data_schema_map_cache_manage_submit(array $form, array &$form_state) {
  $api = $form['#api'];
  $op = $form_state['input']['op'];

  if ($op == t('Create') || $op == t('Rebuild')) {
    open_data_schema_map_file_cache_create($api);
    drupal_set_message(t('A cache file for the %name api endpoint has been created.', array('%name' => $api->name)));
  }
  elseif ($op == t('Delete')) {
    drupal_set_message(t('A cache file for the %name api endpoint has been deleted.', array('%name' => $api->name)));
    open_data_schema_map_file_cache_delete($api);
  }

  // Rebuild menu so the api is not available anymore.
  menu_rebuild();

  drupal_goto('admin/config/services/odsm');
}
