<?php

/**
 * @file
 * Drush commands for pod.
 */

/**
 * Implements hook_drush_command().
 */
function open_data_schema_pod_drush_command() {
  $items['open-data-schema-pod-validate'] = array(
    'aliases' => array('data-json-validate'),
    'description' => 'Validates local data.json',
    'callback' => 'open_data_schema_pod_drush_validate',
  );
  return $items;
}

/**
 * Runs data.json validation for drush.
 */
function open_data_schema_pod_drush_validate() {
  global $base_url;
  $results = open_data_schema_pod_process_validate($base_url . '/data.json', TRUE);
  if ($results['errors']) {
    drush_log('Data.json does not validate.', 'error');
  }
  else {
    drush_log('Data.json validates.', 'success');
  }
}
