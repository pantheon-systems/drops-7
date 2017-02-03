<?php
/**
 * @file
 * panopoly_images.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function panopoly_images_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "file_entity" && $api == "file_default_displays") {
    return array("version" => "1");
  }
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_image_default_styles().
 */
function panopoly_images_image_default_styles() {
  $styles = array();

  // Exported image style: panopoly_image_featured.
  $styles['panopoly_image_featured'] = array(
    'label' => 'Featured',
    'effects' => array(
      7 => array(
        'name' => 'manualcrop_crop',
        'data' => array(
          'width' => 48,
          'height' => 26,
          'keepproportions' => 1,
          'reuse_crop_style' => 'panopoly_image_full',
          'style_name' => 'panopoly_image_featured',
        ),
        'weight' => 0,
      ),
      5 => array(
        'name' => 'image_scale',
        'data' => array(
          'width' => 480,
          'height' => '',
          'upscale' => 0,
        ),
        'weight' => 1,
      ),
      6 => array(
        'name' => 'image_crop',
        'data' => array(
          'width' => 480,
          'height' => 260,
          'anchor' => 'center-center',
        ),
        'weight' => 3,
      ),
    ),
  );

  // Exported image style: panopoly_image_full.
  $styles['panopoly_image_full'] = array(
    'label' => 'Full width (of the containing region - could upscale small images)',
    'effects' => array(
      2 => array(
        'name' => 'manualcrop_crop',
        'data' => array(
          'width' => 20,
          'height' => 20,
          'keepproportions' => 0,
          'reuse_crop_style' => '',
          'style_name' => 'panopoly_image_full',
        ),
        'weight' => 1,
      ),
      1 => array(
        'name' => 'image_scale',
        'data' => array(
          'width' => 960,
          'height' => '',
          'upscale' => 0,
        ),
        'weight' => 2,
      ),
    ),
  );

  // Exported image style: panopoly_image_half.
  $styles['panopoly_image_half'] = array(
    'label' => 'Half width (of the containing region - could upscale small images)',
    'effects' => array(
      17 => array(
        'name' => 'manualcrop_reuse',
        'data' => array(
          'reuse_crop_style' => 'panopoly_image_full',
        ),
        'weight' => 0,
      ),
      3 => array(
        'name' => 'image_scale',
        'data' => array(
          'width' => 480,
          'height' => '',
          'upscale' => 0,
        ),
        'weight' => 1,
      ),
    ),
  );

  // Exported image style: panopoly_image_original.
  $styles['panopoly_image_original'] = array(
    'label' => 'Original size (after manual crop)',
    'effects' => array(
      1 => array(
        'name' => 'manualcrop_reuse',
        'data' => array(
          'reuse_crop_style' => 'panopoly_image_full',
        ),
        'weight' => 0,
      ),
    ),
  );

  // Exported image style: panopoly_image_quarter.
  $styles['panopoly_image_quarter'] = array(
    'label' => 'Quarter width (of the containing region - could upscale small images)',
    'effects' => array(
      18 => array(
        'name' => 'manualcrop_reuse',
        'data' => array(
          'reuse_crop_style' => 'panopoly_image_full',
        ),
        'weight' => 0,
      ),
      8 => array(
        'name' => 'image_scale',
        'data' => array(
          'width' => 240,
          'height' => '',
          'upscale' => 0,
        ),
        'weight' => 1,
      ),
    ),
  );

  // Exported image style: panopoly_image_spotlight.
  $styles['panopoly_image_spotlight'] = array(
    'label' => 'Spotlight widget',
    'effects' => array(
      11 => array(
        'name' => 'manualcrop_crop',
        'data' => array(
          'width' => 96,
          'height' => 56,
          'keepproportions' => 1,
          'reuse_crop_style' => 'panopoly_image_full',
          'style_name' => 'panopoly_image_spotlight',
        ),
        'weight' => 1,
      ),
      10 => array(
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => 960,
          'height' => 560,
        ),
        'weight' => 2,
      ),
    ),
  );

  // Exported image style: panopoly_image_square.
  $styles['panopoly_image_square'] = array(
    'label' => 'Square',
    'effects' => array(
      13 => array(
        'name' => 'manualcrop_crop',
        'data' => array(
          'width' => 20,
          'height' => 20,
          'keepproportions' => 1,
          'reuse_crop_style' => 'panopoly_image_full',
          'style_name' => 'panopoly_image_square',
        ),
        'weight' => 0,
      ),
      12 => array(
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => 200,
          'height' => 200,
        ),
        'weight' => 1,
      ),
    ),
  );

  // Exported image style: panopoly_image_thumbnail.
  $styles['panopoly_image_thumbnail'] = array(
    'label' => 'Thumbnail',
    'effects' => array(
      15 => array(
        'name' => 'manualcrop_crop',
        'data' => array(
          'width' => 20,
          'height' => 20,
          'keepproportions' => 1,
          'reuse_crop_style' => 'panopoly_image_full',
          'style_name' => 'panopoly_image_thumbnail',
        ),
        'weight' => 0,
      ),
      14 => array(
        'name' => 'image_scale',
        'data' => array(
          'width' => 80,
          'height' => 80,
          'upscale' => 0,
        ),
        'weight' => 1,
      ),
    ),
  );

  // Exported image style: panopoly_image_video.
  $styles['panopoly_image_video'] = array(
    'label' => 'Video widget',
    'effects' => array(
      16 => array(
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => 640,
          'height' => 390,
        ),
        'weight' => 1,
      ),
    ),
  );

  return $styles;
}
