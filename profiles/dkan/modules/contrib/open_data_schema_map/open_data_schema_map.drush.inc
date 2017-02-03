<?php
/**
 * @file
 * Drush commands for open_data_schema_map.
 */
/**
 * Implements hook_drush_command().
 */
function open_data_schema_map_drush_command() {

  // Drop datastore.
  $items['odsm-filecache'] = array(
    'aliases' => array('odsmfc'),
    'description' => '.',
    'callback' => 'open_data_schema_map_file_cache_endpoint',
    'arguments' => array(
      'machine_name' => 'Machine name for the odsm endpoint to be cached',
    ),
  );
  return $items;
}
