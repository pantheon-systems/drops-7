<?php
/**
 * @file
 * openpublish_most_popular.box.inc
 */

/**
 * Implements hook_default_box().
 */
function openpublish_most_popular_default_box() {
  $export = array();

  $box = new stdClass;
  $box->disabled = FALSE; /* Edit this to true to make a default box disabled initially */
  $box->api_version = 1;
  $box->delta = 'box-4cc2a9f2';
  $box->plugin_key = 'view';
  $box->title = 'Most Popular';
  $box->description = 'Most Popular';
  $box->options = array(
    'view' => 'op_most_popular--block_1',
    'settings' => array(),
    'view_object' => NULL,
    'view_more_text' => '',
    'view_more_link' => '',
    'expose_filters' => 0,
    'additional_classes' => '',
  );
  $export['box-4cc2a9f2'] = $box;

  return $export;
}
