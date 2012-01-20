<?php
/**
 * @file
 * openpublish_listings.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function openpublish_listings_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "boxes" && $api == "box") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function openpublish_listings_views_api() {
  list($module, $api) = func_get_args();
  if ($module == "views" && $api == "views_default") {
    return array("version" => "3.0");
  }
}
