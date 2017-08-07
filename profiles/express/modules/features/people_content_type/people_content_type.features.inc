<?php
/**
 * @file
 * people_content_type.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function people_content_type_ctools_plugin_api($module = NULL, $api = NULL) {
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
 * Implements hook_views_api().
 */
function people_content_type_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}

/**
 * Implements hook_node_info().
 */
function people_content_type_node_info() {
  $items = array(
    'people_list_page' => array(
      'name' => t('People List Page'),
      'base' => 'node_content',
      'description' => t('create a page of Persons in a list, table or grid format, filterable by type and department'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'person' => array(
      'name' => t('Person'),
      'base' => 'node_content',
      'description' => t('use person to create individual staff pages with contact info and bios'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  drupal_alter('node_info', $items);
  return $items;
}
