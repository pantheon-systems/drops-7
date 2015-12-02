<?php
/**
 * @file
 * cu_block_section.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function cu_block_section_field_group_info() {
  $export = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_block_section_content|bean|block_section|form';
  $field_group->group_name = 'group_block_section_content';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'block_section';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_block_section_tabs';
  $field_group->data = array(
    'label' => 'Content',
    'weight' => '3',
    'children' => array(
      0 => 'field_blocks_section_blocks',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-block-section-content field-group-htab',
        'required_fields' => 1,
        'id' => '',
      ),
    ),
  );
  $export['group_block_section_content|bean|block_section|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_block_section_design|bean|block_section|form';
  $field_group->group_name = 'group_block_section_design';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'block_section';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_block_section_tabs';
  $field_group->data = array(
    'label' => 'Design',
    'weight' => '5',
    'children' => array(
      0 => 'field_block_section_bg_image',
      1 => 'field_hero_unit_bg_color',
      2 => 'field_hero_unit_text_color',
      3 => 'field_block_section_padding',
      4 => 'field_block_section_content_bg',
      5 => 'field_block_section_tablet_pad',
      6 => 'field_block_section_mobile_pad',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-block-section-design field-group-htab',
        'required_fields' => 1,
        'id' => '',
      ),
    ),
  );
  $export['group_block_section_design|bean|block_section|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_block_section_tabs|bean|block_section|form';
  $field_group->group_name = 'group_block_section_tabs';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'block_section';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Block Section',
    'weight' => '2',
    'children' => array(
      0 => 'group_block_section_content',
      1 => 'group_block_section_design',
    ),
    'format_type' => 'htabs',
    'format_settings' => array(
      'formatter' => '',
      'instance_settings' => array(
        'classes' => 'group-block-section-tabs field-group-htabs',
      ),
    ),
  );
  $export['group_block_section_tabs|bean|block_section|form'] = $field_group;

  return $export;
}
