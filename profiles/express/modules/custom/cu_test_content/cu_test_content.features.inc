<?php
/**
 * @file
 * cu_test_content.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_test_content_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "context" && $api == "context") {
    return array("version" => "3");
  }
}
