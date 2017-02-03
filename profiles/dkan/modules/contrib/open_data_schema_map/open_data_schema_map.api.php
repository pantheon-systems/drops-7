<?php

/**
 * @file
 * Let there be hooks.
 */

/******** Add Endpoint in code *******/

/**
 * Adds declared endpoint to list.
 *
 * This and hook_open_data_schema_map_load() are necessary so that modules can
 * declare more than one endpoint.
 */
function hook_open_data_schema_map_endpoints_alter(&$records) {
  $records[] = 'my_machine_name';
}

/**
 * Loads endpoints with callback.
 *
 * @return object
 *   Record object with the following. All are required:
 *     - name
 *     - enabled
 *     - schema
 *     - entity
 *     - bundle
 *     - arguments
 *     - description
 *     - machine_name
 *     - endpoint
 *     - callback
 */
function hook_open_data_schema_map_load($machine_name) {
  if ($machine_name == 'my_machine_name') {
    $record = new stdClass();
    $record->name = 'My endpoint name';
    $record->enabled = TRUE;
    $record->schema = '';
    $record->entity = '';
    $record->bundle = '';
    $record->arguments = '';
    $record->machine_name = 'my_machine_name';
    $record->endpoint = 'api/action/3/my_endpoint';
    $record->callback = 'my_module_endpoint_callback';
    return $record;
  }
}

/**
 * Callback for my custom endpoint.
 *
 * @return array
 *   Results of my custom function or query.
 */
function my_module_endpoint_callback($queries, $args) {
  // Here we can call a query on a single table or provide other callback to
  // generate items. This endpoint has not arguments so ignoring those.
  $items = my_sudo_code_query();
  $results = array(
    'description' => t('My endpoint'),
    'results' => $items,
  );
  return $results;
}

/******** Add schema *******/

/**
 * Declare new open data schema.
 */
function hook_open_data_schema() {
  return array(
    'short_name' => 'new_schema',
    'title' => 'MY New Schema',
    // This is the path to the schema. Schema MUST be in json 3 or json
    // 4 format.
    'schema_file' => $path,
    'description' => t('This new schema rocks.'),
  );

}

/**
 * Allows adding new schema types.
 *
 * Currently onl json-4 and json-3 are accepted.
 */
function hook_open_data_schema_map_schema_types_alter(&$schemas) {
}

/******** Update results *******/

/**
 * Allows overriding final results about to be rendered.
 */
function hook_open_data_schema_map_results_alter(&$result, $api_machine_name, $schema) {
  if ($schema == 'new_schema') {
    // Wrap results in 'output' array.
    $result['output'] = $results;
  }
}

/**
 * Allows changing the output of a processed field.
 */
function hook_open_data_schema_map_process_field_alter(&$result, $api_field, $token) {
  if ($api_field == 'foo') {
    $result = 'bar';
  }
}

/**
 * Allows altering of arguments before they are queried.
 *
 * @param array $token
 *   Exploded token. This will be queried by $query->fieldConditioin if
 *   $token[0] = 'node' or $query->propertyCondition if $token[0] = 'field'.
 *   $token[1] will be the node property or field queried.
 * @param array $arg
 *   Exploded argument.
 */
function hook_open_data_schema_map_args_alter(&$token, &$arg) {
  if ($arg['token']['value'] == '[node:url:arg:last]') {
    // Change property being queried. The nid column in the node table will now
    // be queried.
    $token[1] = 'nid';
  }
}
