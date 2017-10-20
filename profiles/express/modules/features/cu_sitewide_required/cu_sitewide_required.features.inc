<?php
/**
 * @file
 * cu_sitewide_required.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_sitewide_required_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "context" && $api == "context") {
    return array("version" => "3");
  }
}
