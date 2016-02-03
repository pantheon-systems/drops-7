<?php
/**
 * @file
 * visualization_entity_visualization_contributor_role.features.roles_permissions.inc
 */

/**
 * Implements hook_default_roles_permissions().
 */
function visualization_entity_visualization_contributor_role_default_roles_permissions() {
  $roles = array();

  // Exported role: visualization contributor
  $roles['visualization contributor'] = array(
    'name' => 'visualization contributor',
    'weight' => 6,
    'machine_name' => 'visualization_contributor',
    'permissions' => array(
      'eck add geo_file geojson entities' => TRUE,
      'eck add visualization choropleth_visualization entities' => TRUE,
      'eck add visualization geojson_visualization entities' => TRUE,
      'eck administer visualization choropleth_visualization entities' => TRUE,
      'eck administer visualization geojson_visualization entities' => TRUE,
      'eck delete geo_file geojson entities' => TRUE,
      'eck delete visualization choropleth_visualization entities' => TRUE,
      'eck delete visualization geojson_visualization entities' => TRUE,
      'eck edit geo_file geojson entities' => TRUE,
      'eck edit visualization choropleth_visualization entities' => TRUE,
      'eck edit visualization geojson_visualization entities' => TRUE,
      'eck list bundles' => TRUE,
      'eck list entities' => TRUE,
      'eck list entity types' => TRUE,
      'eck list geo_file geojson entities' => TRUE,
      'eck list visualization choropleth_visualization entities' => TRUE,
      'eck list visualization geojson_visualization entities' => TRUE,
      'eck view entities' => TRUE,
      'eck view geo_file geojson entities' => TRUE,
      'eck view visualization choropleth_visualization entities' => TRUE,
      'eck view visualization geojson_visualization entities' => TRUE,
    ),
  );

  return $roles;
}
