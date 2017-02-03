<?php

/**
 * @file Panelizer views integration code.
 */

/**
 * Implements hook_views_data_alter().
 */
function panelizer_views_data_alter(&$data) {
  // Delegate.
  foreach (panelizer_get_plugins_with_hook('views_data_alter') as $handler) {
    $handler->hook_views_data_alter($data);
  }
}

/**
 * Implements hook_views_plugins_alter().
 */
function panelizer_views_plugins_alter(&$plugins) {
  // Delegate.
  foreach (panelizer_get_plugins_with_hook('views_plugins_alter') as $handler) {
    $handler->hook_views_plugins_alter($plugins);
  }
}

