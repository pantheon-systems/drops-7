<?php
/**
 * @file
 * cu_redirect_2.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_redirect_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}
