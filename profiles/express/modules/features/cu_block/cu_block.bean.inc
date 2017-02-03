<?php
/**
 * @file
 * cu_block.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function cu_block_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'block';
  $bean_type->label = 'Text Block';
  $bean_type->options = '';
  $bean_type->description = 'A regular block that can contain text and images.';
  $export['block'] = $bean_type;

  return $export;
}
