<?php
/**
 * @file
 * cu_view_modes.ds.inc
 */

/**
 * Implements hook_ds_view_modes_info().
 */
function cu_view_modes_ds_view_modes_info() {
  $export = array();

  $ds_view_mode = new stdClass();
  $ds_view_mode->api_version = 1;
  $ds_view_mode->view_mode = 'email_feature';
  $ds_view_mode->label = 'Email Feature';
  $ds_view_mode->entities = array(
    'node' => 'node',
  );
  $export['email_feature'] = $ds_view_mode;

  $ds_view_mode = new stdClass();
  $ds_view_mode->api_version = 1;
  $ds_view_mode->view_mode = 'email_teaser';
  $ds_view_mode->label = 'Email Teaser';
  $ds_view_mode->entities = array(
    'node' => 'node',
  );
  $export['email_teaser'] = $ds_view_mode;

  $ds_view_mode = new stdClass();
  $ds_view_mode->api_version = 1;
  $ds_view_mode->view_mode = 'embed';
  $ds_view_mode->label = 'Embed';
  $ds_view_mode->entities = array(
    'node' => 'node',
  );
  $export['embed'] = $ds_view_mode;

  $ds_view_mode = new stdClass();
  $ds_view_mode->api_version = 1;
  $ds_view_mode->view_mode = 'feature';
  $ds_view_mode->label = 'Feature';
  $ds_view_mode->entities = array(
    'node' => 'node',
  );
  $export['feature'] = $ds_view_mode;

  $ds_view_mode = new stdClass();
  $ds_view_mode->api_version = 1;
  $ds_view_mode->view_mode = 'promo';
  $ds_view_mode->label = 'Promo';
  $ds_view_mode->entities = array(
    'node' => 'node',
  );
  $export['promo'] = $ds_view_mode;

  $ds_view_mode = new stdClass();
  $ds_view_mode->api_version = 1;
  $ds_view_mode->view_mode = 'sidebar';
  $ds_view_mode->label = 'Title & Thumbnail';
  $ds_view_mode->entities = array(
    'node' => 'node',
  );
  $export['sidebar'] = $ds_view_mode;

  $ds_view_mode = new stdClass();
  $ds_view_mode->api_version = 1;
  $ds_view_mode->view_mode = 'table';
  $ds_view_mode->label = 'Table';
  $ds_view_mode->entities = array(
    'node' => 'node',
  );
  $export['table'] = $ds_view_mode;

  $ds_view_mode = new stdClass();
  $ds_view_mode->api_version = 1;
  $ds_view_mode->view_mode = 'title';
  $ds_view_mode->label = 'Title';
  $ds_view_mode->entities = array(
    'node' => 'node',
  );
  $export['title'] = $ds_view_mode;

  return $export;
}
