<?php
/**
 * @file
 * panopoly_images.features.defaultconfig.inc
 */

/**
 * Implements hook_defaultconfig_features().
 */
function panopoly_images_defaultconfig_features() {
  return array(
    'panopoly_images' => array(
      'strongarm' => 'strongarm',
    ),
  );
}

/**
 * Implements hook_defaultconfig_strongarm().
 */
function panopoly_images_defaultconfig_strongarm() {
  $export = array();

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'focal_point_enabled_for';
  $strongarm->value = array(
    'image' => 'image',
    'media' => 0,
  );
  $export['focal_point_enabled_for'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'manualcrop_file_entity_settings_image';
  $strongarm->value = array(
    'manualcrop_enable' => 1,
    'manualcrop_keyboard' => 1,
    'manualcrop_thumblist' => 0,
    'manualcrop_inline_crop' => 0,
    'manualcrop_crop_info' => 1,
    'manualcrop_instant_preview' => 1,
    'manualcrop_instant_crop' => 0,
    'manualcrop_default_crop_area' => 0,
    'manualcrop_maximize_default_crop_area' => 0,
    'manualcrop_styles_mode' => 'include',
    'manualcrop_styles_list' => array(
      'panopoly_image_full' => 'panopoly_image_full',
    ),
    'manualcrop_require_cropping' => array(),
  );
  $export['manualcrop_file_entity_settings_image'] = $strongarm;

  return $export;
}
