<?php
/**
 * @file
 * cu_feature_callout.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function cu_feature_callout_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'feature_callout';
  $bean_type->label = 'Content Grid';
  $bean_type->options = '';
  $bean_type->description = 'Display important content in a column/grid layout';
  $export['feature_callout'] = $bean_type;

  return $export;
}
