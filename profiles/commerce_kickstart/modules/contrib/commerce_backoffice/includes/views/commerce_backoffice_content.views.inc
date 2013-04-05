<?php

/**
 * @file
 * Views plugin handlers.
 */

/**
 * Implements hook_views_data_alter().
 */
function commerce_backoffice_content_views_data_alter(&$data) {
  // Add the node filter that filters by node types with product reference fields.
  $data['node']['non_product_display_node_type'] = array(
    'title' => t('Non product display content types'),
    'help' => t('Filters by the content type but only shows non product displays content types as options.'),
    'real field' => 'type',
    'filter' => array(
      'handler' => 'commerce_backoffice_content_filter_node_type',
    ),
  );
  $data['views_entity_node']['operations_dropbutton'] = array(
    'field' => array(
      'title' => t('Operations links (Dropbutton)'),
      'help' => t('Display the available operations links for the node in a dropbutton.'),
      'handler' => 'commerce_backoffice_handler_field_node_operations',
    ),
  );
  $data['node']['term_node_tid_multiple'] = array(
    'title' => 'Has taxonomy terms (Multiple)',
    'help' => 'Display content if it has one of (or all of) the selected terms.',
    'filter' => array(
      'handler' => 'commerce_backoffice_handler_filter_term_node_tid',
      'product display' => FALSE,
    ),
  );
}

/**
 * Implements hook_views_plugins().
 */
function commerce_backoffice_content_views_plugins() {
  $path = drupal_get_path('module', 'commerce_backoffice') . 'includes/views';
  $plugins = array(
    'module' => 'views', // This just tells our themes are elsewhere.
    'display' => array(
      'system' => array(
        'title' => t('System'),
        'help' => t('Display the view as replacement for an existing menu path.'),
        'path' => "$path/plugins",
        'handler' => 'commerce_backoffice_content_plugin_display_system',
        'theme' => 'views_view',
        'uses hook menu' => TRUE,
        'use ajax' => TRUE,
        'use pager' => TRUE,
        'accept attachments' => TRUE,
        'admin' => t('System'),
      ),
    ),
  );
  return $plugins;
}
