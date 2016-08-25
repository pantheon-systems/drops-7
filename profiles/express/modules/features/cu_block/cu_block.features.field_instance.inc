<?php
/**
 * @file
 * cu_block.features.field_instance.inc
 */

/**
 * Implements hook_field_default_field_instances().
 */
function cu_block_field_default_field_instances() {
  $field_instances = array();

  // Exported field_instance: 'bean-block-field_block_photo'
  $field_instances['bean-block-field_block_photo'] = array(
    'bundle' => 'block',
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'bean',
    'fences_wrapper' => 'div',
    'field_name' => 'field_block_photo',
    'label' => 'Photo',
    'required' => 0,
    'settings' => array(
      'alt_field' => 1,
      'default_image' => 0,
      'file_directory' => 'block',
      'file_extensions' => 'png gif jpg jpeg',
      'max_filesize' => '',
      'max_resolution' => '',
      'min_resolution' => '',
      'title_field' => 1,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'image',
      'settings' => array(
        'insert' => 1,
        'insert_absolute' => 2,
        'insert_class' => '',
        'insert_default' => 'image_medium',
        'insert_styles' => array(
          'auto' => 0,
          'colorbox__backstretch' => 0,
          'colorbox__flexslider_full' => 0,
          'colorbox__flexslider_thumbnail' => 0,
          'colorbox__focal_point_preview' => 0,
          'colorbox__grid-3' => 0,
          'colorbox__grid-4' => 0,
          'colorbox__grid-6' => 0,
          'colorbox__grid-9' => 0,
          'colorbox__grid-12' => 0,
          'colorbox__hero' => 0,
          'colorbox__large' => 0,
          'colorbox__large_square_thumbnail' => 0,
          'colorbox__large_wide_thumbnail' => 0,
          'colorbox__medium' => 0,
          'colorbox__preview' => 0,
          'colorbox__slider' => 0,
          'colorbox__slider-large' => 0,
          'colorbox__small' => 'colorbox__small',
          'colorbox__small_square_thumbnail' => 'colorbox__small_square_thumbnail',
          'colorbox__small_thumbnail' => 'colorbox__small_thumbnail',
          'colorbox__square' => 'colorbox__square',
          'colorbox__square_thumbnail' => 0,
          'colorbox__thumbnail' => 0,
          'icon_link' => 0,
          'image' => 'image',
          'image_backstretch' => 0,
          'image_flexslider_full' => 0,
          'image_flexslider_thumbnail' => 0,
          'image_focal_point_preview' => 0,
          'image_grid-3' => 0,
          'image_grid-4' => 0,
          'image_grid-6' => 0,
          'image_grid-9' => 0,
          'image_grid-12' => 0,
          'image_hero' => 'image_hero',
          'image_large' => 'image_large',
          'image_large_square_thumbnail' => 'image_large_square_thumbnail',
          'image_large_wide_thumbnail' => 'image_large_wide_thumbnail',
          'image_medium' => 'image_medium',
          'image_preview' => 0,
          'image_slider' => 'image_slider',
          'image_slider-large' => 0,
          'image_small' => 'image_small',
          'image_small_square_thumbnail' => 0,
          'image_small_thumbnail' => 0,
          'image_square' => 0,
          'image_square_thumbnail' => 'image_square_thumbnail',
          'image_thumbnail' => 'image_thumbnail',
          'link' => 0,
        ),
        'insert_width' => '',
        'preview_image_style' => 'preview',
        'progress_indicator' => 'throbber',
      ),
      'type' => 'image_image',
      'weight' => 2,
    ),
  );

  // Exported field_instance: 'bean-block-field_block_text'
  $field_instances['bean-block-field_block_text'] = array(
    'bundle' => 'block',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 1,
      ),
    ),
    'entity_type' => 'bean',
    'fences_wrapper' => 'div',
    'field_name' => 'field_block_text',
    'label' => 'Body',
    'required' => 0,
    'settings' => array(
      'text_processing' => 1,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'rows' => 5,
      ),
      'type' => 'text_textarea',
      'weight' => 1,
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Body');
  t('Photo');

  return $field_instances;
}
