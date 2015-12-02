<?php
/**
 * @file
 * webform_block_beans.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function webform_block_beans_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'webform_block';
  $bean_type->label = 'Webform Block';
  $bean_type->options = '';
  $bean_type->description = '';
  $export['webform_block'] = $bean_type;

  return $export;
}
