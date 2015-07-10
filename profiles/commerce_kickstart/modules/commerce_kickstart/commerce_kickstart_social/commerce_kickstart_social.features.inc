<?php
/**
 * @file
 * commerce_kickstart_social.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function commerce_kickstart_social_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "oauthconnector" && $api == "oauthconnector") {
    return array("version" => "1");
  }
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}
