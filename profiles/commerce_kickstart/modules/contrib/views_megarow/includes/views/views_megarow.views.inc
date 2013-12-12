<?php

/**
 * Implements hook_views_data_alter().
 */
function views_megarow_views_data_alter(&$data) {
  // Expose generally usable entity-related fields.
  foreach (entity_get_info() as $entity_type => $info) {
    $data['views_entity_' . $entity_type]['megarow_links'] = array(
      'title' => t('Megarow links'),
      'help' => t('Outputs links that launch the megarow.'),
      'field' => array(
        'handler' => 'views_handler_field_megarow_links',
      ),
    );
  }
}

/**
 * Implements hook_views_plugins().
 */
function views_megarow_views_plugins() {
  $plugins = array(
    'style' => array(
      'table_megarows' => array(
        'title' => t('Megarow table'),
        'handler' => 'views_plugin_style_table_megarows',
        'theme' => 'views_view_table',
        'theme file' => 'theme.inc',
        'theme path' => drupal_get_path('module', 'views') . '/theme',
        'uses row plugin' => TRUE,
        'uses row class' => TRUE,
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
        'help topic' => 'style-table',
      ),
    ),
  );
  return $plugins;
}
