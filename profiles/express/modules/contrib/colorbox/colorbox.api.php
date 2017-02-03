<?php

/**
 * @file
 * API documentation for the colorbox module.
 */

/**
 * Allows to override Colorbox settings and style.
 *
 * Implements hook_colorbox_settings_alter().
 *
 * @param $settings
 *   An associative array of Colorbox settings. See the
 *   @link http://colorpowered.com/colorbox/ Colorbox documentation @endlink
 *   for the full list of supported parameters.
 * @param $style
 *   The name of the active style plugin. If $style is 'none', no Colorbox
 *   theme will be loaded.
 */
function hook_colorbox_settings_alter(&$settings, &$style) {
  // Disable automatic downscaling of images to maxWidth/maxHeight size.
  $settings['scalePhotos'] = FALSE;

  // Use custom style plugin specifically for node/123.
  if ($_GET['q'] == 'node/123') {
    $style = 'mystyle';
  }
}

/**
 * Allows to override activation of Colobox for the current URL.
 *
 * @param $active
 *   A boolean indicating whether colorbox should be active for the current
 *   URL or not.
 */
function hook_colorbox_active_alter(&$active) {
  $path = drupal_get_path_alias($_GET['q']);
  if (drupal_match_path($path, 'admin/config/colorbox_test')) {
    // Enable colorbox for this URL.
    $active = TRUE;
  }
}
