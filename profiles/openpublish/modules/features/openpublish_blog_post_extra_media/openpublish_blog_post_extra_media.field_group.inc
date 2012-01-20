<?php
/**
 * @file
 * openpublish_blog_post_extra_media.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function openpublish_blog_post_extra_media_field_group_info() {
  $export = array();

  $field_group = new stdClass;
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_op_blogpost_media|node|openpublish_blog_post|form';
  $field_group->group_name = 'group_op_blogpost_media';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'openpublish_blog_post';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Media',
    'weight' => '7',
    'children' => array(
      0 => 'field_op_main_image',
      1 => 'field_op_caption',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'label' => 'Media',
      'instance_settings' => array(
        'required_fields' => 0,
        'classes' => '',
        'description' => '',
      ),
      'formatter' => 'collapsible',
    ),
  );
  $export['group_op_blogpost_media|node|openpublish_blog_post|form'] = $field_group;

  return $export;
}
