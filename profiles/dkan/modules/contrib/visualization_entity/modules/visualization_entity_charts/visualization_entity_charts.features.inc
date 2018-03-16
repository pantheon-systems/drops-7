<?php
/**
 * @file
 * visualization_entity_charts.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function visualization_entity_charts_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "field_group" && $api == "field_group") {
    return array("version" => "1");
  }
}

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
