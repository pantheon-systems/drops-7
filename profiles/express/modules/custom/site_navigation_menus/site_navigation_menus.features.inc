<?php
/**
 * @file
 * site_navigation_menus.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function site_navigation_menus_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}
