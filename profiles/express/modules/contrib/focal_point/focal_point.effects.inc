<?php

/**
 * @file
 * Default image preset.
 */


/**
 * Implements hook_image_effect_info().
 */
function focal_point_image_effect_info() {
  $effects = array(
    'focal_point_scale_and_crop' => array(
      'label' => t('Focal Point Scale And Crop'),
      'help' => t('Scale and crop based on data provided by <em>Focal Point</em>.'),
      'effect callback' => 'focal_point_scale_and_crop_effect',
      'dimensions callback' => 'image_resize_dimensions',
      'form callback' => 'focal_point_crop_form',
      'summary theme' => 'focal_point_image_resize_summary',
    ),
    'focal_point_crop' => array(
      'label' => t('Focal Point Crop'),
      'help' => t('Crop based on data provided by <em>Focal Point</em>.'),
      'effect callback' => 'focal_point_crop_effect',
      'dimensions callback' => 'image_resize_dimensions',
      'form callback' => 'focal_point_crop_form',
      'summary theme' => 'focal_point_image_crop_summary',
    ),
  );

  return $effects;
}

/**
 * Image effect callback.
 */
function focal_point_scale_and_crop_effect(&$image, $data) {
  $resize_data = focal_point_effect_resize_data($image->info['width'], $image->info['height'], $data['width'], $data['height']);
  if (!image_resize_effect($image, $resize_data)) {
    return FALSE;
  }

  if ($crop_data = focal_point_effect_crop_data($image, $data)) {
    // Next we crop.
    return image_crop_effect($image, $crop_data);
  }

  // At worst use the default effect and let Drupal handle the errors that
  // likely exist at this point.
  return image_scale_and_crop_effect($image, $data);
}

/**
 * Form builder for the scale and crop forms.
 */
function focal_point_crop_form($data = array()) {
  $form = image_crop_form($data);
  unset($form['anchor']);
  // Add input fields for focal point shifting.
  $form['focal_point_advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['focal_point_advanced']['shift_x'] = array(
    '#type' => 'textfield',
    '#title' => 'Horizontal Shift',
    '#description' => t('If set images will be cropped as though the focal point indicator has been shifted left (positive values) or shifted right (negative values) by the number of pixels specified.'),
    '#default_value' => (isset($data['focal_point_advanced']['shift_x']) ? $data['focal_point_advanced']['shift_x'] : ''),
    '#field_suffix' => t('pixels'),
    '#size' => 10,
    '#element_validate' => array('image_effect_integer_validate'),
    '#allow_negative' => TRUE,
  );
  $form['focal_point_advanced']['shift_y'] = array(
    '#type' => 'textfield',
    '#title' => 'Veritical Shift',
    '#description' => t('If set images will be cropped as though the focal point indicator has been shifted up (positive values) or shifted down (negative values) by the number of pixels specified.'),
    '#default_value' => (isset($data['focal_point_advanced']['shift_y']) ? $data['focal_point_advanced']['shift_y'] : ''),
    '#field_suffix' => t('pixels'),
    '#size' => 10,
    '#element_validate' => array('image_effect_integer_validate'),
    '#allow_negative' => TRUE,
  );

  return $form;
}

/**
 * Image effect callback.
 */
function focal_point_crop_effect(&$image, $data) {
  if ($crop_data = focal_point_effect_crop_data($image, $data)) {
    return image_crop_effect($image, $crop_data);
  }

  // At worst use the default effect and let Drupal handle the errors that
  // likely exist at this point.
  return image_crop_effect($image, $data);
}

/**
 * Compile the necessary data for the image crop effect.
 *
 * @param object $image
 *   The image object provided to an effect callback.
 * @param array $data
 *   The data array provided to an effect callback.
 *
 * @return array|bool
 *   An array containing the following keys:
 *    - width
 *    - height
 *    - anchor (in the form xoffset-yoffset)
 *   If no image file can be located, FALSE is returned.
 */
