<?php
/**
 * @file
 * Views-specific implementations and functions.
 */

/**
 * Implements hook_views_plugins().
 */
function views_responsive_grid_views_plugins() {
  return array(
    'style' => array(
      'responsive_grid' => array(
        'title' => t('Responsive grid'),
        'help' => t('Display content in a responsive grid.'),
        'handler' => 'views_responsive_grid_plugin_style_responsive_grid',
        'theme' => 'views_view_responsive_grid',
        'uses row plugin' => TRUE,
        'uses row class' => FALSE,
        'uses options' => TRUE,
        'uses grouping' => TRUE,
        'type' => 'normal',
      ),
    ),
  );
}
