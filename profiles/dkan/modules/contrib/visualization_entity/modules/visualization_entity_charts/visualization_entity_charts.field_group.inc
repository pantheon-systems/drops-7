<?php
/**
 * @file
 * visualization_entity_charts.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function visualization_entity_charts_field_group_info() {
  $field_groups = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_chart_source|visualization|ve_chart|form';
  $field_group->group_name = 'group_chart_source';
  $field_group->entity_type = 'visualization';
  $field_group->bundle = 've_chart';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Chart Source',
    'weight' => '1',
    'children' => array(
      0 => 'group_source_internal',
      1 => 'group_source_upload',
    ),
    'format_type' => 'htabs',
    'format_settings' => array(
      'formatter' => '',
      'instance_settings' => array(
        'classes' => 'group-chart-source field-group-htabs',
        'id' => '',
      ),
    ),
  );
  $field_groups['group_chart_source|visualization|ve_chart|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_source_internal|visualization|ve_chart|form';
  $field_group->group_name = 'group_source_internal';
  $field_group->entity_type = 'visualization';
  $field_group->bundle = 've_chart';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_chart_source';
  $field_group->data = array(
    'label' => 'Internal Data',
    'weight' => '7',
    'children' => array(
      0 => 'field_uuid_resource',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'label' => 'Internal Data',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-source-internal field-group-htab',
        'description' => 'To create a chart using data from a resource that already exists on the site, start typing the title of the resource here. Select the resource from the list that appears.',
      ),
      'formatter' => 'open',
    ),
  );
  $field_groups['group_source_internal|visualization|ve_chart|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_source_upload|visualization|ve_chart|form';
  $field_group->group_name = 'group_source_upload';
  $field_group->entity_type = 'visualization';
  $field_group->bundle = 've_chart';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_chart_source';
  $field_group->data = array(
    'label' => 'Upload Data',
    'weight' => '8',
    'children' => array(
      0 => 'field_file',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'label' => 'Upload Data',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => 'group-source-upload field-group-htab',
        'description' => 'To create a chart from data that does not already exist on the site, use the upload option here.',
      ),
      'formatter' => 'closed',
    ),
  );
  $field_groups['group_source_upload|visualization|ve_chart|form'] = $field_group;

  // Translatables
  // Included for use with string extractors like potx.
  t('Chart Source');
  t('Internal Data');
  t('Upload Data');

  return $field_groups;
}
