<?php

/**
 * @file
 * cu_block_to_bean.features.field_instance.inc
 */

/**
 * Implements hook_field_default_field_instances().
 */
function cu_block_to_bean_field_default_field_instances() {
  $field_instances = array();

  // Exported field_instance:
  // 'bean-block_wrapper-field_block_wrapper_reference'.
  $field_instances['bean-block_wrapper-field_block_wrapper_reference'] = array(
    'bundle' => 'block_wrapper',
    'default_value' => NULL,
    'default_value_function' => '',
    'default_value_php' => FALSE,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'list',
        'settings' => array(),
        'type' => 'list_default',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'bean',
    'fences_wrapper' => 'div_div_div',
    'field_name' => 'field_block_wrapper_reference',
    'label' => 'Block',
    'required' => 0,
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

  // Translatables
  // Included for use with string extractors like potx.
  t('Block');

  return $field_instances;
}
