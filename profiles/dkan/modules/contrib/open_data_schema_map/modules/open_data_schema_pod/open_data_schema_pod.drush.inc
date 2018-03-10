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
  _open_data_schema_map_drush_validate('pod');
}
