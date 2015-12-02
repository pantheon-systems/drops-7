<?php
/**
 * @file
 * quicktab_beans.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function quicktab_beans_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'quicktab';
  $bean_type->label = 'Quicktab';
  $bean_type->options = '';
  $bean_type->description = 'Display a set of blocks as tabs';
  $export['quicktab'] = $bean_type;

  return $export;
}
