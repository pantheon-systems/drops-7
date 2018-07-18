<?php
/**
 * @file
 * cu_block_row.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function cu_block_row_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'block_row';
  $bean_type->label = 'Block Row';
  $bean_type->options = '';
  $bean_type->description = 'Display blocks in a single row.';
  $export['block_row'] = $bean_type;

  return $export;
}
