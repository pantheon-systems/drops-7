<?php

/**
 * @file
 * Define the WYSIWYG browser plugin.
 */

/**
 * Implements WYSIWYG's hook_INCLUDE_plugin().
 */
function media_wysiwyg_media_plugin() {
  $plugins['media'] = array(
    'title' => variable_get('media_wysiwyg_wysiwyg_title', t('Media browser')),
    'vendor url' => 'http://drupal.org/project/media',
    'icon path' => drupal_get_path('module', 'media_wysiwyg') . '/images',
    'icon file' => 'wysiwyg-media.gif',
    'icon title' => variable_get('media_wysiwyg_wysiwyg_icon_title', t('Add media')),
    // @todo: move this to the plugin directory for the wysiwyg plugin.
    'js path' => drupal_get_path('module', 'media_wysiwyg') . '/js',
    'js file' => 'wysiwyg-media.js',
    'css path' => drupal_get_path('module', 'media_wysiwyg') . '/css',
    'css file' => 'media_wysiwyg.css',
    'settings' => array(
      'global' => array(
        'enabledPlugins' => variable_get('media_wysiwyg_wysiwyg_browser_plugins', array()),
        'file_directory' => variable_get('media_wysiwyg_wysiwyg_upload_directory', ''),
        'types' => variable_get('media_wysiwyg_wysiwyg_allowed_types', array('audio', 'image', 'video', 'document')),
        'id' => 'media_wysiwyg',
      ),
    ),
  );

  return $plugins;
}
