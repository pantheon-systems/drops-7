<?php
/**
 * @file
 * cu_collections.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_collections_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "bean_admin_ui" && $api == "bean") {
    return array("version" => "5");
  }
  if ($module == "field_group" && $api == "field_group") {
    return array("version" => "1");
  }
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_node_info().
 */
function cu_collections_node_info() {
  $items = array(
    'collection_item' => array(
      'name' => t('Collection Item'),
      'base' => 'node_content',
      'description' => t('Create collections of items grouped by custom filters.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  drupal_alter('node_info', $items);
  return $items;
}
