<?php
/**
 * @file
 * cu_block_section.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function cu_block_section_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'block_section';
  $bean_type->label = 'Block Section';
  $bean_type->options = '';
  $bean_type->description = 'Combine other block types and images to create eye catching sections of content';
  $export['block_section'] = $bean_type;

  return $export;
}
