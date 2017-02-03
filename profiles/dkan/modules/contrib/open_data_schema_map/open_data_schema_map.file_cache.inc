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
    drush_log('API present and loaded', 'success');
    $filename = open_data_schema_map_file_cache_name_helper($api);
    // This will only work for data.json v1.1 right now.
    drush_log('Beginning processing for endpoint (this could take a while)', 'success');
    $render = open_data_schema_map_render_api($api);
    $result = $render['result'];

    // Load the correct output format, render results, and write to a file.
    if ($output_format = open_data_schema_map_output_format_load($api->outputformat)) {
      $response = $output_format['callback']($api, $result);
      drush_log('Saving ' . $filename, 'success');
      file_save_data($response, $filename, FILE_EXISTS_REPLACE);
      drush_log($filename . ' saved', 'success');
    }
  }
}
