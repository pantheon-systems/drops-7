<?php
/**
 * @file
 * openpublish_site_page.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function openpublish_site_page_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "context" && $api == "context") {
    return array("version" => "3");
  }
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_node_info().
 */
function openpublish_site_page_node_info() {
  $items = array(
    'openpublish_site_page' => array(
      'name' => t('Site Page'),
      'base' => 'node_content',
      'description' => t('A page is a simple method for creating and displaying information that rarely changes, such as an "About us" section of a website. By default, a page entry does not allow visitor comments and is not featured on the site\'s initial home page.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
