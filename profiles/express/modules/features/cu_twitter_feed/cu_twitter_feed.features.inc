<?php
/**
 * @file
 * cu_twitter_feed.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_twitter_feed_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "bean_admin_ui" && $api == "bean") {
    return array("version" => "5");
  }
}
