<?php
/**
 * @file
 * Contains code to support the Features module.
 */

/**
 * Implements hook_features_export_options().
 */
function video_features_export_options() {
  $options = array();
  foreach (Preset::getAllPresets() as $preset) {
    $options[$preset['name']] = $preset['name'];
  }
  return $options;
}

/**
 * Implements hook_features_export().
 */
function video_features_export($data, &$export, $module_name = '') {
  foreach ($data as $preset) {
    $export['features']['video'][$preset] = $preset;
  }
}

/**
 * Implements hook_features_export_render().
 */
function video_features_export_render($module_name, $data) {
  $items = array();
  foreach ($data as $key) {
    $preset = Preset::getPreset($key);
    unset($preset['pid'], $preset['module'], $preset['overridden']);
    $items[$key] = $preset;
  }
  $code = "  \$items = " . features_var_export($items, '  ') . ";\n";
  $code .= '  return $items;';
  return array('video_default_presets' => $code);
}

/**
 * Implements hook_features_revert().
 */
function video_features_revert($module) {
  if ($default_presets = features_get_default('video', $module)) {
    foreach (array_keys($default_presets) as $default_preset) {
      Preset::deletePreset($default_preset);
    }
  }
}
