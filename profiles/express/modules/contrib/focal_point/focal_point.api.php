<?php

/**
 * @file
 * Documentation of Feeds hooks.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Alter an array of supported widget types.
 *
 * @param array $supported
 *   An array of widget types as strings.
 */
function hook_focal_point_supported_widget_types_alter(&$supported) {
  $supported[] = 'mymodule_my_custom_widget_type';
}

/**
 * Alter an array of supported file entity types.
 *
 * @param array $supported
 *   An array of file types as strings.
 */
function hook_focal_point_supported_file_types_alter(&$supported) {
  $supported[] = 'custom_file_entity_type';
}

/**
 * Provide a default focal point calculation method.
 */
function hook_focal_point_default_method_info() {
  $info['example'] = array(
    'label' => t('Example module'),
    'callback' => 'callback_get_focal_point',
  );
  return $info;
}

/**
 * Alter the default focal point calculation methods.
 *
 * @param array $info
 *   An keyed array of arrays with the keys label and callback.
 */
function hook_focal_point_default_method_info_alter(&$info) {
  $info['example']['callback'] = 'example_get_better_focal_point';
}

/**
 * Hook invoked after saving a Focal_Point element.
 *
 * @param array $element
 *   A keyed array with:
 *     - 'fid' : the Drupal file ID
 *     - 'focal_point' : the string representing the focal point position.
 */
function hook_focal_point_save($element) {
  // Your code here.
}

/**
 * Hook invoked after deleting a Focal_Point record.
 *
 * @param int $fid
 *   The Drupal file ID associated to the deleted Focal point.
 */
function hook_focal_point_delete($fid) {
  // Your code here.
}

/**
 * Alters the Focal Point before saving it to the database.
 *
 * @param string $focal_point
 *   The focal point to be saved (an anchor point in the "[0-100],[0-100]"
 *   format).
 * @param int $fid
 *   The FileID.
 * @param string $original_focal_point
 *   The original focal_point if updating (could be NULL)
 */
function hook_focal_point_pre_save_alter(&$focal_point, $fid, $original_focal_point) {
  // Alter the focal point for FID = 1 if the original focal point is empty.
  if (1 == $fid && empty($original_focal_point)) {
    $focal_point = '10,10';
  }
}

/**
 * @} End of "addtogroup hooks".
 */

/**
 * @addtogroup callbacks
 * @{
 */

/**
 * Callculate an image's focal point.
 *
 * Callback for hook_focal_point_default_method_info().
 *
 * @param object $image
 *   An image resource object from image_get_info().
 *
 * @return array|null
 *   An array of coordinates to use as the focal point on the image. These
 *   values should be in pixels and represent the left and top offset from the
 *   image. For example: array(5, 25) would mean the focal point is 5 pixels
 *   from the left of the image, and 25 pixels from the top of the image.
 */
function callback_get_focal_point($image) {
  // Return a random point on the image.
  return (array(
    mt_rand(0, $image->info['width']),
    mt_rand(0, $image->info['height']),
  ));
}

/**
 * @} End of "addtogroup callbacks".
 */
