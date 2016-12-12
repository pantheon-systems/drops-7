<?php
/**
 * @file
 * cu_block_row.features.field_instance.inc
 */

/**
 * Implements hook_field_default_field_instances().
 */
function cu_block_row_field_default_field_instances() {
  $field_instances = array();

  // Exported field_instance: 'bean-block_row-field_block_row_collection'.
  $field_instances['bean-block_row-field_block_row_collection'] = array(
    'bundle' => 'block_row',
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
    'fences_wrapper' => 'no_wrapper',
    'field_name' => 'field_block_row_collection',
    'label' => 'Column',
    'required' => 1,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'field_collection',
      'settings' => array(),
      'type' => 'field_collection_embed',
      'weight' => 31,
    ),
  );

  // Exported field_instance: 'bean-block_row-field_block_row_distribution'.
  $field_instances['bean-block_row-field_block_row_distribution'] = array(
    'bundle' => 'block_row',
    'default_value' => array(
      0 => array(
        'value' => 'auto',
      ),
    ),
    'deleted' => 0,
    'description' => 'Distribution assigns more importance to the first or last block by making that column wider than the rest. For example, this allows you to create a 2 column layout where the first column is 2/3 of the total width and the second column is 1/3 of the total width.',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'list',
        'settings' => array(),
        'type' => 'list_key',
        'weight' => 1,
      ),
    ),
    'entity_type' => 'bean',
    'fences_wrapper' => 'no_wrapper',
    'field_name' => 'field_block_row_distribution',
    'label' => 'Distribution',
    'required' => 1,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(),
      'type' => 'options_buttons',
      'weight' => 32,
    ),
  );

  // Exported field_instance:
  // 'field_collection_item-field_block_row_collection-field_block_row_block'.
  $field_instances['field_collection_item-field_block_row_collection-field_block_row_block'] = array(
    'bundle' => 'field_block_row_collection',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'entityreference',
        'settings' => array(
          'links' => 0,
          'use_content_language' => TRUE,
          'view_mode' => 'default',
        ),
        'type' => 'entityreference_entity_view',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'field_collection_item',
    'fences_wrapper' => 'div',
    'field_name' => 'field_block_row_block',
    'label' => 'Block',
    'required' => 1,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'entityreference',
      'settings' => array(
        'match_operator' => 'CONTAINS',
        'path' => '',
        'size' => 60,
      ),
      'type' => 'entityreference_autocomplete',
      'weight' => 31,
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Block');
  t('Column');
  t('Distribution');
  t('Distribution assigns more importance to the first or last block by making that column wider than the rest. For example, this allows you to create a 2 column layout where the first column is 2/3 of the total width and the second column is 1/3 of the total width.');

  return $field_instances;
}
