<?php

/**
 * @file
 * Export UI plugin definition file.
 */

/**
 * Implements hook_PLUGIN_NAME_ctools_export_ui().
 */
function fieldable_panels_panes_fieldable_panels_pane_ctools_export_ui() {
  $plugin = array(
    'name' => 'fieldable_panels_pane',
    'schema' => 'fieldable_panels_pane_type',
    'access' => 'administer fieldable panels panes',
    'handler' => array('class' => 'fieldable_panels_pane'),

    'title singular' => t('fieldable panels panes type'),
    'title singular proper' => t('Fieldable Panels Panes Type'),
    'title plural' => t('fieldable panels panes types'),
    'title plural proper' => t('Fieldable Panels Panes Types'),
  );

  $plugin['menu'] = array(
    'menu prefix' => 'admin/structure',
    'menu item' => 'fieldable-panels-panes',
    'menu title' => 'Fieldable Panels Panes',
    'menu description' => 'Create and manage fieldable panels pane types.',
  );

  $plugin['allowed operations'] = array(
    'clone' => FALSE,
  );

  $base_path = ctools_export_ui_plugin_base_path($plugin);
  $prefix_count = count(explode('/', $plugin['menu']['menu prefix']));

  $plugin['menu']['items'] = array(
    'list callback' => array('type' => MENU_NORMAL_ITEM),
    'edit callback' => array(
      'path' => '%ctools_export_ui',
      'title callback' => 'fieldable_panels_pane_type_title',
      'title arguments' => array($prefix_count + 1),
      'page arguments' => array($plugin['name'], 'list_entities', $prefix_count + 1),
      'access arguments' => array($plugin['name'], 'list_entities', $prefix_count + 1),
      'type' => MENU_NORMAL_ITEM,
    ),
    'list_entities' => array(
      'path' => '%ctools_export_ui/list',
      'title' => 'List',
      'access callback' => 'ctools_export_ui_task_access',
      'access arguments' => array($plugin['name'], 'list_entities'),
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
    ),
    'add_entity' => array(
      'path' => '%ctools_export_ui/add',
      'title' => 'Add entity',
      'page callback' => 'ctools_export_ui_switcher_page',
      'page arguments' => array($plugin['name'], 'add_entity', $prefix_count + 1),
      'load arguments' => array($plugin['name']),
      'access callback' => 'ctools_export_ui_task_access',
      'access arguments' => array($plugin['name'], 'add_entity'),
      'type' => MENU_LOCAL_TASK,
      'weight' => -9,
    ),
    'edit' => array(
      'path' => '%ctools_export_ui/edit',
      'page arguments' => array($plugin['name'], 'edit', $prefix_count + 1),
      'access arguments' => array($plugin['name'], 'edit', $prefix_count + 1),
      'type' => MENU_LOCAL_TASK,
      'weight' => -8,
    ),
    'export' => array(
      'path' => '%ctools_export_ui/export',
      'page arguments' => array($plugin['name'], 'export', $prefix_count + 1),
      'access arguments' => array($plugin['name'], 'export', $prefix_count + 1),
    ),
    'revert' => array(
      'path' => '%ctools_export_ui/revert',
      'page arguments' => array($plugin['name'], 'delete', $prefix_count + 1),
      'access arguments' => array($plugin['name'], 'delete', $prefix_count + 1),
    ),
    'delete' => array(
      'path' => '%ctools_export_ui/delete',
      'page arguments' => array($plugin['name'], 'delete', $prefix_count + 1),
      'access arguments' => array($plugin['name'], 'delete', $prefix_count + 1),
    ),
    'enable' => array(
      'path' => '%ctools_export_ui/enable',
      'page arguments' => array($plugin['name'], 'enable', $prefix_count + 1),
      'access arguments' => array($plugin['name'], 'enable', $prefix_count + 1),
    ),
    'disable' => array(
      'path' => '%ctools_export_ui/disable',
      'page arguments' => array($plugin['name'], 'disable', $prefix_count + 1),
      'access arguments' => array($plugin['name'], 'disable', $prefix_count + 1),
    ),
  );

  $plugin['redirect'] = array(
    'add_entity' => $base_path . '/%ctools_export_ui',
  );

  $plugin['strings'] = array(
    'title' => array(
      'add_entity' => t('Add fieldable panels pane of type %title'),
      'list_entity' => t('List %title entities'),
    ),
    'confirmation' => array(
      'save_continue' => array(
        'success' => t('%title has been created.'),
        'fail' => t('%title could not be created.'),
      ),
    ),
  );

  return $plugin;
}
