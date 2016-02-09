<?php

$field_instances = array();

// Exported field_instance: 'layout-block_layout-field_feature_layout_toggle'
$field_instances['layout-block_layout-field_feature_layout_toggle'] = array(
  'bundle' => 'express_layout',
  'default_value' => array(
    0 => array(
      'value' => 'default_layout',
    ),
  ),
  'default_value_function' => '',
  'default_value_php' => '',
  'deleted' => 0,
  'description' => 'Choosing the Feature Layout option removes the sidebars and post wide regions to focus on the page content.',
  'display' => array(
    'default' => array(
      'label' => 'above',
      'module' => 'list',
      'settings' => array(),
      'type' => 'list_default',
      'weight' => 11,
    ),
    'teaser' => array(
      'label' => 'above',
      'settings' => array(),
      'type' => 'hidden',
      'weight' => 0,
    ),
  ),
  'entity_type' => 'express_layout',
  'fences_wrapper' => 'no_wrapper',
  'field_name' => 'field_feature_layout_toggle',
  'label' => 'Feature Layout',
  'required' => 1,
  'settings' => array(
    'user_register_form' => FALSE,
  ),
  'widget' => array(
    'active' => 1,
    'module' => 'options',
    'settings' => array(),
    'type' => 'options_select',
    'weight' => 32,
  ),
);

// Exported field_instance: 'layout-block_layout-field_feature_title_image'
$field_instances['layout-block_layout-field_feature_title_image'] = array(
  'bundle' => 'express_layout',
  'deleted' => 0,
  'description' => 'Add a large image above the page title. Using this feature will remove any content in the intro wide region.',
  'display' => array(
    'default' => array(
      'label' => 'above',
      'module' => 'image_url_formatter',
      'settings' => array(
        'image_link' => '',
        'image_style' => 'hero',
        'url_type' => 1,
      ),
      'type' => 'image_url',
      'weight' => 10,
    ),
    'teaser' => array(
      'label' => 'above',
      'settings' => array(),
      'type' => 'hidden',
      'weight' => 0,
    ),
  ),
  'entity_type' => 'express_layout',
  'fences_wrapper' => 'no_wrapper',
  'field_name' => 'field_feature_title_image',
  'label' => 'Title Image',
  'required' => 0,
  'settings' => array(
    'alt_field' => 1,
    'default_image' => 0,
    'file_directory' => 'feature-title-image',
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
        'colorbox__small_square' => 0,
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
        'image_small_square' => 0,
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
    'weight' => 31,
  ),
);

// Exported field_instance: 'layout-block_layout-field_footer'
$field_instances['layout-block_layout-field_footer'] = array(
  'bundle' => 'express_layout',
  'deleted' => 0,
  'description' => '',
  'display' => array(
    'default' => array(
      'label' => 'hidden',
      'module' => 'entityreference',
      'settings' => array(
        'link' => FALSE,
      ),
      'type' => 'entityreference_label',
      'weight' => 0,
    ),
    'teaser' => array(
      'label' => 'above',
      'settings' => array(),
      'type' => 'hidden',
      'weight' => 0,
    ),
  ),
  'entity_type' => 'express_layout',
  'fences_wrapper' => 'no_wrapper',
  'field_name' => 'field_footer',
  'label' => 'Below Content',
  'required' => 0,
  'settings' => array(
    'user_register_form' => FALSE,
  ),
  'widget' => array(
    'active' => 1,
    'module' => 'inline_entity_form',
    'settings' => array(
      'fields' => array(),
      'type_settings' => array(
        'allow_existing' => 1,
        'allow_new' => 1,
        'delete_references' => 0,
        'label_plural' => 'entities',
        'label_singular' => 'entity',
        'match_operator' => 'CONTAINS',
        'override_labels' => 0,
      ),
    ),
    'type' => 'inline_entity_form',
    'weight' => 11,
  ),
);

