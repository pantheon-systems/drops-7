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
    'effects' => array(
      1 => array(
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
    'effects' => array(
      1 => array(
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
    'effects' => array(
      1 => array(
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
