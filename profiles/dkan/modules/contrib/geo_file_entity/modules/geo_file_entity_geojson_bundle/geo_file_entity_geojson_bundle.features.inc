<?php
/**
 * @file
 * geo_file_entity_geojson_bundle.features.inc
 */

/**
 * Implements hook_eck_bundle_info().
 */
function geo_file_entity_geojson_bundle_eck_bundle_info() {
  $items = array(
    'geo_file_geojson' => array(
      'machine_name' => 'geo_file_geojson',
      'entity_type' => 'geo_file',
      'name' => 'geojson',
      'label' => 'geojson',
    ),
  );
  return $items;
}
