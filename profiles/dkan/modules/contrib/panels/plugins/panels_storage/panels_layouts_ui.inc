<?php

/**
 * @file
 * Provides a panels_storage plugin for the reusable flexible layouts UI.
 */

// Plugin definition.
$plugin = array(
  'access callback' => 'panels_layouts_ui_panels_storage_access',
);

/**
 * Access callback for panels storage.
 */
function panels_layouts_ui_panels_storage_access($storage_type, $storage_id, $op, $account) {
  return user_access('administer panels layouts', $account);
}
