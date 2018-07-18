<?php
/**
 * @file
 * cu_block_to_bean.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function cu_block_to_bean_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'block_wrapper';
  $bean_type->label = 'Block Wrapper';
  $bean_type->options = '';
  $bean_type->description = 'Bean wrapper for non-bean blocks.';
  $export['block_wrapper'] = $bean_type;

  return $export;
}
