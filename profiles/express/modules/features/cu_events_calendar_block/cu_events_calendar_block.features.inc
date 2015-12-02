<?php
/**
 * @file
 * cu_events_calendar_block.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_events_calendar_block_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "bean_admin_ui" && $api == "bean") {
    return array("version" => "5");
  }
  if ($module == "field_group" && $api == "field_group") {
    return array("version" => "1");
  }
}
