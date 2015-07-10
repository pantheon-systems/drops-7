<?php
/**
 * @file
 * commerce_kickstart_lite_product.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function commerce_kickstart_lite_product_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_image_default_styles().
 */
function commerce_kickstart_lite_product_image_default_styles() {
  $styles = array();

  // Exported image style: product_full.
  $styles['product_full'] = array(
    'name' => 'product_full',
    'effects' => array(
      1 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => 400,
          'height' => 550,
        ),
        'weight' => 1,
      ),
    ),
    'label' => 'product_full',
  );

  // Exported image style: product_medium.
  $styles['product_medium'] = array(
    'name' => 'product_medium',
    'effects' => array(
      1 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => 230,
          'height' => 260,
        ),
        'weight' => 1,
      ),
    ),
    'label' => 'product_medium',
  );

  // Exported image style: product_thumbnail.
  $styles['product_thumbnail'] = array(
    'name' => 'product_thumbnail',
    'effects' => array(
      1 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => 110,
          'height' => 130,
        ),
        'weight' => 1,
      ),
    ),
    'label' => 'product_thumbnail',
  );

  return $styles;
}
