<?php
/**
 * @file
 * Integration with references dialog.
 */

/**
 * Implements hook_references_dialog_entity_admin_paths().
 */
function eck_references_dialog_entity_admin_paths() {
  $admin_paths = array();
  $path = eck__entity_type__path();
  foreach (EntityType::loadAll() as $entity_type) {
    $admin_paths["{$entity_type->name}"] = array(
      'add' => "{$path}/{$entity_type->name}/[bundle]/add",
      'edit' => "{$path}/{$entity_type->name}/[bundle]/[entity_id]/edit",
    );
  }
  return $admin_paths;
}
