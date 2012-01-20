<?php
/**
 * @file
 * openpublish_related_content.features.inc
 */

/**
 * Implements hook_views_api().
 */
function openpublish_related_content_views_api() {
  list($module, $api) = func_get_args();
  if ($module == "views" && $api == "views_default") {
    return array("version" => "3.0");
  }
}
