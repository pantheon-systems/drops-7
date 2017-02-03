<?php
/**
 * @file
 * cu_bucket.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_bucket_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}
