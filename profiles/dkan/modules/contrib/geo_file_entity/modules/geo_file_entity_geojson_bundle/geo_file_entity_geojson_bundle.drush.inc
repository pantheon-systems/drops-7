<?php

/**
 * @file
 * Drush integration for the geo_file_entity_geojson_bundle module.
 */

/**
 * Implements hook_drush_command().
 */
function geo_file_entity_geojson_bundle_drush_command() {
  $items['preload-geojson'] = array(
    'description' => dt('Create a base of commonly used geojson files eg: USA States'),
    'arguments' => array(),
    'aliases' => array('pre-geo'),
  );
  return $items;
}

/**
 * Implements hook_drush_help().
 */
function geo_File_entity_geojson_bundle_drush_help($command) {
  switch ($command) {
    case 'preload-geojson':

    case 'pre-geo':

      return dt('Preload Commonly used geojson files eg: USA States');

    break;
  }
}
/**
 * drush preload-geojson command callback.
 */
function drush_geo_file_entity_geojson_bundle_preload_geojson() {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'geo_file')
    ->entityCondition('bundle', 'geojson')
    ->entityCondition('title', 'USA States Polygons')
    ->range(0, 1)
    ->addMetaData('account', user_load(1));
  $result = $query->execute();

  if (!isset($result['geo_file'])) {
    // Create USA States file.
    $file_path = drupal_get_path('module', 'geo_file_entity_geojson_bundle');
    $file_path = realpath($file_path . '/data/usa_states.json');
    $file = file_get_contents($file_path);
    $file = file_save_data($file, 'public://usa_states.json');
    $file = (array) $file;
    $file['display'] = 1;

    $usa_states = array();
    $usa_states['title'] = 'USA States Polygons';
    $usa_states['type'] = 'geojson';
    $usa_states['field_name_attribute'] = array(
      LANGUAGE_NONE => array(
        array(
          'value' => 'name'
        ),
      ),
    );
    $usa_states['field_file'] = array(
      LANGUAGE_NONE => array(
        $file
      ),
    );
    $usa_states = entity_create('geo_file', $usa_states);
    $usa_states->save();
  }
}
