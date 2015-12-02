<?php
/**
 * @file
 * cu_facebook_like_button.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function cu_facebook_like_button_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'facebook_like_button';
  $bean_type->label = 'Facebook Like Button';
  $bean_type->options = '';
  $bean_type->description = 'Add a Facebook Like button';
  $export['facebook_like_button'] = $bean_type;

  return $export;
}
