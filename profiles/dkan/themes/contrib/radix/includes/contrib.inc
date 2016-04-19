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
 * Helper function to rewrite the bootstrap_library Javascript.
 *
 * Adds a group and weight to put the Bootstrap Javascript after jQuery but
 * before jQuery UI.
 */
function _radix_bootstrap_library_javascript_alter(&$js, $weight) {
  foreach ($js as $js_file => $options) {
    // Handle the case where a single file name was used, rather than giving
    // the file as the key and the options as the value.
    if (!is_array($options)) {
      unset($js[$js_file]);
      $js_file = $options;
      $options = array();
    }

    // Make our changes and overwrite.
    $options['group'] = JS_LIBRARY;
    $options['weight'] = $weight;
    $js[$js_file] = $options;
  }
}

/**
 * Implements hook_libraries_info_alter().
 */
function radix_libraries_info_alter(&$libraries) {
  if (isset($libraries['bootstrap'])) {
    // Prevent duplicate bootstrap css since it's already
    // compiled into our screen.css from compass_bootstrap via radix theme.
    unset($libraries['bootstrap']['files']['css']);

    // Figure out the weight relative to jquery.ui.
    $jquery_ui_library = drupal_get_library('system', 'ui');
    $jquery_ui_js = reset($jquery_ui_library['js']);
    $weight = $jquery_ui_js['weight'] - 1;

    // Adjust the group and weight of Bootstrap's Javascript.
    _radix_bootstrap_library_javascript_alter($libraries['bootstrap']['files']['js'], $weight);
    _radix_bootstrap_library_javascript_alter($libraries['bootstrap']['variants']['js']['files']['js'], $weight);
  }
}
