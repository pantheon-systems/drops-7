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
  $items['odsm-validate'] = array(
    'aliases' => array('odsmv'),
    'description' => 'Validate Open Data Schema Map',
    'callback' => '_open_data_schema_map_drush_validate',
    'arguments' => array(
      'schema' => 'Schema to validate (eg. dcat, pod)',
    ),
  );

  return $items;
}

/**
 * Runs validation for drush.
 */
function _open_data_schema_map_drush_validate($schema) {
  // @todo Assumes class name is based on schema name.
  $validator_class = ucfirst($schema) . 'Validator';

  $results = _open_data_schema_map_process_validate($validator_class, TRUE);
  if ($results['errors']) {
    drush_log('Validation error.', 'error');
    drush_print_r($results['errors']);
  }
  else {
    drush_log('Validation successful.', 'success');
  }
}
