<?php
/**
 * @file
 * cu_content_lock.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_content_lock_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}
