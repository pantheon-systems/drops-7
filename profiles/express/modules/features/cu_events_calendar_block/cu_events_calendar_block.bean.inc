<?php
/**
 * @file
 * cu_events_calendar_block.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function cu_events_calendar_block_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'cu_events_calendar_block';
  $bean_type->label = 'Events Calendar Block';
  $bean_type->options = '';
  $bean_type->description = 'Display events from the CU-Boulder Events Calendar';
  $export['cu_events_calendar_block'] = $bean_type;

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'events_calendar_grid';
  $bean_type->label = 'Events Calendar Grid';
  $bean_type->options = '';
  $bean_type->description = 'Display events from the CU-Boulder Events Calendar in a grid format';
  $export['events_calendar_grid'] = $bean_type;

  return $export;
}
