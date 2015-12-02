<?php
/**
 * @file
 * Views plugin handlers.
 *
 * Ripped from Admin Views.
 * http://cgit.drupalcode.org/admin_views
 */

/**
 * Implements hook_views_plugins().
 */
function cu_extended_content_search_views_plugins() {
  $path = drupal_get_path('module', 'cu_extended_content_search');

  $plugins = array(
    'module' => 'views', // This just tells our themes are elsewhere.
    'display' => array(
      'system' => array(
        'title' => t('System'),
        'help' => t('Display the view as replacement for an existing menu path.'),
        'path' => "$path/plugins",
        'handler' => 'views_plugin_display_system',
        'theme' => 'views_view',
        'uses hook menu' => TRUE,
        'use ajax' => TRUE,
        'use pager' => TRUE,
        'accept attachments' => TRUE,
        'admin' => t('System'),
      ),
    ),
    'access' => array(
      'menu' => array(
        'title' => t('Menu system path'),
        'help' => t('Access will be granted based on the original system path.'),
        'handler' => 'views_plugin_access_menu',
        'uses options' => FALSE,
        'path' => "$path/plugins",
      ),
    ),
  );

  return $plugins;
}
