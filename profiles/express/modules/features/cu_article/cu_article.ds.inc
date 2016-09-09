<?php
/**
 * @file
 * cu_article.ds.inc
 */

/**
 * Implements hook_ds_view_modes_info().
 */
function cu_article_ds_view_modes_info() {
  $export = array();

  $ds_view_mode = new stdClass();
  $ds_view_mode->api_version = 1;
  $ds_view_mode->view_mode = 'grid';
  $ds_view_mode->label = 'Grid';
  $ds_view_mode->entities = array(
    'node' => 'node',
  );
  $export['grid'] = $ds_view_mode;

  $ds_view_mode = new stdClass();
  $ds_view_mode->api_version = 1;
  $ds_view_mode->view_mode = 'sidebar_category';
  $ds_view_mode->label = 'Sidebar Category';
  $ds_view_mode->entities = array(
    'node' => 'node',
  );
  $export['sidebar_category'] = $ds_view_mode;

  $ds_view_mode = new stdClass();
  $ds_view_mode->api_version = 1;
  $ds_view_mode->view_mode = 'feature_category';
  $ds_view_mode->label = 'Feature Category';
  $ds_view_mode->entities = array(
    'node' => 'node',
  );
  $export['feature_category'] = $ds_view_mode;

  return $export;
}
