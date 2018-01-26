<?php
/**
 * @file
 * cu_facebook_like_button.features.field_base.inc
 */

/**
 * Implements hook_field_default_field_bases().
 */
function cu_facebook_like_button_field_default_field_bases() {
  $field_bases = array();

  // Exported field_base: 'field_fb_verb'.
  $field_bases['field_fb_verb'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_fb_verb',
    'indexes' => array(
      'value' => array(
        0 => 'value',
      ),
    ),
    'locked' => 0,
    'module' => 'list',
    'settings' => array(
      'allowed_values' => array(
        'like' => 'Like',
        'recommend' => 'Recommend',
      ),
      'allowed_values_function' => '',
      'allowed_values_php' => '',
    ),
    'translatable' => 0,
    'type' => 'list_text',
  );

  return $field_bases;
}
