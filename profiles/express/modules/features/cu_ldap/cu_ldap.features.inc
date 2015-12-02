<?php
/**
 * @file
 * cu_ldap.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_ldap_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "ldap_servers" && $api == "ldap_servers") {
    return array("version" => "1");
  }
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}
