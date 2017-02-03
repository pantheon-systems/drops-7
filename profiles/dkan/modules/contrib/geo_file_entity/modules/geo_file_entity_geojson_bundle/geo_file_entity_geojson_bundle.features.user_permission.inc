<?php
/**
 * @file
 * geo_file_entity_geojson_bundle.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function geo_file_entity_geojson_bundle_user_default_permissions() {
  $permissions = array();

  // Exported permission: 'eck view geo_file geojson entities'.
  $permissions['eck view geo_file geojson entities'] = array(
    'name' => 'eck view geo_file geojson entities',
    'roles' => array(
      'anonymous user' => 'anonymous user',
    ),
    'module' => 'eck',
  );

  return $permissions;
}
