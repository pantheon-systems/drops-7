<?php
/**
 * @file
 * visualization_entity_choropleth_bundle.features.field_instance.inc
 */

/**
 * Implements hook_field_default_field_instances().
 */
function visualization_entity_choropleth_bundle_field_default_field_instances() {
  $field_instances = array();

  // Exported field_instance: 'visualization-choropleth_visualization-field_average_multiple_rows'
  $field_instances['visualization-choropleth_visualization-field_average_multiple_rows'] = array(
    'bundle' => 'choropleth_visualization',
    'default_value' => array(
      0 => array(
        'value' => 1,
      ),
    ),
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
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'visualization',
    'field_name' => 'field_average_multiple_rows',
    'label' => 'average multiple rows',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(
        'display_label' => 0,
      ),
      'type' => 'options_onoff',
      'weight' => 4,
    ),
  );

  // Exported field_instance: 'visualization-choropleth_visualization-field_color_scale'
  $field_instances['visualization-choropleth_visualization-field_color_scale'] = array(
    'bundle' => 'choropleth_visualization',
    'default_value' => array(
      0 => array(
        'value' => 0,
      ),
    ),
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'visualization_entity_choropleth_bundle',
        'settings' => array(),
        'type' => 'choropleth_color_scale',
        'weight' => 3,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'visualization',
    'field_name' => 'field_color_scale',
    'label' => 'color scale',
    'required' => 0,
    'settings' => array(
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
      'weight' => 3,
    ),
  );

  // Exported field_instance: 'visualization-choropleth_visualization-field_data_breakpoints'
  $field_instances['visualization-choropleth_visualization-field_data_breakpoints'] = array(
    'bundle' => 'choropleth_visualization',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_plain',
        'weight' => 5,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'visualization',
    'field_name' => 'field_data_breakpoints',
    'label' => 'data breakpoints',
    'required' => 0,
    'settings' => array(
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
      'weight' => 6,
    ),
  );

  // Exported field_instance: 'visualization-choropleth_visualization-field_data_column'
  $field_instances['visualization-choropleth_visualization-field_data_column'] = array(
    'bundle' => 'choropleth_visualization',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_plain',
        'weight' => 4,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'visualization',
    'field_name' => 'field_data_column',
    'label' => 'data column',
    'required' => 0,
    'settings' => array(
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
      'weight' => 5,
    ),
  );

  // Exported field_instance: 'visualization-choropleth_visualization-field_data_units'
  $field_instances['visualization-choropleth_visualization-field_data_units'] = array(
    'bundle' => 'choropleth_visualization',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_plain',
        'weight' => 6,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'visualization',
    'field_name' => 'field_data_units',
    'label' => 'data units',
    'required' => 0,
    'settings' => array(
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
      'weight' => 7,
    ),
  );

  // Exported field_instance: 'visualization-choropleth_visualization-field_geojson'
  $field_instances['visualization-choropleth_visualization-field_geojson'] = array(
    'bundle' => 'choropleth_visualization',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'entityreference',
        'settings' => array(
          'link' => 1,
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
    'entity_type' => 'visualization',
    'field_name' => 'field_geojson',
    'label' => 'geojson',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(
        'apply_chosen' => 0,
      ),
      'type' => 'options_select',
      'weight' => 0,
    ),
  );

  // Exported field_instance: 'visualization-choropleth_visualization-field_uuid_resource'
  $field_instances['visualization-choropleth_visualization-field_uuid_resource'] = array(
    'bundle' => 'choropleth_visualization',
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
        'weight' => 11,
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
    'label' => 'uuid_resource',
    'required' => 0,
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

  // Exported field_instance: 'visualization-choropleth_visualization-field_ve_geojson_key'
  $field_instances['visualization-choropleth_visualization-field_ve_geojson_key'] = array(
    'bundle' => 'choropleth_visualization',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'Property on GeoJSON which contains same values as "Map Column" in data.',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 8,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'visualization',
    'field_name' => 'field_ve_geojson_key',
    'label' => 'GeoJSON Key',
    'required' => 0,
    'settings' => array(
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
      'weight' => 8,
    ),
  );

  // Exported field_instance: 'visualization-choropleth_visualization-field_ve_geojson_label'
  $field_instances['visualization-choropleth_visualization-field_ve_geojson_label'] = array(
    'bundle' => 'choropleth_visualization',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'Property on GeoJSON which has human readable name.',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 10,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'visualization',
    'field_name' => 'field_ve_geojson_label',
    'label' => 'GeoJSON Label',
    'required' => 0,
    'settings' => array(
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
      'weight' => 10,
    ),
  );

  // Exported field_instance: 'visualization-choropleth_visualization-field_ve_map_column'
  $field_instances['visualization-choropleth_visualization-field_ve_map_column'] = array(
    'bundle' => 'choropleth_visualization',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'Column in the data which maps to a property in the geojon.  If there is a column in the data "State Name" with values like California and Pennsylvania it should map to a property in the geojson like "State" with the same values. ',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 9,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'visualization',
    'field_name' => 'field_ve_map_column',
    'label' => 'Map Column',
    'required' => 0,
    'settings' => array(
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
      'weight' => 9,
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Column in the data which maps to a property in the geojon.  If there is a column in the data "State Name" with values like California and Pennsylvania it should map to a property in the geojson like "State" with the same values. ');
  t('GeoJSON Key');
  t('GeoJSON Label');
  t('Map Column');
  t('Property on GeoJSON which contains same values as "Map Column" in data.');
  t('Property on GeoJSON which has human readable name.');
  t('average multiple rows');
  t('color scale');
  t('data breakpoints');
  t('data column');
  t('data units');
  t('geojson');
  t('uuid_resource');

  return $field_instances;
}
