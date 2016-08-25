<?php
/**
 * @file
 * cu_sitemap.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_sitemap_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}
