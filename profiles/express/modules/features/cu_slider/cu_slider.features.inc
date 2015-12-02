<?php
/**
 * @file
 * cu_slider.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_slider_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "bean_admin_ui" && $api == "bean") {
    return array("version" => "5");
  }
  if ($module == "field_group" && $api == "field_group") {
    return array("version" => "1");
  }
  if ($module == "flexslider" && $api == "flexslider_default_preset") {
    return array("version" => "1");
  }
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function cu_slider_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}
