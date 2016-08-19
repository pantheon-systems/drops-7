<?php

/**
 * @file
 * Provides a panels_storage plugin for Panelizer defaults.
 */

// Plugin definition
$plugin = array(
  'access callback' => 'panelizer_default_panels_storage_access',
);

/**
 * Access callback for panels storage.
 */
function panelizer_default_panels_storage_access($storage_type, $storage_id, $op, $account) {
  list ($entity_type, $bundle, $id) = explode(':', $storage_id);
  if ($op == 'change layout') {
    return user_access("administer panelizer $entity_type $bundle defaults", $account) || user_access("administer panelizer $entity_type $bundle layout", $account);
  }
  else {
    return user_access("administer panelizer $entity_type $bundle defaults", $account) || user_access("administer panelizer $entity_type $bundle content", $account);
  }
}
