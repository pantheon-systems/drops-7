<?php

/**
 * @file
 * Name: focal_point.smartcrop.inc.
 *
 * Adds functionality to leverage the smartcrop module in order to calculate the
 * focal point of an image. This file will only be included if the smartcrop
 * module is currently enabled.
 */

include_once drupal_get_path('module', 'smartcrop') . '/image.gd.inc';

/**
 * Given an image, use smartcrop to estimate a good focal point.
 *
 * @see image_gd_smartcrop_crop()
 * @see http://envalo.com/image-cropping-php-using-entropy-explained/
 */
function focal_point_smartcrop_estimation(stdClass $image_data) {
  // To avoid bad performance with large images, we calculate the focal point on
  // a smaller version of the image. 500px should be far enough.
  // @todo Consider making this configurable.
  static $px_needed = 500;
  $image_data = clone($image_data);
  // If the current image toolkit is ImageMagick, GD resource has to be
  // populated.
  if (image_get_toolkit() != 'gd') {
    image_gd_load($image_data);
  }
  $image_width = $image_data->info['width'];
  $image_height = $image_data->info['height'];
  $ratio = max($px_needed / $image_width, $px_needed / $image_height);
  $resized = FALSE;
  if ($ratio <= 1) {
    $resized = image_gd_resize($image_data, round($ratio * $image_width), round($ratio * $image_height));
  }

  $full_width = imagesx($image_data->resource);
  $full_height = imagesy($image_data->resource);

  // Given that most of the time, cropped images have a width/height ratio
  // between 4:1 and 1:4, the width of a cropped image will be greater than
  // (original height)/4 and the height greater than (original width)/4.
  // Based on this assumption, we will try to find the most interesting area
  // (max entropy) of this size.
  // The focal point will simply be the center of the resulting area.
  // @todo Consider making this configurable.
  static $highest_ratio = 4;
  static $largest_ratio = 4;
  $target_width = round($full_height / $highest_ratio);
  $target_height = round($full_width / $largest_ratio);

  // First, we will cut our image into 25 vertical and horizontal slices, to get
  // 25x25 areas, then calculate their entropy.
  // @todo Consider making this configurable.
  static $slices_count = 25;
  $area_width = $full_width / $slices_count;
  $area_height = $full_height / $slices_count;

  $area_entropies = array();
  for ($i = 0; $i < $slices_count; $i++) {
    $area_entropies[$i] = array();
    for ($j = 0; $j < $slices_count; $j++) {
      $area_entropies[$i][$j] = _smartcrop_gd_entropy_slice($image_data, round($i * $area_width), round($j * $area_height), $area_width, $area_height);
    }
  }

  // We calculate how many areas are needed to get the target width and height.
  $target_areas_counts = array(
    ceil(min($target_width / $area_width, $slices_count)), ceil(min($target_height / $area_height, $slices_count)),
  );

  // We get the adjacent areas that lead to the maximum entropy.
  $best_first_area = _focal_point_smartcrop_get_best_first_area($area_entropies, $target_areas_counts);
  // We calculate the middle of the resulting area.
  $x = $area_width * ($best_first_area[0] + ($target_areas_counts[0] / 2));
  $y = $area_height * ($best_first_area[1] + ($target_areas_counts[1] / 2));

  if ($resized) {
    // We return the center of the resulting area, but for the original image,
    // not the small one.
    $x = $x / $ratio;
    $y = $y / $ratio;
  }
  return array(round($x), round($y));
}

/**
 * Calculate the best area to use based on its entropy.
 */
function _focal_point_smartcrop_get_best_first_area($area_entropies, $target_areas_counts) {
  $slices_count = count($area_entropies);
  $best_entropy = -1;

  for ($i = 0; $i <= $slices_count - $target_areas_counts[0]; $i++) {
    for ($j = 0; $j <= $slices_count - $target_areas_counts[1]; $j++) {
      $first_area = array($i, $j);
      $entropy = _focal_point_smartcrop_sum_entropies($area_entropies, $first_area, $target_areas_counts);
      if ($entropy > $best_entropy) {
        $best_entropy = $entropy;
        $best_first_area = $first_area;
      }
    }
  }

  return $best_first_area;
}

/**
 * Calculate the entropy of several adjacent areas.
 */
function _focal_point_smartcrop_sum_entropies($area_entropies, $first_area, $target_areas_counts) {
  $entropy = 0;
  for ($i = 0; $i < $target_areas_counts[0]; $i++) {
    for ($j = 0; $j < $target_areas_counts[1]; $j++) {
      $entropy += $area_entropies[$first_area[0] + $i][$first_area[1] + $j];
    }
  }
  return $entropy;
}
