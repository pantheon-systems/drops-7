<?php
/**
 * @file
 * cu_facebook_like_button.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_facebook_like_button_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "bean_admin_ui" && $api == "bean") {
    return array("version" => "5");
  }
}
