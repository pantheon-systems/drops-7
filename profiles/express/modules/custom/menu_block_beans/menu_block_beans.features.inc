<?php
/**
 * @file
 * menu_block_beans.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function menu_block_beans_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "bean_admin_ui" && $api == "bean") {
    return array("version" => "5");
  }
}
