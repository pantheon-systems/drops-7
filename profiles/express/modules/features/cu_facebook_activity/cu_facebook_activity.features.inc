<?php
/**
 * @file
 * cu_facebook_activity.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_facebook_activity_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "bean_admin_ui" && $api == "bean") {
    return array("version" => "5");
  }
}
