<?php
/**
 * @file
 * cu_feature_callout.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_feature_callout_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "bean_admin_ui" && $api == "bean") {
    return array("version" => "5");
  }
  if ($module == "field_group" && $api == "field_group") {
    return array("version" => "1");
  }
}
