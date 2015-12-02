<?php
/**
 * @file
 * cu_hero_unit.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function cu_hero_unit_field_group_info() {
  $export = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_hero_unit_content|bean|hero_unit|form';
  $field_group->group_name = 'group_hero_unit_content';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'hero_unit';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Content',
    'weight' => '2',
    'children' => array(
      0 => 'group_hero_unit_design',
      1 => 'group_hero_unit_text',
    ),
    'format_type' => 'htabs',
    'format_settings' => array(
      'formatter' => '',
      'instance_settings' => array(
        'classes' => '',
      ),
    ),
  );
  $export['group_hero_unit_content|bean|hero_unit|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_hero_unit_design|bean|hero_unit|form';
  $field_group->group_name = 'group_hero_unit_design';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'hero_unit';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_hero_unit_content';
  $field_group->data = array(
    'label' => 'Design',
    'weight' => '4',
    'children' => array(
      0 => 'field_hero_unit_bg_color',
      1 => 'field_hero_unit_image',
      2 => 'field_hero_unit_link_color',
      3 => 'field_hero_unit_size',
      4 => 'field_hero_unit_text_align',
      5 => 'field_hero_unit_text_color',
      6 => 'field_hero_unit_size_priority',
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
  $export['group_hero_unit_design|bean|hero_unit|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_hero_unit_text|bean|hero_unit|form';
  $field_group->group_name = 'group_hero_unit_text';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'hero_unit';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_hero_unit_content';
  $field_group->data = array(
    'label' => 'Text',
    'weight' => '3',
    'children' => array(
      0 => 'field_hero_unit_headline',
      1 => 'field_hero_unit_link',
      2 => 'field_hero_unit_text',
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
  $export['group_hero_unit_text|bean|hero_unit|form'] = $field_group;

  return $export;
}