// Exported field_instance: 'layout-block_layout-field_header'
$field_instances['layout-block_layout-field_header'] = array(
  'bundle' => 'express_layout',
  'deleted' => 0,
  'description' => '',
  'display' => array(
    'default' => array(
      'label' => 'hidden',
      'module' => 'entityreference',
      'settings' => array(
        'link' => FALSE,
      ),
      'type' => 'entityreference_label',
      'weight' => 1,
    ),
    'teaser' => array(
      'label' => 'above',
      'settings' => array(),
      'type' => 'hidden',
      'weight' => 0,
    ),
  ),
  'entity_type' => 'express_layout',
  'fences_wrapper' => 'no_wrapper',
  'field_name' => 'field_header',
  'label' => 'Above Content',
  'required' => 0,
  'settings' => array(
    'user_register_form' => FALSE,
  ),
  'widget' => array(
    'active' => 1,
    'module' => 'inline_entity_form',
    'settings' => array(
      'fields' => array(),
      'type_settings' => array(
        'allow_existing' => 1,
        'allow_new' => 1,
        'delete_references' => 0,
        'label_plural' => 'entities',
        'label_singular' => 'entity',
        'match_operator' => 'CONTAINS',
        'override_labels' => 0,
      ),
    ),
    'type' => 'inline_entity_form',
    'weight' => 9,
  ),
);

// Exported field_instance: 'layout-block_layout-field_inner_content_left'
$field_instances['layout-block_layout-field_inner_content_left'] = array(
  'bundle' => 'express_layout',
  'deleted' => 0,
  'description' => '',
  'display' => array(
    'default' => array(
      'label' => 'hidden',
      'module' => 'entityreference',
      'settings' => array(
        'link' => FALSE,
      ),
      'type' => 'entityreference_label',
      'weight' => 2,
    ),
    'teaser' => array(
      'label' => 'above',
      'settings' => array(),
      'type' => 'hidden',
      'weight' => 0,
    ),
  ),
  'entity_type' => 'express_layout',
  'fences_wrapper' => 'no_wrapper',
  'field_name' => 'field_inner_content_left',
  'label' => 'Inner Left',
  'required' => 0,
  'settings' => array(
    'user_register_form' => FALSE,
  ),
  'widget' => array(
    'active' => 1,
    'module' => 'inline_entity_form',
    'settings' => array(
      'fields' => array(),
      'type_settings' => array(
        'allow_existing' => 1,
        'allow_new' => 1,
        'delete_references' => 0,
        'label_plural' => 'entities',
        'label_singular' => 'entity',
        'match_operator' => 'CONTAINS',
        'override_labels' => 0,
      ),
    ),
    'type' => 'inline_entity_form',
    'weight' => 13,
  ),
);

// Exported field_instance: 'layout-block_layout-field_inner_content_right'
$field_instances['layout-block_layout-field_inner_content_right'] = array(
  'bundle' => 'express_layout',
  'deleted' => 0,
  'description' => '',
  'display' => array(
    'default' => array(
      'label' => 'hidden',
      'module' => 'entityreference',
      'settings' => array(
        'link' => FALSE,
      ),
      'type' => 'entityreference_label',
      'weight' => 3,
    ),
    'teaser' => array(
      'label' => 'above',
      'settings' => array(),
      'type' => 'hidden',
      'weight' => 0,
    ),
  ),
  'entity_type' => 'express_layout',
  'fences_wrapper' => 'no_wrapper',
  'field_name' => 'field_inner_content_right',
  'label' => 'Inner Right',
  'required' => 0,
  'settings' => array(
    'user_register_form' => FALSE,
  ),
  'widget' => array(
    'active' => 1,
    'module' => 'inline_entity_form',
    'settings' => array(
      'fields' => array(),
      'type_settings' => array(
        'allow_existing' => 1,
        'allow_new' => 1,
        'delete_references' => 0,
        'label_plural' => 'entities',
        'label_singular' => 'entity',
        'match_operator' => 'CONTAINS',
        'override_labels' => 0,
      ),
    ),
    'type' => 'inline_entity_form',
    'weight' => 15,
  ),
);

