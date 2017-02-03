<?php
/**
 * @file
 * Provides Features integration for Open Data Schema Map.
 */

/**
 * Implements hook_features_api().
 */
function open_data_schema_map_features_api() {
  return array(
    'open_data_schema_apis' => array(
      'name' => 'Open Data Schema API Map',
      'default_hook' => 'open_data_schema_apis_defaults',
      'file' => drupal_get_path('module', 'open_data_schema_map') . '/open_data_schema_map.features.inc',
      'feature_source' => TRUE,
    ),
  );
}

/**
 * Implements hook_features_export().
 */
function open_data_schema_apis_features_export($data, &$export, $module_name = '') {
  $export['dependencies']['open_data_schema_map'] = 'open_data_schema_map';

  // Build array of schema name => module name for individual schema deps.
  $schema_modules = array();
  foreach (module_implements('open_data_schema') as $module) {
    $schemas = module_invoke($module, 'open_data_schema');
    foreach ($schemas as $schema) {
      $schema_modules[$schema['short_name']] = $module;
    }
  }

  foreach ($data as $key => $machine_name) {
    // Load the api, check the schema and require the module associated with it.
    if ($map = open_data_schema_map_api_load($machine_name)) {
      $export['features']['open_data_schema_apis'][$machine_name] = $machine_name;
      $export['dependencies'][$schema_modules[$map->api_schema]] = $schema_modules[$map->api_schema];
    }
  }
  return array();
}

/**
 * Implements hook_features_export_options().
 */
function open_data_schema_apis_features_export_options() {
  $apis = array();

  foreach (open_data_schema_map_api_load_all() as $api) {
    $apis[$api->machine_name] = $api->name;
  }

  return $apis;
}

/**
 * Implements hook_features_export_render().
 */
function open_data_schema_apis_features_export_render($module, $data, $export = NULL) {
  $code = array();
  $code[] = '  $open_data_schema_apis = array();';
  $code[] = '';

  $rendered_apis = array();
  foreach ($data as $machine_name => $api) {
    if ($definition = open_data_schema_map_api_load($api)) {
      unset($definition->id);
      $code[] = "  // Exported Open Data API Map: {$definition->name}";
      $api_code = features_var_export($definition, '  ');
      $code[] = "  \$open_data_schema_apis['$api'] = $api_code;";
    }
  }
  $code[] = '  return $open_data_schema_apis;';
  $code = implode("\n", $code);
  return array('open_data_schema_apis_defaults' => $code);
}

/**
 * Implements hook_features_revert().
 */
function open_data_schema_apis_features_revert($module) {
  return open_data_schema_apis_features_rebuild($module);
}

/**
 * Implements hook_features_rebuild().
 */
function open_data_schema_apis_features_rebuild($module) {
  // Overwrite API in DB from code...
  if ($defaults = features_get_default('open_data_schema_apis', $module)) {
    foreach ($defaults as $machine_name => $definition) {
      $existing = open_data_schema_map_api_load($machine_name);
      // @todo add open_data_schema_map_schema_save() function to replace this
      if ($existing && isset($existing->id)) {
        drupal_write_record('open_data_schema_map', $definition, $existing->id);
      }
      else {
        drupal_write_record('open_data_schema_map', $definition);
      }
    }
  }
}
