<?php
/**
 * @file
 * visualization_entity_visualization_contributor_role.features.user_role.inc
 */

/**
 * Implements hook_user_default_roles().
 */
function visualization_entity_visualization_contributor_role_user_default_roles() {
  $roles = array();

  // Exported role: visualization contributor.
  $roles['visualization contributor'] = array(
    'name' => 'visualization contributor',
    'weight' => 6,
    'machine_name' => 'visualization_contributor',
  );

  return $roles;
}
