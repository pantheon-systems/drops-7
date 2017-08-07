<?php
/**
 * @file
 * cu_facebook_activity.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function cu_facebook_activity_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'facebook_activity';
  $bean_type->label = 'Facebook Activity';
  $bean_type->options = '';
  $bean_type->description = 'Display activity of a Facebook page';
  $export['facebook_activity'] = $bean_type;

  return $export;
}
