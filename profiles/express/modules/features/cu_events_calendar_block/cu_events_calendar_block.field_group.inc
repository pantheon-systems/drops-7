<?php
/**
 * @file
 * cu_events_calendar_block.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function cu_events_calendar_block_field_group_info() {
  $export = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_event_categories|bean|cu_events_calendar_block|form';
  $field_group->group_name = 'group_event_categories';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'cu_events_calendar_block';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_event_tabs';
  $field_group->data = array(
    'label' => 'Categories',
    'weight' => '15',
    'children' => array(
      0 => 'field_event_category',
      1 => 'field_event_subcategories',
      2 => 'field_event_categories',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => '',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_event_categories|bean|cu_events_calendar_block|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_event_date_range|bean|cu_events_calendar_block|form';
  $field_group->group_name = 'group_event_date_range';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'cu_events_calendar_block';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_event_tabs';
  $field_group->data = array(
    'label' => 'Date Range',
    'weight' => '18',
    'children' => array(
      0 => 'field_event_date_range',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => '',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_event_date_range|bean|cu_events_calendar_block|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_event_display|bean|cu_events_calendar_block|form';
  $field_group->group_name = 'group_event_display';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'cu_events_calendar_block';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_event_tabs';
  $field_group->data = array(
    'label' => 'Display',
    'weight' => '14',
    'children' => array(
      0 => 'field_event_expire',
      1 => 'field_event_number',
      2 => 'field_event_template',
      3 => 'field_event_type',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => '',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_event_display|bean|cu_events_calendar_block|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_event_link|bean|cu_events_calendar_block|form';
  $field_group->group_name = 'group_event_link';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'cu_events_calendar_block';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_event_tabs';
  $field_group->data = array(
    'label' => 'Link',
    'weight' => '16',
    'children' => array(
      0 => 'field_event_link',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => '',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_event_link|bean|cu_events_calendar_block|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_event_tabs|bean|cu_events_calendar_block|form';
  $field_group->group_name = 'group_event_tabs';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'cu_events_calendar_block';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Tabs',
    'weight' => '2',
    'children' => array(
      0 => 'group_event_categories',
      1 => 'group_event_date_range',
      2 => 'group_event_display',
      3 => 'group_event_link',
    ),
    'format_type' => 'htabs',
    'format_settings' => array(
      'formatter' => '',
      'instance_settings' => array(
        'classes' => '',
      ),
    ),
  );
  $export['group_event_tabs|bean|cu_events_calendar_block|form'] = $field_group;

  return $export;
}
