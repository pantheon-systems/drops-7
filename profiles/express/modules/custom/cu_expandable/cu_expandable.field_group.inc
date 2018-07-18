<?php
/**
 * @file
 * cu_expandable.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function cu_expandable_field_group_info() {
  $field_groups = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_expandable_block|field_collection_item|field_expandable_section|form';
  $field_group->group_name = 'group_expandable_block';
  $field_group->entity_type = 'field_collection_item';
  $field_group->bundle = 'field_expandable_section';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_expandable_section';
  $field_group->data = array(
    'label' => 'Block',
    'weight' => '7',
    'children' => array(
      0 => 'field_expandable_block',
      1 => 'field_expandable_block_titles',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-expandable-block field-group-htab',
        'required_fields' => 1,
      ),
    ),
  );
  $field_groups['group_expandable_block|field_collection_item|field_expandable_section|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_expandable_content|field_collection_item|field_expandable_section|form';
  $field_group->group_name = 'group_expandable_content';
  $field_group->entity_type = 'field_collection_item';
  $field_group->bundle = 'field_expandable_section';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_expandable_section';
  $field_group->data = array(
    'label' => 'Content',
    'weight' => '6',
    'children' => array(
      0 => 'field_expandable_text',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-expandable-content field-group-htab',
        'required_fields' => 1,
      ),
    ),
  );
  $field_groups['group_expandable_content|field_collection_item|field_expandable_section|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_expandable_label|field_collection_item|field_expandable_section|form';
  $field_group->group_name = 'group_expandable_label';
  $field_group->entity_type = 'field_collection_item';
  $field_group->bundle = 'field_expandable_section';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_expandable_section';
  $field_group->data = array(
    'label' => 'Label',
    'weight' => '5',
    'children' => array(
      0 => 'field_expandable_title',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-expandable-label field-group-htab',
        'required_fields' => 1,
      ),
    ),
  );
  $field_groups['group_expandable_label|field_collection_item|field_expandable_section|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_expandable_options|bean|expandable|form';
  $field_group->group_name = 'group_expandable_options';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'expandable';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Options',
    'weight' => '2',
    'children' => array(
      0 => 'field_expandable_display',
      1 => 'field_expandable_section_open',
      2 => 'field_expandable_select_prompt',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Options',
      'instance_settings' => array(
        'required_fields' => 1,
        'id' => '',
        'classes' => 'group-expandable-options field-group-fieldset',
        'description' => '',
      ),
      'formatter' => 'open',
    ),
  );
  $field_groups['group_expandable_options|bean|expandable|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_expandable_section|field_collection_item|field_expandable_section|form';
  $field_group->group_name = 'group_expandable_section';
  $field_group->entity_type = 'field_collection_item';
  $field_group->bundle = 'field_expandable_section';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Section',
    'weight' => '0',
    'children' => array(
      0 => 'group_expandable_label',
      1 => 'group_expandable_content',
      2 => 'group_expandable_block',
    ),
    'format_type' => 'htabs',
    'format_settings' => array(
      'formatter' => '',
      'instance_settings' => array(
        'classes' => 'group-expandable-section field-group-htabs',
        'id' => '',
      ),
    ),
  );
  $field_groups['group_expandable_section|field_collection_item|field_expandable_section|form'] = $field_group;

  // Translatables
  // Included for use with string extractors like potx.
  t('Block');
  t('Content');
  t('Label');
  t('Options');
  t('Section');

  return $field_groups;
}
