<?php
/**
 * @file
 * cu_slider.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function cu_slider_field_group_info() {
  $field_groups = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_slide_image|field_collection_item|field_slider_slide|form';
  $field_group->group_name = 'group_slide_image';
  $field_group->entity_type = 'field_collection_item';
  $field_group->bundle = 'field_slider_slide';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_slide_tabs';
  $field_group->data = array(
    'label' => 'Image',
    'weight' => '1',
    'children' => array(
      0 => 'field_slider_image',
    ),
    'format_type' => 'htab',
    'format_settings' => array(
      'label' => 'Image',
      'instance_settings' => array(
        'required_fields' => 1,
        'classes' => '',
        'description' => '',
      ),
      'formatter' => 'open',
    ),
  );
  $field_groups['group_slide_image|field_collection_item|field_slider_slide|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_slide_link|field_collection_item|field_slider_slide|form';
  $field_group->group_name = 'group_slide_link';
  $field_group->entity_type = 'field_collection_item';
  $field_group->bundle = 'field_slider_slide';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_slide_tabs';
  $field_group->data = array(
    'label' => 'Link',
    'weight' => '3',
    'children' => array(
      0 => 'field_slider_url',
      1 => 'field_slider_link',
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
  $field_groups['group_slide_link|field_collection_item|field_slider_slide|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_slide_tabs|field_collection_item|field_slider_slide|form';
  $field_group->group_name = 'group_slide_tabs';
  $field_group->entity_type = 'field_collection_item';
  $field_group->bundle = 'field_slider_slide';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Slide Content',
    'weight' => '0',
    'children' => array(
      0 => 'group_slide_image',
      1 => 'group_slide_link',
      2 => 'group_slider_slide_content',
    ),
    'format_type' => 'htabs',
    'format_settings' => array(
      'formatter' => '',
      'instance_settings' => array(
        'classes' => '',
      ),
    ),
  );
  $field_groups['group_slide_tabs|field_collection_item|field_slider_slide|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_slider_slide_content|field_collection_item|field_slider_slide|form';
  $field_group->group_name = 'group_slider_slide_content';
  $field_group->entity_type = 'field_collection_item';
  $field_group->bundle = 'field_slider_slide';
  $field_group->mode = 'form';
  $field_group->parent_name = 'group_slide_tabs';
  $field_group->data = array(
    'label' => 'Slide Text',
    'weight' => '2',
    'children' => array(
      0 => 'field_slider_caption',
      1 => 'field_slider_teaser',
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
  $field_groups['group_slider_slide_content|field_collection_item|field_slider_slide|form'] = $field_group;

  // Translatables
  // Included for use with string extractors like potx.
  t('Image');
  t('Link');
  t('Slide Content');
  t('Slide Text');

  return $field_groups;
}
