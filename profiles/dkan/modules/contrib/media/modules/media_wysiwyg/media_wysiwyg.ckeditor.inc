<?php

/**
 * @file
 * Provides WYSIWYG integration for CKEditor.
 */

/**
 * Implements hook_ckeditor_plugin().
 */
function media_wysiwyg_ckeditor_plugin() {
  return array(
    'media' => array(
      'name' => 'media',
      'desc' => t('Plugin for inserting images from Drupal media module'),
      'path' => base_path() . drupal_get_path('module', 'media_wysiwyg') . '/wysiwyg_plugins/media_ckeditor/',
      'buttons' => array(
        'Media' => array(
          'icon' => 'images/icon.gif',
          'label' => 'Add media',
        ),
      ),
      'default' => 'f',
    ),
  );
}

/**
 * Implements hook_ckeditor_plugin_alter().
 */
function media_wysiwyg_ckeditor_plugin_alter(&$plugins) {
  // To ensure the ckeditor media integration is done by this module all other
  // integrations are overwritten here. However additionally added keys will be
  // left intact.
  $default_plugin = media_wysiwyg_ckeditor_plugin();
  $plugins['media'] = $default_plugin['media'] + $plugins['media'];
}
