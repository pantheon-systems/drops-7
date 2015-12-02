<?php
/**
 * @file
 * files.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function files_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function files_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}

/**
 * Implements hook_node_info().
 */
function files_node_info() {
  $items = array(
    'file' => array(
      'name' => t('File'),
      'base' => 'node_content',
      'description' => t('Upload files (such as PDFs, Word docs or spreadsheets) which can then be linked to. '),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  drupal_alter('node_info', $items);
  return $items;
}
