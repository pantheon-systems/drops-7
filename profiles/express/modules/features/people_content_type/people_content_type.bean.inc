<?php
/**
 * @file
 * people_content_type.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function people_content_type_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'people_list_block';
  $bean_type->label = 'People List Block';
  $bean_type->options = '';
  $bean_type->description = 'A configurable block that displays a list of people. This block is a simple display and contains fewer options than the people list page.';
  $export['people_list_block'] = $bean_type;

  return $export;
}
