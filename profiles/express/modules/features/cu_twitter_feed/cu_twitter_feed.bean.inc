<?php
/**
 * @file
 * cu_twitter_feed.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function cu_twitter_feed_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'twitter_block';
  $bean_type->label = 'Twitter Block';
  $bean_type->options = '';
  $bean_type->description = 'Display Tweets!';
  $export['twitter_block'] = $bean_type;

  return $export;
}
