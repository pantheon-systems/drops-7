<?php
/**
 * @file
 * cu_block_to_bean.features.field_base.inc
 */

/**
 * Implements hook_field_default_field_bases().
 */
function cu_block_to_bean_field_default_field_bases() {
  $field_bases = array();

  // Exported field_base: 'field_block_wrapper_reference'.
  $field_bases['field_block_wrapper_reference'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_block_wrapper_reference',
    'indexes' => array(
      'value' => array(
        0 => 'value',
      ),
    ),
    'locked' => 0,
    'module' => 'list',
    'settings' => array(
      'allowed_values' => array(),
      'allowed_values_function' => '_wrapper_block_list',
      'allowed_values_php' => FALSE,
    ),
    'translatable' => 0,
    'type' => 'list_text',
  );

  return $field_bases;
}
