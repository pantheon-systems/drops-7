<?php

/**
 * @file
 */

/**
 * Implementation of hook_views_data()
 */
function views_node_taxonomy_filter_views_data() {

  $data = array();

  $data['views_node_taxonomy_filter']['table']['group']  = t('Taxonomy Filter');
  $data['views_node_taxonomy_filter']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'tid',
    ),
  );

  // nid field
  $data['views_node_taxonomy_filter']['nid'] = array(
    'title' => t('Node Taxonomy Filter'),
    'help' => t('Filter Views by a taxonomy term applied to a node'),
    'filter' => array(
      'handler' => 'views_node_taxonomy_filter_handler_filter_tid',
    ),
  );

  return $data;
}