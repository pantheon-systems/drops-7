<?php

$plugin = array(
  'schema' => 'panelizer_defaults',
  'access' => 'administer panelizer',

  'menu' => array(
    'menu prefix' => 'admin/config/content/panelizer/%panelizer_handler',
    'menu item' => '%',
    'menu title' => 'Panelizer',
    'menu description' => 'Add, edit or delete custom content layouts.',
    'items' => array(),
  ),

  'title singular' => t('panelizer default'),
  'title singular proper' => t('Panelizer default'),
  'title plural' => t('panelizer defaults'),
  'title plural proper' => t('Panelizer defaults'),

  'handler' => array(
    'class' => 'panelizer_defaults_ui',
  ),

  'allowed operations' => array(),
);

// Add our possibly variable operations.
foreach (panelizer_operations() as $path => $operation) {
  $location = isset($operation['ui path']) ? $operation['ui path'] : $path;
  $plugin['menu']['items'][$location] = array('path' => 'list/%ctools_export_ui/' . $path);
  $plugin['allowed operations'][$location] = array('title' => t($operation['menu title']));
}

$plugin['menu']['items']['access'] = array('path' => 'list/%ctools_export_ui/access');
$plugin['allowed operations']['access'] = array('title' => t('Access'));

