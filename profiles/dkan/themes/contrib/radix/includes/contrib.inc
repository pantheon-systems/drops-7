<?php
/**
 * @file
 * Theme and preprocess functions for contrib modules.
 */

/**
 * Preprocess function for Date pager template.
 */
function radix_preprocess_date_views_pager(&$variables) {
  // Render pagers controls as buttons.
  $variables['prev_options']['attributes']['class'][] = 'btn';
  $variables['prev_options']['attributes']['class'][] = 'btn-default';
  $variables['next_options']['attributes']['class'][] = 'btn';
  $variables['next_options']['attributes']['class'][] = 'btn-default';
}

/**
 * Implements hook_libraries_info_alter().
 */
function radix_libraries_info_alter(&$libraries) {
  // Prevent duplicate bootstrap css since it's already
  // compiled into our screen.css from compass_bootstrap via radix theme.
  if (isset($libraries['bootstrap'])) {
    unset($libraries['bootstrap']['files']['css']);
  }
}
