<?php
/**
 * @file
 * cu_expandable.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function cu_expandable_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'expandable';
  $bean_type->label = 'Expandable';
  $bean_type->options = '';
  $bean_type->description = 'Format content as Tabs or Accordions.';
  $export['expandable'] = $bean_type;

  return $export;
}
