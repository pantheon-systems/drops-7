<?php

/**
 * @file
 * Provides WYSIWYG integration for CKEditor.
 */

/**
 * Implements hook_ckeditor_plugin_alter().
 */
function media_wysiwyg_ckeditor_plugin_alter(&$plugins) {
  // Override the default CKEditor Media plugin.
  $plugins['media'] = array(
    'name' => 'media',
    'desc' => t('Plugin for inserting images from Drupal media module'),
    'path' => '%base_path%' . drupal_get_path('module', 'media_wysiwyg') . '/wysiwyg_plugins/media_ckeditor/',
    'buttons' => array(
      'Media' => array(
        'icon' => 'images/icon.gif',
        'label' => 'Add media',
      ),
    ),
    'default' => 'f',
  );
}
