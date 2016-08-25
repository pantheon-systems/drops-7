<?php
$field_bases = array();

// Exported field_base: 'field_tracking_code'.
$field_bases['field_tracking_code'] = array(
  'active' => 1,
  'cardinality' => 1,
  'deleted' => 0,
  'entity_types' => array(),
  'field_name' => 'field_tracking_code',
  'indexes' => array(
    'format' => array(
      0 => 'format',
    ),
  ),
  'locked' => 0,
  'module' => 'text',
  'settings' => array(),
  'translatable' => 0,
  'type' => 'text_long',
);

// Exported field_base: 'field_tracking_name'.
$field_bases['field_tracking_name'] = array(
  'active' => 1,
  'cardinality' => 1,
  'deleted' => 0,
  'entity_types' => array(),
  'field_name' => 'field_tracking_name',
  'indexes' => array(
    'format' => array(
      0 => 'format',
    ),
  ),
  'locked' => 0,
  'module' => 'text',
  'settings' => array(
    'max_length' => 255,
  ),
  'translatable' => 0,
  'type' => 'text',
);

// Exported field_base: 'field_tracking_path'.
$field_bases['field_tracking_path'] = array(
  'active' => 1,
  'cardinality' => 1,
  'deleted' => 0,
  'entity_types' => array(),
  'field_name' => 'field_tracking_path',
  'indexes' => array(),
  'locked' => 0,
  'module' => 'link',
  'settings' => array(
    'attributes' => array(
      'class' => '',
      'rel' => '',
      'target' => 'default',
    ),
    'display' => array(
      'url_cutoff' => 80,
    ),
    'enable_tokens' => 1,
    'title' => 'optional',
    'title_maxlength' => 128,
    'title_value' => '',
    'url' => 0,
  ),
  'translatable' => 0,
  'type' => 'link_field',
);


foreach ($field_bases as $field) {
  if (!field_info_field($field['field_name'])) {
    field_create_field($field);
  }
}
