<?php

/**
 * @file
 * Provides a panels_storage plugin for Panelizer entity overrides.
 */

// Plugin definition
$plugin = array(
  'access callback' => 'panelizer_entity_panels_storage_access',
);

/**
 * Access callback for panels storage.
 */
function panelizer_entity_panels_storage_access($storage_type, $storage_id, $op, $account) {
  list ($entity_type, $entity_id, $view_mode) = explode(':', $storage_id);

  $entities = entity_load($entity_type, array($entity_id));
  $entity = reset($entities);
  if (!$entity) {
    return FALSE;
  }

  $handler = panelizer_entity_plugin_get_handler($entity_type);
  if (!$handler) {
    return FALSE;
  }

  if ($op == 'read') {
    return $handler->entity_access('view', $entity);
  }

  // The 'layout' operation has a special permission.
  if ($op == 'change layout') {
    return $handler->access_admin($entity, 'layout', $view_mode);
  }
  else {
    return $handler->access_admin($entity, 'content', $view_mode);
  }
}