// Exported field_instance: 'layout-block_layout-field_intro'
$field_instances['layout-block_layout-field_intro'] = array(
  'bundle' => 'express_layout',
  'deleted' => 0,
  'description' => '',
  'display' => array(
    'default' => array(
      'label' => 'hidden',
      'module' => 'entityreference',
      'settings' => array(
        'link' => FALSE,
      ),
      'type' => 'entityreference_label',
      'weight' => 4,
    ),
    'teaser' => array(
      'label' => 'above',
      'settings' => array(),
      'type' => 'hidden',
      'weight' => 0,
    ),
  ),
  'entity_type' => 'express_layout',
  'fences_wrapper' => 'no_wrapper',
  'field_name' => 'field_intro',
  'label' => 'Intro Wide',
  'required' => 0,
  'settings' => array(
    'user_register_form' => FALSE,
  ),
  'widget' => array(
    'active' => 1,
    'module' => 'inline_entity_form',
    'settings' => array(
      'fields' => array(),
      'type_settings' => array(
        'allow_existing' => 1,
        'allow_new' => 1,
        'delete_references' => 0,
        'label_plural' => 'entities',
        'label_singular' => 'entity',
        'match_operator' => 'CONTAINS',
        'override_labels' => 0,
      ),
    ),
    'type' => 'inline_entity_form',
    'weight' => 1,
  ),
);

// Exported field_instance: 'layout-block_layout-field_sidebar_first'
$field_instances['layout-block_layout-field_sidebar_first'] = array(
  'bundle' => 'express_layout',
  'deleted' => 0,
  'description' => '',
  'display' => array(
    'default' => array(
      'label' => 'hidden',
      'module' => 'entityreference',
      'settings' => array(
        'link' => FALSE,
      ),
      'type' => 'entityreference_label',
      'weight' => 6,
    ),
    'teaser' => array(
      'label' => 'above',
      'settings' => array(),
      'type' => 'hidden',
      'weight' => 0,
    ),
  ),
  'entity_type' => 'express_layout',
  'fences_wrapper' => 'no_wrapper',
  'field_name' => 'field_sidebar_first',
  'label' => 'Left Sidebar',
  'required' => 0,
  'settings' => array(
    'user_register_form' => FALSE,
  ),
  'widget' => array(
    'active' => 1,
    'module' => 'inline_entity_form',
    'settings' => array(
      'fields' => array(),
      'type_settings' => array(
        'allow_existing' => 1,
        'allow_new' => 1,
        'delete_references' => 0,
        'label_plural' => 'entities',
        'label_singular' => 'entity',
        'match_operator' => 'CONTAINS',
        'override_labels' => 0,
      ),
    ),
    'type' => 'inline_entity_form',
    'weight' => 5,
  ),
);

// Exported field_instance: 'layout-block_layout-field_sidebar_second'
$field_instances['layout-block_layout-field_sidebar_second'] = array(
  'bundle' => 'express_layout',
  'deleted' => 0,
  'description' => '',
  'display' => array(
    'default' => array(
      'label' => 'hidden',
      'module' => 'entityreference',
      'settings' => array(
        'link' => FALSE,
      ),
      'type' => 'entityreference_label',
      'weight' => 7,
    ),
    'teaser' => array(
      'label' => 'above',
      'settings' => array(),
      'type' => 'hidden',
      'weight' => 0,
    ),
  ),
  'entity_type' => 'express_layout',
  'fences_wrapper' => 'no_wrapper',
  'field_name' => 'field_sidebar_second',
  'label' => 'Right Sidebar',
  'required' => 0,
  'settings' => array(
    'user_register_form' => FALSE,
  ),
  'widget' => array(
    'active' => 1,
    'module' => 'inline_entity_form',
    'settings' => array(
      'fields' => array(),
      'type_settings' => array(
        'allow_existing' => 1,
        'allow_new' => 1,
        'delete_references' => 0,
        'label_plural' => 'entities',
        'label_singular' => 'entity',
        'match_operator' => 'CONTAINS',
        'override_labels' => 0,
      ),
    ),
    'type' => 'inline_entity_form',
    'weight' => 7,
  ),
);

