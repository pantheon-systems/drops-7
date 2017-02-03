<?php
/**
 * @file
 * panopoly_widgets.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function panopoly_widgets_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "file_entity" && $api == "file_default_displays") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function panopoly_widgets_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}
