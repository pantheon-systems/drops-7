<?php
/**
 * @file
 * photo_gallery.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function photo_gallery_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "context" && $api == "context") {
    return array("version" => "3");
  }
  if ($module == "ds" && $api == "ds") {
    return array("version" => "1");
  }
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function photo_gallery_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}

/**
 * Implements hook_node_info().
 */
function photo_gallery_node_info() {
  $items = array(
    'photo_gallery' => array(
      'name' => t('Photo Gallery'),
      'base' => 'node_content',
      'description' => t('A collection of photos with lightbox functionality'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  drupal_alter('node_info', $items);
  return $items;
}
