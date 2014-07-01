<?php
/**
 * @file
 * region.vars.php
 */

/**
 * Implements hook_preprocess_region().
 */
function bootstrap_preprocess_region(&$variables) {
  global $theme;

  $region = $variables['region'];
  $classes = &$variables['classes_array'];

  // Content region.
  if ($region === 'content') {
    // @todo is this actually used properly?
    $variables['theme_hook_suggestions'][] = 'region__no_wrapper';
  }
  // Help region.
  elseif ($region === 'help' && !empty($variables['content'])) {
    $variables['content'] = _bootstrap_icon('question-sign') . $variables['content'];
    $classes[] = 'alert';
    $classes[] = 'alert-info';
    $classes[] = 'messages';
    $classes[] = 'info';
  }

  // Support for "well" classes in regions.
  static $wells;
  if (!isset($wells)) {
    foreach (system_region_list($theme) as $name => $title) {
      $wells[$name] = theme_get_setting('bootstrap_region_well-' . $name);
    }
  }
  if (!empty($wells[$region])) {
    $classes[] = $wells[$region];
  }
}
