<?php
/**
 * @file
 * visualization_entity_charts_dkan.features.field_instance.inc
 */

/**
 * Implements hook_field_default_field_instances().
 */
function visualization_entity_charts_dkan_field_default_field_instances() {
  $field_instances = array();

  // Exported field_instance: 'visualization-ve_chart-field_uuid_resource'
  $field_instances['visualization-ve_chart-field_uuid_resource'] = array(
    'bundle' => 've_chart',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'uuidreference',
        'settings' => array(
          'link' => FALSE,
        ),
        'type' => 'uuidreference_label',
        'weight' => 14,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'visualization',
    'field_name' => 'field_uuid_resource',
    'label' => 'Existing resource',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'uuidreference',
      'settings' => array(
        'autocomplete_path' => 'uuidreference/autocomplete',
      ),
      'type' => 'uuidreference_autocomplete',
      'weight' => 2,
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Existing resource');

  return $field_instances;
}
