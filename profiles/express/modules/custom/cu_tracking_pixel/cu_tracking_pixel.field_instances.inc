<?php

$field_instances = array();

// Exported field_instance:
// 'tracking_pixel-tracking_pixel_code-field_tracking_code'.
$field_instances['tracking_pixel-field_tracking_code'] = array(
  'bundle' => 'tracking_pixel',
  'default_value' => NULL,
  'deleted' => 0,
  'description' => '',
  'display' => array(
    'default' => array(
      'label' => 'above',
      'module' => 'text',
      'settings' => array(),
      'type' => 'text_default',
      'weight' => 1,
    ),
  ),
  'entity_type' => 'tracking_pixel',
  'fences_wrapper' => 'no_wrapper',
  'field_name' => 'field_tracking_code',
  'label' => 'Tracking Code',
  'required' => 0,
  'settings' => array(
    'linkit' => array(
      'button_text' => 'Search',
      'enable' => 0,
      'profile' => '',
    ),
    'text_processing' => 0,
    'user_register_form' => FALSE,
  ),
  'widget' => array(
    'active' => 1,
    'module' => 'text',
    'settings' => array(
      'rows' => 5,
    ),
    'type' => 'text_textarea',
    'weight' => 2,
  ),
);

// Exported field_instance:
// 'tracking_pixel-tracking_pixel_code-field_tracking_name'.
$field_instances['tracking_pixel-field_tracking_name'] = array(
  'bundle' => 'tracking_pixel',
  'default_value' => NULL,
  'deleted' => 0,
  'description' => '',
  'display' => array(
    'default' => array(
      'label' => 'above',
      'module' => 'text',
      'settings' => array(),
      'type' => 'text_default',
      'weight' => 2,
    ),
  ),
  'entity_type' => 'tracking_pixel',
  'fences_wrapper' => 'no_wrapper',
  'field_name' => 'field_tracking_name',
  'label' => 'Name',
  'required' => 1,
  'settings' => array(
    'linkit' => array(
      'button_text' => 'Search',
      'enable' => 0,
      'profile' => '',
    ),
    'text_processing' => 0,
    'user_register_form' => FALSE,
  ),
  'widget' => array(
    'active' => 1,
    'module' => 'text',
    'settings' => array(
      'size' => 60,
    ),
    'type' => 'text_textfield',
    'weight' => 0,
  ),
);

// Exported field_instance:
// 'tracking_pixel-tracking_pixel_code-field_tracking_path'.
$field_instances['tracking_pixel-field_tracking_path'] = array(
  'bundle' => 'tracking_pixel',
  'default_value' => NULL,
  'deleted' => 0,
  'description' => '',
  'display' => array(
    'default' => array(
      'label' => 'above',
      'module' => 'link',
      'settings' => array(),
      'type' => 'link_default',
      'weight' => 0,
    ),
  ),
  'entity_type' => 'tracking_pixel',
  'fences_wrapper' => 'no_wrapper',
  'field_name' => 'field_tracking_path',
  'label' => 'Path',
  'required' => 1,
  'settings' => array(
    'absolute_url' => 0,
    'attributes' => array(
      'class' => '',
      'configurable_class' => 0,
      'configurable_title' => 0,
      'rel' => '',
      'target' => 'default',
      'title' => '',
    ),
    'display' => array(
      'url_cutoff' => 80,
    ),
    'enable_tokens' => 1,
    'linkit' => array(
      'button_text' => 'Search',
      'enable' => 1,
      'profile' => 'express_linkit_field_profile',
    ),
    'rel_remove' => 'default',
    'title' => 'none',
    'title_label_use_field_label' => 0,
    'title_maxlength' => 128,
    'title_value' => '',
    'url' => 0,
    'user_register_form' => FALSE,
    'validate_url' => 1,
  ),
  'widget' => array(
    'active' => 0,
    'module' => 'link',
    'settings' => array(),
    'type' => 'link_field',
    'weight' => 1,
  ),
);

// Translatables
// Included for use with string extractors like potx.
t('Name');
t('Path');
t('Tracking Code');



foreach($field_instances as $instance) {
  $field_info = field_info_instance('tracking_pixel', $instance['field_name'], 'tracking_pixel');
  if (is_null($field_info)) {
    field_create_instance($instance);
  }
}