function focal_point_effect_crop_data($image, $data) {
  $files = file_load_multiple(array(), array('uri' => $image->source));
  if (count($files)) {
    $file = reset($files);
    $focal_point = $file->focal_point;

    // Special handling for preview images.
    $parameters = drupal_get_query_parameters();
    if (!empty($parameters['focal_point'])) {
      $focal_point = focal_point_validate($parameters['focal_point']) ? $parameters['focal_point'] : $focal_point;
    }

    $focal_point = focal_point_parse($focal_point);
    $crop_data = array(
      'width' => (int) $data['width'],
      'height' => (int) $data['height'],
    );

    // Get the pixel location of the focal point for the current image taking
    // the image boundaries into account.
    $shift_x = (isset($data['focal_point_advanced']) && isset($data['focal_point_advanced']['shift_x'])) ? $data['focal_point_advanced']['shift_x'] : 0;
    $shift_y = (isset($data['focal_point_advanced']) && isset($data['focal_point_advanced']['shift_y'])) ? $data['focal_point_advanced']['shift_y'] : 0;
    $anchor_x = focal_point_effect_calculate_anchor($image->info['width'], $crop_data['width'], $focal_point['x-offset'], $shift_x);
    $anchor_y = focal_point_effect_calculate_anchor($image->info['height'], $crop_data['height'], $focal_point['y-offset'], $shift_y);

    $crop_data['anchor'] = $anchor_x . '-' . $anchor_y;

    return $crop_data;
  }

  return FALSE;
}

/**
 * Calculate how to resize the given image using the given heights and widths.
 *
 * Calculate the resize dimensions of an image based on the longest crop
 * dimension so that the aspect ratio is preserved and that there is always
 * enough image available to the crop.
 *
 * @param int $image_width
 *   The original width of the image.
 * @param int $image_height
 *   The original height of the image.
 * @param int $crop_width
 *   The width the image should be after cropping.
 * @param int $crop_height
 *   The height the image should be after cropping.
 *
 * @return array
 *   An array with the keys width and height representing the final crop size
 *   such that the aspect ratio is preserved.
 */
function focal_point_effect_resize_data($image_width, $image_height, $crop_width, $crop_height) {
  $resize_data = array();

  if ($crop_width > $crop_height) {
    $resize_data['width'] = (int) $crop_width;
    $resize_data['height'] = (int) ($crop_width * $image_height / $image_width);

    // Ensure there is enough area to crop.
    if ($resize_data['height'] < $crop_height) {
      $resize_data['width'] = (int) ($crop_height * $resize_data['width'] / $resize_data['height']);
      $resize_data['height'] = (int) $crop_height;
    }
  }
  else {
    $resize_data['width'] = (int) ($crop_height * $image_width / $image_height);
    $resize_data['height'] = (int) $crop_height;

    // Ensure there is enough area to crop.
    if ($resize_data['width'] < $crop_width) {
      $resize_data['height'] = (int) ($crop_width * $resize_data['height'] / $resize_data['width']);
      $resize_data['width'] = (int) $crop_width;
    }
  }

  return $resize_data;
}

/**
 * Calculate the anchor offset for the given dimension.
 *
 * @param int $image_size
 *   The dimension of the full-sized image.
 * @param int $crop_size
 *   The dimension of the crop.
 * @param int $focal_point_offset
 *   The corresponding focal point percentage value for the given dimension.
 * @param int $focal_point_shift
 *   The pixel value for shifting the focal point for the given dimension.
 *
 * @return int
 *   The anchor offset for the given dimension.
 */
function focal_point_effect_calculate_anchor($image_size, $crop_size, $focal_point_offset, $focal_point_shift = 0) {
  $focal_point_pixel = (int) $focal_point_offset * $image_size / 100;
  $focal_point_pixel -= (int) $focal_point_shift;

  // If the crop size is larger than the image size, use the image size to avoid
  // stretching. This will cause the excess space to be filled with black.
  $crop_size = min($image_size, $crop_size);

  // Define the anchor as half the crop width to the left.
  $offset = (int) ($focal_point_pixel - (.5 * $crop_size));
  // Ensure the anchor doesn't fall off the left edge of the image.
  $offset = max($offset, 0);
  // Ensure the anchor doesn't fall off the right side of the image.
  if ($offset + $crop_size > $image_size) {
    $offset = $image_size - $crop_size;
  }

  return $offset;
}
