<?php
/**
 * @file
 * cu_newsletter_list.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function cu_newsletter_list_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'newsletter_list';
  $bean_type->label = 'Newsletter List';
  $bean_type->options = '';
  $bean_type->description = 'Display a list of latest issues of a newsletter.';
  $export['newsletter_list'] = $bean_type;

  return $export;
}
