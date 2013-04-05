<?php
/**
 * @file
 * commerce_kickstart_merchandising.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function commerce_kickstart_merchandising_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function commerce_kickstart_merchandising_views_api() {
  return array("version" => "3.0");
}

/**
 * Implements hook_node_info().
 */
function commerce_kickstart_merchandising_node_info() {
  $items = array(
    'ad_push' => array(
      'name' => t('Ad Push'),
      'base' => 'node_content',
      'description' => t('Use <em>ad push</em> to promote content on the homepage.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => t('Title field is for administrative purpose only and will not be displayed on the site.'),
    ),
  );
  return $items;
}
