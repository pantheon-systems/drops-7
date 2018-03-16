<?php
/**
 * @file
 * Definition of the user plugin.
 */

$plugin = array(
  'handler' => 'PanelizerEntityUser',
  'entity path' => 'user/%user',
  'uses page manager' => TRUE,
  'hooks' => array(
    'menu' => TRUE,
    'admin_paths' => TRUE,
    'permission' => TRUE,
    'panelizer_defaults' => TRUE,
    'default_page_manager_handlers' => TRUE,
    'form_alter' => TRUE,
    'views_data_alter' => TRUE,
  ),
);
