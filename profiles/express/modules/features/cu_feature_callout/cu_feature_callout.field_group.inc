<?php
/**
 * @file
 * cu_feature_callout.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function cu_feature_callout_field_group_info() {
  $export = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_callout_content|field_collection_item|field_callouts|form';
  $field_group->group_name = 'group_callout_content';
  $field_group->entity_type = 'field_collection_item';
  $field_group->bundle = 'field_callouts';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Callout Content',
    'weight' => '36',
    'children' => array(
      0 => 'group_callout_image',
      1 => 'group_callout_text',
      2 => 'group_callout_title',
    ),
    'format_type' => 'htabs',
    'format_settings' => array(
      'formatter' => '',
      'instance_settings' => array(
        'classes' => '',
      ),
    ),
  );
  $export['group_callout_content|field_collection_item|field_callouts|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_callout_image|field_collection_item|field_callouts|form';
  $field_group->group_name = 'group_callout_image';
  $field_group->entity_type = 'field_collection_item';
  $field_group->bundle = 'field_callouts';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_callout_content';
  $field_group->data = array(
    'label' => 'Image',
    'weight' => '38',
    'children' => array(
      0 => 'field_callout_image',
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
  $export['group_callout_image|field_collection_item|field_callouts|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_callout_text|field_collection_item|field_callouts|form';
  $field_group->group_name = 'group_callout_text';
  $field_group->entity_type = 'field_collection_item';
  $field_group->bundle = 'field_callouts';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_callout_content';
  $field_group->data = array(
    'label' => 'Text',
    'weight' => '37',
    'children' => array(
      0 => 'field_callout_text',
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
  $export['group_callout_text|field_collection_item|field_callouts|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_callout_title|field_collection_item|field_callouts|form';
  $field_group->group_name = 'group_callout_title';
  $field_group->entity_type = 'field_collection_item';
  $field_group->bundle = 'field_callouts';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_callout_content';
  $field_group->data = array(
    'label' => 'Title & Link',
    'weight' => '36',
    'children' => array(
      0 => 'field_callout_title',
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
  $export['group_callout_title|field_collection_item|field_callouts|form'] = $field_group;

  return $export;
}
