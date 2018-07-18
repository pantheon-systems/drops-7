<?php
/**
 * @file
 * Definition of the bean panelizer plugin.
 */

$plugin = array(
  'handler' => 'PanelizerEntityBean',
  'entity path' => 'block/%bean_delta',
  'hooks' => array(
    'menu' => TRUE,
    'admin_paths' => TRUE,
    'permission' => TRUE,
    'panelizer_defaults' => TRUE,
    'form_alter' => TRUE,
    'page_alter' => TRUE,
    'views_data_alter' => TRUE,
    'views_plugins_alter' => TRUE,
  ),
);
