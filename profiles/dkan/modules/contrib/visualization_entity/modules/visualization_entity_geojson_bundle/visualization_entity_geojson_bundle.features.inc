<?php
/**
 * @file
 * visualization_entity_geojson_bundle.features.inc
 */

/**
 * Implements hook_eck_bundle_info().
 */
function visualization_entity_geojson_bundle_eck_bundle_info() {
  $items = array(
    'visualization_geojson_visualization' => array(
      'machine_name' => 'visualization_geojson_visualization',
      'entity_type' => 'visualization',
      'name' => 'geojson_visualization',
      'label' => 'Geojson Visualization',
    ),
  );
  return $items;
}
