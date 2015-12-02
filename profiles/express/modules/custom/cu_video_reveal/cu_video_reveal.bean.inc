<?php
/**
 * @file
 * cu_video_reveal.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function cu_video_reveal_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'video_reveal';
  $bean_type->label = 'Video Reveal';
  $bean_type->options = '';
  $bean_type->description = 'A block containing an image and text that switches to a video when clicked.';
  $export['video_reveal'] = $bean_type;

  return $export;
}
