<?php
/**
 * @file
 * commerce_kickstart_slideshow.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function commerce_kickstart_slideshow_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function commerce_kickstart_slideshow_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}

/**
 * Implements hook_image_default_styles().
 */
function commerce_kickstart_slideshow_image_default_styles() {
  $styles = array();

  // Exported image style: hp_slideshow.
  $styles['hp_slideshow'] = array(
    'effects' => array(
      2 => array(
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => 940,
          'height' => 480,
        ),
        'weight' => 2,
      ),
    ),
    'label' => 'hp_slideshow',
  );

  return $styles;
}

/**
 * Implements hook_node_info().
 */
function commerce_kickstart_slideshow_node_info() {
  $items = array(
    'slideshow' => array(
      'name' => t('Slideshow'),
      'base' => 'node_content',
      'description' => t('Use <em>slideshow</em> to promote content that will be displayed on the homepage.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => t('Title field is for administrative purpose only and will not be displayed on the site, use Headline instead.'),
    ),
  );
  drupal_alter('node_info', $items);
  return $items;
}
