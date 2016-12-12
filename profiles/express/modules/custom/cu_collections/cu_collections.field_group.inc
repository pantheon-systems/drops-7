<?php
/**
 * @file
 * cu_collections.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function cu_collections_field_group_info() {
  $field_groups = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_collection_body|node|collection_item|form';
  $field_group->group_name = 'group_collection_body';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'collection_item';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_content';
  $field_group->data = array(
    'label' => 'Body',
    'weight' => '12',
    'children' => array(
      0 => 'field_collection_body',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'label' => 'Body',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-collection-body field-group-htab',
        'description' => '',
      ),
      'formatter' => 'open',
    ),
  );
  $field_groups['group_collection_body|node|collection_item|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_collection_grid_display|bean|collection_grid|form';
  $field_group->group_name = 'group_collection_grid_display';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'collection_grid';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_collection_grid_options';
  $field_group->data = array(
    'label' => 'Display',
    'weight' => '12',
    'children' => array(
      0 => 'field_collection_filter_type',
      1 => 'field_collection_display_summary',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-collection-grid-display field-group-htab',
        'required_fields' => 1,
      ),
    ),
  );
  $field_groups['group_collection_grid_display|bean|collection_grid|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_collection_grid_filter|bean|collection_grid|form';
  $field_group->group_name = 'group_collection_grid_filter';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'collection_grid';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_collection_grid_options';
  $field_group->data = array(
    'label' => 'Filter',
    'weight' => '11',
    'children' => array(
      0 => 'field_collection_grid_type',
      1 => 'field_collection_grid_category',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'label' => 'Filter',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-collection-grid-filter field-group-htab',
        'description' => '',
      ),
      'formatter' => 'open',
    ),
  );
  $field_groups['group_collection_grid_filter|bean|collection_grid|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_collection_grid_options|bean|collection_grid|form';
  $field_group->group_name = 'group_collection_grid_options';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'collection_grid';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Settings',
    'weight' => '2',
    'children' => array(
      0 => 'group_collection_grid_filter',
      1 => 'group_collection_grid_display',
    ),
    'format_type' => 'htabs',
    'format_settings' => array(
      'formatter' => '',
      'instance_settings' => array(
        'classes' => 'group-collection-grid-options field-group-htabs',
        'id' => '',
      ),
    ),
  );
  $field_groups['group_collection_grid_options|bean|collection_grid|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_collection_preview|node|collection_item|form';
  $field_group->group_name = 'group_collection_preview';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'collection_item';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_content';
  $field_group->data = array(
    'label' => 'Preview',
    'weight' => '13',
    'children' => array(
      0 => 'field_collection_preview',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-collection-preview field-group-htab',
        'required_fields' => 1,
      ),
    ),
  );
  $field_groups['group_collection_preview|node|collection_item|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_content|node|collection_item|form';
  $field_group->group_name = 'group_content';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'collection_item';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Content',
    'weight' => '1',
    'children' => array(
      0 => 'group_collection_body',
      1 => 'group_collection_preview',
    ),
    'format_type' => 'htabs',
    'format_settings' => array(
      'formatter' => '',
      'instance_settings' => array(
        'classes' => 'group-content field-group-htabs',
        'id' => '',
      ),
    ),
  );
  $field_groups['group_content|node|collection_item|form'] = $field_group;

  // Translatables
  // Included for use with string extractors like potx.
  t('Body');
  t('Content');
  t('Display');
  t('Filter');
  t('Preview');
  t('Settings');

  return $field_groups;
}
