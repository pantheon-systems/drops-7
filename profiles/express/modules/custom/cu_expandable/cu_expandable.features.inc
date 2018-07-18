<?php
/**
 * @file
 * cu_expandable.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_expandable_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "bean_admin_ui" && $api == "bean") {
    return array("version" => "5");
  }
  if ($module == "field_group" && $api == "field_group") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function cu_expandable_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}
