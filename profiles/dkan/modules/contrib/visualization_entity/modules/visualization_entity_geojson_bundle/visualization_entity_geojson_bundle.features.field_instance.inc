<?php
/**
 * @file
 * visualization_entity_geojson_bundle.features.field_instance.inc
 */

/**
 * Implements hook_field_default_field_instances().
 */
function visualization_entity_geojson_bundle_field_default_field_instances() {
  $field_instances = array();

  // Exported field_instance: 'visualization-geojson_visualization-field_uuid_resource'
  $field_instances['visualization-geojson_visualization-field_uuid_resource'] = array(
    'bundle' => 'geojson_visualization',
    'default_value' => array(
      0 => array(
        'target_uuid' => '704dfe95-cd90-4ebf-8a6c-6394e5b23cb3',
      ),
    ),
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'uuidreference',
        'settings' => array(
          'link' => 1,
        ),
        'type' => 'uuidreference_label',
        'weight' => 0,
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
    'label' => 'Resource',
    'required' => 1,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'uuidreference_select',
      'settings' => array(),
      'type' => 'uuidreference_select',
      'weight' => 2,
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Resource');

  return $field_instances;
}
