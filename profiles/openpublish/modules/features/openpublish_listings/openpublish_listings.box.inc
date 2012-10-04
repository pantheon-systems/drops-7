<?php
/**
 * @file
 * openpublish_listings.box.inc
 */

/**
 * Implements hook_default_box().
 */
function openpublish_listings_default_box() {
  $export = array();

  $box = new stdClass();
  $box->disabled = FALSE; /* Edit this to true to make a default box disabled initially */
  $box->api_version = 1;
  $box->delta = 'op_recent_videos';
  $box->plugin_key = 'view';
  $box->title = 'Recent Videos';
  $box->description = 'Open Publish Recent Videos';
  $box->options = array(
    'view' => 'op_recent_content--block_3',
    'settings' => array(
      'limit' => '3',
    ),
    'view_object' => NULL,
    'view_more_text' => '',
    'view_more_link' => '',
    'expose_filters' => 0,
    'additional_classes' => '',
  );
  $export['op_recent_videos'] = $box;

  $box = new stdClass();
  $box->disabled = FALSE; /* Edit this to true to make a default box disabled initially */
  $box->api_version = 1;
  $box->delta = 'op_top_stories';
  $box->plugin_key = 'view';
  $box->title = 'Top Stories';
  $box->description = 'Open Publish Top Stories';
  $box->options = array(
    'view' => 'op_recent_content--block_top_stories',
    'settings' => array(
      'nid' => '4+5',
    ),
    'view_object' => NULL,
    'view_more_text' => '',
    'view_more_link' => '',
    'expose_filters' => 0,
    'additional_classes' => '',
  );
  $export['op_top_stories'] = $box;

  return $export;
}
