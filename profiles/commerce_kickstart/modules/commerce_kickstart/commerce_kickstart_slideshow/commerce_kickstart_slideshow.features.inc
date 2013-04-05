<?php
/**
 * @file
 * commerce_kickstart_slideshow.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function commerce_kickstart_slideshow_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function commerce_kickstart_slideshow_views_api() {
  return array("version" => "3.0");
}

/**
 * Implements hook_image_default_styles().
 */
function commerce_kickstart_slideshow_image_default_styles() {
  $styles = array();

  // Exported image style: hp_slideshow.
  $styles['hp_slideshow'] = array(
    'name' => 'hp_slideshow',
    'effects' => array(
      2 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '940',
          'height' => '480',
        ),
        'weight' => '2',
      ),
    ),
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
  return $items;
}
