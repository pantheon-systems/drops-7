<?php
/**
 * @file
 * openpublish_blog_post.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function openpublish_blog_post_ctools_plugin_api() {
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
function openpublish_blog_post_node_info() {
  $items = array(
    'openpublish_blog_post' => array(
      'name' => t('Blog Post'),
      'base' => 'node_content',
      'description' => t('Used to create a single post to a column or blog.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