// Exported field_instance: 'layout-block_layout-field_slider'
$field_instances['layout-block_layout-field_slider'] = array(
  'bundle' => 'express_layout',
  'deleted' => 0,
  'description' => '',
  'display' => array(
    'default' => array(
      'label' => 'hidden',
      'module' => 'entityreference',
      'settings' => array(
        'link' => FALSE,
      ),
      'type' => 'entityreference_label',
      'weight' => 5,
    ),
    'teaser' => array(
      'label' => 'above',
      'settings' => array(),
      'type' => 'hidden',
      'weight' => 0,
    ),
  ),
  'entity_type' => 'express_layout',
  'fences_wrapper' => 'no_wrapper',
  'field_name' => 'field_slider',
  'label' => 'Slider',
  'required' => 0,
  'settings' => array(
    'user_register_form' => FALSE,
  ),
  'widget' => array(
    'active' => 1,
    'module' => 'inline_entity_form',
    'settings' => array(
      'fields' => array(),
      'type_settings' => array(
        'allow_existing' => 1,
        'allow_new' => 1,
        'delete_references' => 0,
        'label_plural' => 'entities',
        'label_singular' => 'entity',
        'match_operator' => 'CONTAINS',
        'override_labels' => 0,
      ),
    ),
    'type' => 'inline_entity_form',
    'weight' => 3,
  ),
);

// Exported field_instance: 'layout-block_layout-field_wide_2'
$field_instances['layout-block_layout-field_wide_2'] = array(
  'bundle' => 'express_layout',
  'deleted' => 0,
  'description' => '',
  'display' => array(
    'default' => array(
      'label' => 'hidden',
      'module' => 'entityreference',
      'settings' => array(
        'link' => FALSE,
      ),
      'type' => 'entityreference_label',
      'weight' => 8,
    ),
    'teaser' => array(
      'label' => 'above',
      'settings' => array(),
      'type' => 'hidden',
      'weight' => 0,
    ),
  ),
  'entity_type' => 'express_layout',
  'fences_wrapper' => 'no_wrapper',
  'field_name' => 'field_wide_2',
  'label' => 'Post Wide',
  'required' => 0,
  'settings' => array(
    'user_register_form' => FALSE,
  ),
  'widget' => array(
    'active' => 1,
    'module' => 'inline_entity_form',
    'settings' => array(
      'fields' => array(),
      'type_settings' => array(
        'allow_existing' => 1,
        'allow_new' => 1,
        'delete_references' => 0,
        'label_plural' => 'entities',
        'label_singular' => 'entity',
        'match_operator' => 'CONTAINS',
        'override_labels' => 0,
      ),
    ),
    'type' => 'inline_entity_form',
    'weight' => 15,
  ),
);

// Exported field_instance: 'layout-block_layout-field_wide_2'
$field_instances['layout-block_layout-field_content_bottom'] = array(
  'bundle' => 'express_layout',
  'deleted' => 0,
  'description' => '',
  'display' => array(
    'default' => array(
      'label' => 'hidden',
      'module' => 'entityreference',
      'settings' => array(
        'link' => FALSE,
      ),
      'type' => 'entityreference_label',
      'weight' => 8,
    ),
    'teaser' => array(
      'label' => 'above',
      'settings' => array(),
      'type' => 'hidden',
      'weight' => 0,
    ),
  ),
  'entity_type' => 'express_layout',
  'fences_wrapper' => 'no_wrapper',
  'field_name' => 'field_content_bottom',
  'label' => 'Inner Content Bottom',
  'required' => 0,
  'settings' => array(
    'user_register_form' => FALSE,
  ),
  'widget' => array(
    'active' => 1,
    'module' => 'inline_entity_form',
    'settings' => array(
      'fields' => array(),
      'type_settings' => array(
        'allow_existing' => 1,
        'allow_new' => 1,
        'delete_references' => 0,
        'label_plural' => 'entities',
        'label_singular' => 'entity',
        'match_operator' => 'CONTAINS',
        'override_labels' => 0,
      ),
    ),
    'type' => 'inline_entity_form',
    'weight' => 12,
  ),
);

// Translatables
// Included for use with string extractors like potx.
t('Above Content');
t('Add a large image above the page title. Using this feature will remove any content in the intro wide region.');
t('Below Content');
t('Choosing the Feature Layout option removes the sidebars and post wide regions to focus on the page content.');
t('Feature Layout');
t('Inner Left');
t('Inner Right');
t('Intro Wide');
t('Left Sidebar');
t('Node ID');
t('Post Wide');
t('Right Sidebar');
t('Slider');
t('Title Image');
t('Inner Content Bottom');

foreach($field_instances as $instance) {

  $field_info = field_info_instance('express_layout', $instance['field_name'], 'express_layout');
  if (is_null($field_info)) {
    field_create_instance($instance);
  }
}
