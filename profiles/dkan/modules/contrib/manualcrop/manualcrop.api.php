<?php
/**
 * @file
 * API documentation for Manual Crop
 */

/**
 * Inform Manual Crop about supported widgets and their settings.
 */
function hook_manualcrop_supported_widgets() {
  return array(
    'image_image' => array('thumblist', 'inline_crop', 'instant_crop'),
  );
}

/**
 * Allows other modules to alter the list of Manual Crop supported widgets
 * and their settings.
 *
 * @see hook_manualcrop_supported_widgets()
 */
function hook_manualcrop_supported_widgets_alter(&$widgets) {
  $widgets['widget_name'] = array('thumblist');
}
