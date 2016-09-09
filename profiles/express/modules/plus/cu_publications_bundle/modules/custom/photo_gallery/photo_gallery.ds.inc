<?php
/**
 * @file
 * photo_gallery.ds.inc
 */

/**
 * Implements hook_ds_field_settings_info().
 */
function photo_gallery_ds_field_settings_info() {
  $export = array();

  $ds_fieldsetting = new stdClass();
  $ds_fieldsetting->api_version = 1;
  $ds_fieldsetting->id = 'node|photo_gallery|single_page';
  $ds_fieldsetting->entity_type = 'node';
  $ds_fieldsetting->bundle = 'photo_gallery';
  $ds_fieldsetting->view_mode = 'single_page';
  $ds_fieldsetting->settings = array(
    'title' => array(
      'weight' => '0',
      'label' => 'hidden',
      'format' => 'default',
      'formatter_settings' => array(
        'link' => '0',
        'wrapper' => 'h2',
        'class' => '',
      ),
    ),
  );
  $export['node|photo_gallery|single_page'] = $ds_fieldsetting;

  return $export;
}

/**
 * Implements hook_ds_layout_settings_info().
 */
function photo_gallery_ds_layout_settings_info() {
  $export = array();

  $ds_layout = new stdClass();
  $ds_layout->api_version = 1;
  $ds_layout->id = 'node|photo_gallery|single_page';
  $ds_layout->entity_type = 'node';
  $ds_layout->bundle = 'photo_gallery';
  $ds_layout->view_mode = 'single_page';
  $ds_layout->layout = 'ds_1col';
  $ds_layout->settings = array(
    'hide_empty_regions' => 0,
    'hide_sidebars' => 0,
    'regions' => array(
      'ds_content' => array(
        0 => 'title',
        1 => 'body',
        2 => 'photo_gallery_entity_view_1',
      ),
    ),
    'fields' => array(
      'title' => 'ds_content',
      'body' => 'ds_content',
      'photo_gallery_entity_view_1' => 'ds_content',
    ),
    'classes' => array(),
  );
  $export['node|photo_gallery|single_page'] = $ds_layout;

  return $export;
}
