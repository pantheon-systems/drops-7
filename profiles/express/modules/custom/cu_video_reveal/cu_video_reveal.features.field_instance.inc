<?php
/**
 * @file
 * cu_video_reveal.features.field_instance.inc
 */

/**
 * Implements hook_field_default_field_instances().
 */
function cu_video_reveal_field_default_field_instances() {
  $field_instances = array();

  // Exported field_instance: 'bean-video_reveal-field_video_reveal_image'
  $field_instances['bean-video_reveal-field_video_reveal_image'] = array(
    'bundle' => 'video_reveal',
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'image_url_formatter',
        'settings' => array(
          'image_link' => '',
          'image_style' => 'hero',
          'url_type' => 0,
        ),
        'type' => 'image_url',
        'weight' => 1,
      ),
    ),
    'entity_type' => 'bean',
    'fences_wrapper' => 'no_wrapper',
    'field_name' => 'field_video_reveal_image',
    'label' => 'Image',
    'required' => 1,
    'settings' => array(
      'alt_field' => 0,
      'default_image' => 0,
      'file_directory' => '',
      'file_extensions' => 'png gif jpg jpeg',
      'max_filesize' => '',
      'max_resolution' => '',
      'min_resolution' => '',
      'title_field' => 0,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'image',
      'settings' => array(
        'insert' => 0,
        'insert_absolute' => 0,
        'insert_class' => '',
        'insert_default' => 'auto',
        'insert_styles' => array(
          'auto' => 'auto',
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
          'colorbox__small' => 0,
          'colorbox__small_thumbnail' => 0,
          'colorbox__square' => 0,
          'colorbox__square_thumbnail' => 0,
          'colorbox__thumbnail' => 0,
          'icon_link' => 0,
          'image' => 0,
          'image_backstretch' => 0,
          'image_flexslider_full' => 0,
          'image_flexslider_thumbnail' => 0,
          'image_focal_point_preview' => 0,
          'image_grid-3' => 0,
          'image_grid-4' => 0,
          'image_grid-6' => 0,
          'image_grid-9' => 0,
          'image_grid-12' => 0,
          'image_hero' => 0,
          'image_large' => 0,
          'image_large_square_thumbnail' => 0,
          'image_large_wide_thumbnail' => 0,
          'image_medium' => 0,
          'image_preview' => 0,
          'image_slider' => 0,
          'image_slider-large' => 0,
          'image_small' => 0,
          'image_small_thumbnail' => 0,
          'image_square' => 0,
          'image_square_thumbnail' => 0,
          'image_thumbnail' => 0,
          'link' => 0,
        ),
        'insert_width' => '',
        'preview_image_style' => 'thumbnail',
        'progress_indicator' => 'throbber',
      ),
      'type' => 'image_image',
      'weight' => 5,
    ),
  );

  // Exported field_instance: 'bean-video_reveal-field_video_reveal_text'
  $field_instances['bean-video_reveal-field_video_reveal_text'] = array(
    'bundle' => 'video_reveal',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 2,
      ),
    ),
    'entity_type' => 'bean',
    'fences_wrapper' => 'div',
    'field_name' => 'field_video_reveal_text',
    'label' => 'Text',
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
      'weight' => 8,
    ),
  );

  // Exported field_instance: 'bean-video_reveal-field_video_reveal_url'
  $field_instances['bean-video_reveal-field_video_reveal_url'] = array(
    'bundle' => 'video_reveal',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'video_embed_field',
        'settings' => array(
          'description' => 1,
          'description_position' => 'bottom',
          'video_style' => 'normal',
        ),
        'type' => 'video_embed_field',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'bean',
    'fences_wrapper' => 'no_wrapper',
    'field_name' => 'field_video_reveal_url',
    'label' => 'Video',
    'required' => 1,
    'settings' => array(
      'allowed_providers' => array(
        0 => 'youtube',
        1 => 'vimeo',
      ),
      'description_field' => 0,
      'description_length' => 128,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'video_embed_field',
      'settings' => array(),
      'type' => 'video_embed_field_video',
      'weight' => 4,
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Image');
  t('Text');
  t('Video');

  return $field_instances;
}
