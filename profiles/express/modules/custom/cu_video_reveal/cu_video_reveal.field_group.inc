<?php
/**
 * @file
 * cu_video_reveal.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function cu_video_reveal_field_group_info() {
  $export = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_video_reveal_image|bean|video_reveal|form';
  $field_group->group_name = 'group_video_reveal_image';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'video_reveal';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_video_reveal_tabs';
  $field_group->data = array(
    'label' => 'Image',
    'weight' => '4',
    'children' => array(
      0 => 'field_video_reveal_image',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-video-reveal-image field-group-htab',
        'required_fields' => 1,
        'id' => '',
      ),
    ),
  );
  $export['group_video_reveal_image|bean|video_reveal|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_video_reveal_tabs|bean|video_reveal|form';
  $field_group->group_name = 'group_video_reveal_tabs';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'video_reveal';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Tabs',
    'weight' => '2',
    'children' => array(
      0 => 'group_video_reveal_video',
      1 => 'group_video_reveal_image',
      2 => 'group_video_reveal_text',
    ),
    'format_type' => 'htabs',
    'format_settings' => array(
      'formatter' => '',
      'instance_settings' => array(
        'classes' => 'group-video-reveal-tabs field-group-htabs',
      ),
    ),
  );
  $export['group_video_reveal_tabs|bean|video_reveal|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_video_reveal_text|bean|video_reveal|form';
  $field_group->group_name = 'group_video_reveal_text';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'video_reveal';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_video_reveal_tabs';
  $field_group->data = array(
    'label' => 'Text',
    'weight' => '6',
    'children' => array(
      0 => 'field_video_reveal_text',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-video-reveal-text field-group-htab',
        'required_fields' => 1,
        'id' => '',
      ),
    ),
  );
  $export['group_video_reveal_text|bean|video_reveal|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_video_reveal_video|bean|video_reveal|form';
  $field_group->group_name = 'group_video_reveal_video';
  $field_group->entity_type = 'bean';
  $field_group->bundle = 'video_reveal';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_video_reveal_tabs';
  $field_group->data = array(
    'label' => 'Video',
    'weight' => '3',
    'children' => array(
      0 => 'field_video_reveal_url',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-video-reveal-video field-group-htab',
        'required_fields' => 1,
        'id' => '',
      ),
    ),
  );
  $export['group_video_reveal_video|bean|video_reveal|form'] = $field_group;

  return $export;
}
