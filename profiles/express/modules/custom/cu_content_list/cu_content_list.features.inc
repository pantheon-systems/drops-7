<?php
/**
 * @file
 * cu_content_list.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_content_list_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "bean_admin_ui" && $api == "bean") {
    return array("version" => "5");
  }
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_node_info().
 */
function cu_content_list_node_info() {
  $items = array(
    'content_list_page' => array(
      'name' => t('Content List Page'),
      'base' => 'node_content',
      'description' => t('A list of teasers of other content(articles, pages, people).'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
