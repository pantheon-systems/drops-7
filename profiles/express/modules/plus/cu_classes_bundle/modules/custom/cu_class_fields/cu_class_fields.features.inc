<?php
/**
 * @file
 * cu_class_fields.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_class_fields_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "field_group" && $api == "field_group") {
    return array("version" => "1");
  }
}
