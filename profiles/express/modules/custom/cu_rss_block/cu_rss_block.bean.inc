<?php
/**
 * @file
 * cu_rss_block.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function cu_rss_block_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'rss';
  $bean_type->label = 'RSS';
  $bean_type->options = '';
  $bean_type->description = 'Display content from a RSS feed.';
  $export['rss'] = $bean_type;

  return $export;
}
