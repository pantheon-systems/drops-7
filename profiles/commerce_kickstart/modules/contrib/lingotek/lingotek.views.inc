<?php

/**
 * @file
 * Lingotek Module + Views
 */

/**
 * Implements hook_views_data().
 */
function lingotek_views_data() {
  $data['node']['lingotek_workbench_link'] = array(
    'group' => 'Lingotek',
    'title' => t('Workbench Link'),
    'help' => t('Lingotek Integration'),
    'real field' => 'nid',
    'field' => array(
      'help' => t('Provide a link to the Lingotek Workbench so that a user may translate the node.'),
      'handler' => 'lingotek_views_handler_workbench_link',
    ),
  );
  $data['comments']['lingotek_workbench_link'] = array(
    'group' => 'Lingotek',
    'title' => t('Workbench Link'),
    'help' => t('Lingotek Integration'),
    'real field' => 'nid',
    'field' => array(
      'help' => t('Provide a link to the Lingotek Workbench so that a user may translate the node.'),
      'handler' => 'lingotek_views_handler_workbench_link',
    ),
  );
  return $data;
}