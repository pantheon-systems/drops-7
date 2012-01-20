<?php
/**
 * @file
 * openpublish_article_extra_media.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function openpublish_article_extra_media_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "field_group" && $api == "field_group") {
    return array("version" => "1");
  }
}
