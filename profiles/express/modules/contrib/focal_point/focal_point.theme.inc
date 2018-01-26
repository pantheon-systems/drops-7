<?php

/**
 * @file
 * Theme functions defined by focal_point.
 */

/**
 * Replacement for the standard theme_image_style function.
 *
 * This function appends an fid and focal_point to the image URL.
 *
 * @param array $variables
 *   This expects the same attributes as theme_image_style with the following
 *   one addition:
 *    - focal_point.
 *
 * @return string
 *   The rendered HTML of the image in question.
 *
 * @see theme_image_style()
 */
function theme_focal_point_image_style($variables) {
  // Determine the dimensions of the styled image.
  $dimensions = array(
    'width' => $variables['width'],
    'height' => $variables['height'],
  );

  image_style_transform_dimensions($variables['style_name'], $dimensions);

  $variables['width'] = $dimensions['width'];
  $variables['height'] = $dimensions['height'];

  // Determine the URL for the styled image.
  $url = image_style_url($variables['style_name'], $variables['path']);
  $delimiter = strpos($url, '?') !== FALSE ? '&' : '?';

  $variables['path'] = $url . $delimiter . 'focal_point=' . $variables['focal_point'];

  return theme('image', $variables);
}

/**
 * Replacement for the standard theme_image_resize_summary function.
 *
 * Returns HTML for a summary of an image resize effect, adding information
 * about a shifted focal point to markup when necessary.
 *
 * @param array $variables
 *   An associative array containing the same attributes as
 *   theme_image_resize_summary,
 *   but with added support for the shift_x and shift_y values.
 *
 * @return string
 *   The rendered summary string for the effects overview page.
 *
 * @see theme_image_resize_summary()
 */
function theme_focal_point_image_resize_summary($variables) {
  $data = $variables['data'];

  // Determine whether to summarize the shift X/Y configuration for this effect.
  $shift_x = isset($variables['data']['focal_point_advanced']['shift_x']) ? (int) $variables['data']['focal_point_advanced']['shift_x'] : '0';
  $shift_y = isset($variables['data']['focal_point_advanced']['shift_y']) ? (int) $variables['data']['focal_point_advanced']['shift_y'] : '0';
  $theme_shift = '';
  if ($shift_x || $shift_y) {
    $args = array(
      '%shift_x' => format_plural($shift_x, '1 pixel', '@count pixels'),
      '%shift_y' => format_plural($shift_y, '1 pixel', '@count pixels'),
    );
    $theme_shift = ' <em>' . t('(shifted %shift_x horizontally and %shift_y vertically)', $args) . '</em>';
  }

  return theme('image_resize_summary', array('data' => $data)) . $theme_shift;
}

/**
 * Replacement for the standard theme_image_crop_summary function.
 *
 * @see theme_image_crop_summary()
 */
function theme_focal_point_image_crop_summary($variables) {
  return theme('focal_point_image_resize_summary', $variables);
}
