<?php
/**
 * @file
 * Definition of the comment plugin.
 */

if (module_exists('comment')) {
  $plugin = array(
    'handler' => 'PanelizerEntityComment',
    'entity path' => 'comment/%comment',
    'uses page manager' => FALSE,
    'hooks' => array(
      'menu' => TRUE,
      'permission' => TRUE,
      'panelizer_defaults' => TRUE,
      'default_page_manager_handlers' => TRUE,
      'form_alter' => TRUE,
      'views_data_alter' => TRUE,
    ),
  );
}
