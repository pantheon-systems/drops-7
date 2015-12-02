<?php
/**
 * @file
 * quicktab_beans.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function quicktab_beans_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "bean_admin_ui" && $api == "bean") {
    return array("version" => "5");
  }
}
