<?php
/**
 * @file
 * people_content_type.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function people_content_type_field_group_info() {
  $export = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_contact|node|person|form';
  $field_group->group_name = 'group_contact';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'person';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_information';
  $field_group->data = array(
    'label' => 'Contact',
    'weight' => '23',
    'children' => array(
      0 => 'field_person_address',
      1 => 'field_person_email',
      2 => 'field_person_office_hours',
      3 => 'field_person_phone',
      4 => 'field_person_website',
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
  $export['group_contact|node|person|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_information|node|person|form';
  $field_group->group_name = 'group_information';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'person';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Information',
    'weight' => '6',
    'children' => array(
      0 => 'group_contact',
      1 => 'group_other',
      2 => 'group_personal',
    ),
    'format_type' => 'htabs',
    'format_settings' => array(
      'formatter' => '',
      'instance_settings' => array(
        'classes' => '',
      ),
    ),
  );
  $export['group_information|node|person|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_other|node|person|form';
  $field_group->group_name = 'group_other';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'person';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_information';
  $field_group->data = array(
    'label' => 'Other',
    'weight' => '24',
    'children' => array(
      0 => 'field_person_bio',
      1 => 'field_person_filter_1',
      2 => 'field_person_filter_2',
      3 => 'field_person_filter_3',
      4 => 'field_person_short_bio',
      5 => 'field_extra',
      6 => 'field_person_interests',
      7 => 'body',
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
  $export['group_other|node|person|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_people_filter_display|node|people_list_page|form';
  $field_group->group_name = 'group_people_filter_display';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'people_list_page';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_people_list_filter';
  $field_group->data = array(
    'label' => 'Show/Hide Filters',
    'weight' => '12',
    'children' => array(
      0 => 'field_people_dept_filter_show',
      1 => 'field_people_filter1_show',
      2 => 'field_people_filter2_show',
      3 => 'field_people_filter3_show',
      4 => 'field_people_pos_filter_show',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Show/Hide Filters',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-people-filter-display field-group-fieldset',
        'description' => 'Choose to show what filters are visible on this page.',
      ),
      'formatter' => 'collapsed',
    ),
  );
  $export['group_people_filter_display|node|people_list_page|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_people_list_display|node|people_list_page|form';
  $field_group->group_name = 'group_people_list_display';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'people_list_page';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Display/Format',
    'weight' => '4',
    'children' => array(
      0 => 'field_people_list_display',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Display/Format',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-people-list-display field-group-fieldset',
        'description' => '',
      ),
      'formatter' => 'collapsed',
    ),
  );
  $export['group_people_list_display|node|people_list_page|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_people_list_filter|node|people_list_page|form';
  $field_group->group_name = 'group_people_list_filter';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'people_list_page';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Filter/Group/Order',
    'weight' => '5',
    'children' => array(
      0 => 'field_people_list_department',
      1 => 'field_people_list_person_type',
      2 => 'field_people_filter_1',
      3 => 'field_people_filter_2',
      4 => 'field_people_filter_3',
      5 => 'field_people_group_by',
      6 => 'field_people_order_by',
      7 => 'group_people_filter_display',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Filter/Group/Order',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-people-list-filter field-group-fieldset',
        'description' => 'Customize which people are listed on the page.',
      ),
      'formatter' => 'collapsed',
    ),
  );
  $export['group_people_list_filter|node|people_list_page|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_personal|node|person|form';
  $field_group->group_name = 'group_personal';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'person';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_information';
  $field_group->data = array(
    'label' => 'Personal',
    'weight' => '22',
    'children' => array(
      0 => 'field_person_department',
      1 => 'field_person_first_name',
      2 => 'field_person_job_type',
      3 => 'field_person_last_name',
      4 => 'field_person_photo',
      5 => 'field_person_title',
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
  $export['group_personal|node|person|form'] = $field_group;

  return $export;
}
