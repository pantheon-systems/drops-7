<?php
/**
 * @file
 * commerce_kickstart_blog.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function commerce_kickstart_blog_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function commerce_kickstart_blog_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}

/**
 * Implements hook_image_default_styles().
 */
function commerce_kickstart_blog_image_default_styles() {
  $styles = array();

  // Exported image style: blog_full.
  $styles['blog_full'] = array(
    'effects' => array(
      2 => array(
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => 940,
          'height' => 380,
        ),
        'weight' => 1,
      ),
    ),
    'label' => 'blog_full',
  );

  // Exported image style: blog_medium.
  $styles['blog_medium'] = array(
    'effects' => array(
      2 => array(
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => 430,
          'height' => 300,
        ),
        'weight' => 1,
      ),
    ),
    'label' => 'blog_medium',
  );

  return $styles;
}

/**
 * Implements hook_node_info().
 */
function commerce_kickstart_blog_node_info() {
  $items = array(
    'blog_post' => array(
      'name' => t('Blog Post'),
      'base' => 'node_content',
      'description' => t('Use <em>blog post</em> to write regular content.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  drupal_alter('node_info', $items);
  return $items;
}
