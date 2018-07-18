<?php
/**
 * @file
 * cu_content_list.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function cu_content_list_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'content_list';
  $bean_type->label = 'Content List';
  $bean_type->options = '';
  $bean_type->description = 'Create a custom list of other content on your site';
  $export['content_list'] = $bean_type;

  return $export;
}
