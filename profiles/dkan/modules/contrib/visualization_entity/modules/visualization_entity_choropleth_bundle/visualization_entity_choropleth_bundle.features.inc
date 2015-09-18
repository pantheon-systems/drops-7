<?php
/**
 * @file
 * visualization_entity_choropleth_bundle.features.inc
 */

/**
 * Implements hook_eck_bundle_info().
 */
function visualization_entity_choropleth_bundle_eck_bundle_info() {
  $items = array(
    'visualization_choropleth_visualization' => array(
      'machine_name' => 'visualization_choropleth_visualization',
      'entity_type' => 'visualization',
      'name' => 'choropleth_visualization',
      'label' => 'Choropleth visualization',
    ),
  );
  return $items;
}
