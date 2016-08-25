<?php
/**
 * @file
 * cu_rss_block.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_rss_block_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "bean_admin_ui" && $api == "bean") {
    return array("version" => "5");
  }
}
