<?php
/**
 * @file
 * cu_slider.features.field_instance.inc
 */

/**
 * Implements hook_field_default_field_instances().
 */
function cu_slider_field_default_field_instances() {
  $field_instances = array();

  // Exported field_instance: 'bean-slider-field_slider_design_style'
  $field_instances['bean-slider-field_slider_design_style'] = array(
    'bundle' => 'slider',
    'default_value' => array(
      0 => array(
        'value' => 1,
      ),
    ),
    'default_value_function' => '',
    'default_value_php' => '',
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'list',
        'settings' => array(),
        'type' => 'list_key',
        'weight' => 3,
      ),
    ),
    'entity_type' => 'bean',
    'fences_wrapper' => 'no_wrapper',
    'field_name' => 'field_slider_design_style',
    'label' => 'Style',
    'required' => 1,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(),
      'type' => 'options_select',
      'weight' => 3,
    ),
  );

  // Exported field_instance: 'bean-slider-field_slider_size'
  $field_instances['bean-slider-field_slider_size'] = array(
    'bundle' => 'slider',
    'default_value' => array(
      0 => array(
        'value' => 'slider',
      ),
    ),
    'default_value_function' => '',
    'default_value_php' => '',
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'list',
        'settings' => array(),
        'type' => 'list_key',
        'weight' => 2,
      ),
    ),
    'entity_type' => 'bean',
    'fences_wrapper' => 'no_wrapper',
    'field_name' => 'field_slider_size',
    'label' => 'Size',
    'required' => 1,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(),
      'type' => 'options_select',
      'weight' => 2,
    ),
  );

  // Exported field_instance: 'bean-slider-field_slider_slide'
  $field_instances['bean-slider-field_slider_slide'] = array(
    'bundle' => 'slider',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'field_collection',
        'settings' => array(
          'view_mode' => 'full',
        ),
        'type' => 'field_collection_fields',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'bean',
    'fences_wrapper' => 'div',
    'field_name' => 'field_slider_slide',
    'label' => 'Slide',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'field_collection',
      'settings' => array(),
      'type' => 'field_collection_embed',
      'weight' => 4,
    ),
  );

  // Exported field_instance:
  // 'field_collection_item-field_slider_slide-field_slider_caption'
  $field_instances['field_collection_item-field_slider_slide-field_slider_caption'] = array(
    'bundle' => 'field_slider_slide',
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
    'entity_type' => 'field_collection_item',
    'fences_wrapper' => 'div',
    'field_name' => 'field_slider_caption',
    'label' => 'Caption',
    'required' => 0,
    'settings' => array(
      'text_processing' => 0,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'size' => 150,
      ),
      'type' => 'text_textfield',
      'weight' => 3,
    ),
  );

  // Exported field_instance:
  // 'field_collection_item-field_slider_slide-field_slider_image'
  $field_instances['field_collection_item-field_slider_slide-field_slider_image'] = array(
    'bundle' => 'field_slider_slide',
    'deleted' => 0,
    'description' => 'The minimum size of an image is 960 pixels (w) by 360 pixels(h).  If you are going to use a regular sized slider, the minimum size should be 960 pixels (w) by 640 pixels(h).',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'image',
        'settings' => array(
          'image_link' => '',
          'image_style' => '',
        ),
        'type' => 'image',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'field_collection_item',
    'fences_wrapper' => 'div',
    'field_name' => 'field_slider_image',
    'label' => 'Image',
    'required' => 1,
    'settings' => array(
      'alt_field' => 1,
      'default_image' => 0,
      'file_directory' => 'slider',
      'file_extensions' => 'png gif jpg jpeg',
      'max_filesize' => '',
      'max_resolution' => '',
      'min_resolution' => '960x360',
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
        'insert_default' => 'image',
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
        'preview_image_style' => 'preview',
        'progress_indicator' => 'throbber',
      ),
      'type' => 'image_image',
      'weight' => 1,
    ),
  );

  // Exported field_instance:
  // 'field_collection_item-field_slider_slide-field_slider_link'
  $field_instances['field_collection_item-field_slider_slide-field_slider_link'] = array(
    'bundle' => 'field_slider_slide',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'link',
        'settings' => array(),
        'type' => 'link_plain',
        'weight' => 4,
      ),
    ),
    'entity_type' => 'field_collection_item',
    'fences_wrapper' => 'no_wrapper',
    'field_name' => 'field_slider_link',
    'label' => 'Link',
    'required' => 0,
    'settings' => array(
      'absolute_url' => 1,
      'attributes' => array(
        'class' => '',
        'configurable_title' => 0,
        'rel' => '',
        'target' => 'default',
        'title' => '',
      ),
      'display' => array(
        'url_cutoff' => '',
      ),
      'enable_tokens' => 1,
      'rel_remove' => 'default',
      'title' => 'none',
      'title_label_use_field_label' => FALSE,
      'title_maxlength' => 128,
      'title_value' => '',
      'url' => 0,
      'user_register_form' => FALSE,
      'validate_url' => 0,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'advanced_link',
      'settings' => array(
        'default_titles' => '',
        'urls_allowed' => 'both',
        'urls_filter' => '',
        'urls_search' => 'contains',
      ),
      'type' => 'advanced_link',
      'weight' => 31,
    ),
  );

  // Exported field_instance:
  // 'field_collection_item-field_slider_slide-field_slider_teaser'
  $field_instances['field_collection_item-field_slider_slide-field_slider_teaser'] = array(
    'bundle' => 'field_slider_slide',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'This field only displays on wide, and wide with teaser sliders. Too much text will cause your slider to have display issues. Please keep your teaser text to less than 200 characters.',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 2,
      ),
    ),
    'entity_type' => 'field_collection_item',
    'fences_wrapper' => 'div',
    'field_name' => 'field_slider_teaser',
    'label' => 'Teaser',
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
      'weight' => 4,
    ),
  );

  // Exported field_instance:
  // 'field_collection_item-field_slider_slide-field_slider_url'
  $field_instances['field_collection_item-field_slider_slide-field_slider_url'] = array(
    'bundle' => 'field_slider_slide',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 3,
      ),
    ),
    'entity_type' => 'field_collection_item',
    'fences_wrapper' => 'div',
    'field_name' => 'field_slider_url',
    'label' => 'Link',
    'required' => 0,
    'settings' => array(
      'text_processing' => 0,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'size' => 150,
      ),
      'type' => 'text_textfield',
      'weight' => 32,
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Caption');
  t('Image');
  t('Link');
  t('Size');
  t('Slide');
  t('Style');
  t('Teaser');
  t('The minimum size of an image is 960 pixels (w) by 360 pixels(h).  If you are going to use a regular sized slider, the minimum size should be 960 pixels (w) by 640 pixels(h).');
  t('This field only displays on wide, and wide with teaser sliders. Too much text will cause your slider to have display issues. Please keep your teaser text to less than 200 characters.');

  return $field_instances;
}
