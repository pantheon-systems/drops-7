<?php

/**
 * Callback function for express/preview/block-designer-themes and
 * express/preview/block-designer-themes/%
 *
 * Preview Block Designer Themes.
 */
function express_block_designer_themes_preview($id = NULL) {

  $query = db_select('express_block_designer_themes', 'ebdt');
  $query->fields('ebdt', array('id', 'block_theme_name', 'block_settings'));
  $query->orderBy('block_theme_name', 'ASC');
  // If an id was provided, only load that theme.
  if ($id) {
    $query->condition('ebdt.id', $id, '=');
  }
  $results = $query->execute()->fetchAll();
  // Exclude theses settings from being added as css classes.
  $excluded_settings = array(
    'exbd_icon',
    'exbd_heading',
  );
  // Prepare for output.
  $examples = array();
  // Loop through returned themes and build preview output.
  foreach($results as $theme) {
    $exbd_theme_settings = unserialize($theme->block_settings);
    foreach ($exbd_theme_settings as $key => $setting) {
      if (!in_array($key, $excluded_settings)) {
        $prefix = str_replace("_", "-", $key);
        $vars['classes_array'][$prefix] = $prefix . '-' . str_replace("_", "-", $setting);
      }
    }
    // Add additional css classes.
    $vars['classes_array'][] = 'block';
    $vars['classes_array'][] = 'exbd-block';
    // Add variable for block title heading.
    $vars['exbd_heading'] = $exbd_theme_settings['exbd_heading'];
    // Add Block Designer Theme title for block title.
    $vars['block_theme_name'] = $theme->block_theme_name;
    // Icon classes
    $icon_classes = array(
      'fa-star',
      'exbd-icon-color-' . $exbd_theme_settings['exbd_icon_color'],
      'exbd-icon-size-' . $exbd_theme_settings['exbd_icon_size'],
    );
    // Combine classes for output.
    $vars['exbd_classes'] = join(' ', $vars['classes_array']);
    $vars['icon_classes'] = join(' ', $icon_classes);
    // Theme block output.
    $examples[]['#markup'] = theme('express_block_designer_block_preview', $vars);
    $examples['#prefix'] = '<div class="row exbd-theme-previews">';

    $examples['#suffix'] = '</div>';
    // If only a single preview, set the page title to the Block Theme name.
    if ($id) {
      drupal_set_title($theme->block_theme_name . ' Preview');
    }
  }
  return $examples;
}
