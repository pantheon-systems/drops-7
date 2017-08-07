<?php
/**
 * @file
 * cu_social_links.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_social_links_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "bean_admin_ui" && $api == "bean") {
    return array("version" => "5");
  }
}
