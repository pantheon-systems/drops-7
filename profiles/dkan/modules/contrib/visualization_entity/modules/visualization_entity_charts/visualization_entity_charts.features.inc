<?php
/**
 * @file
 * visualization_entity_charts.features.inc
 */

/**
 * Implements hook_eck_bundle_info().
 */
function visualization_entity_charts_eck_bundle_info() {
  $items = array(
    'visualization_ve_chart' => array(
      'machine_name' => 'visualization_ve_chart',
      'entity_type' => 'visualization',
      'name' => 've_chart',
      'label' => 'Chart',
    ),
  );
  return $items;
}
