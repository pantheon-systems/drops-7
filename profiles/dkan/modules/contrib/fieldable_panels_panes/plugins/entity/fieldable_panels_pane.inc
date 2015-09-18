<?php
/**
 * @file
 * Definition of the taxonomy term plugin.
 */
$plugin = array(
  'handler' => 'FieldablePanelsPaneEntity',
  'entity path' => 'admin/structure/fieldable-panels-panes/view/%fieldable_panels_panes',
  'hooks' => array(
    'menu' => TRUE,
    'admin_paths' => TRUE,
    'permission' => TRUE,
    'panelizer_defaults' => TRUE,
    'form_alter' => TRUE,
    'views_data_alter' => TRUE,
  ),
);
