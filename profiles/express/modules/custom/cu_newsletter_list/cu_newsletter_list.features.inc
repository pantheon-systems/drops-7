<?php
/**
 * @file
 * cu_newsletter_list.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_newsletter_list_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "bean_admin_ui" && $api == "bean") {
    return array("version" => "5");
  }
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}
