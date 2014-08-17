<?php

/**
 * @file
 * ip_geoloc.openlayers.inc
 *
 * Hook implementations for the OpenLayers interface.
 */

define('IP_GEOLOC_VISITOR_MARKER_LAYER', 'ip_geoloc_visitor_marker_layer');
define('IP_GEOLOC_MARKER_LAYER', 'ip_geoloc_marker_layer');
define('IP_GEOLOC_DEF_NUM_MARKER_LAYERS', 3);

/**
 * Implements hook_openlayers_map_preprocess_alter().
 *
 * Add or alter map before main processing. Add styles, behaviors, layers....
 * Called from openlayers_build_map().
 * At this point center, zoom level etc. have already been initialised from the
 * Map configuration page.
 */
function ip_geoloc_openlayers_map_preprocess_alter(&$map) {

  // $map['center']['initial']['zoom'] = 5;
  // Introduce a number of layers that will be created below in function
  // ip_geoloc_openlayers_layers().
  $map['layers'][IP_GEOLOC_VISITOR_MARKER_LAYER] = IP_GEOLOC_VISITOR_MARKER_LAYER;

  $num_marker_layers = variable_get('ip_geoloc_num_location_marker_layers', IP_GEOLOC_DEF_NUM_MARKER_LAYERS);
  for ($layer = 1; $layer <= $num_marker_layers; $layer++) {
    $map['layers'][IP_GEOLOC_MARKER_LAYER . $layer] = IP_GEOLOC_MARKER_LAYER . $layer;
    $map['layer_activated'][IP_GEOLOC_MARKER_LAYER . $layer] = IP_GEOLOC_MARKER_LAYER . $layer;
    // Markers may be switched on/off via a tickbox on the rendered map, if so
    // configured on page /admin/structure/openlayers/maps/<map_name>/edit,
    // vertical tab "Layers & Styles", bottom section "Overlay layers"
    // Programmatically, you'd do this like so:
    // $map['layer_switcher'][IP_GEOLOC_MARKER_LAYER . $layer] =
    // IP_GEOLOC_MARKER_LAYER . $layer;
  }
}

/**
 * Implements hook_ctools_plugin_api().
 *
 * Required to add a layer to OpenLayers, see ip_geoloc_openlayers_layers().
 */
function ip_geoloc_ctools_plugin_api($module, $api) {
  if ($module == 'openlayers' && $api == 'openlayers_layers') {
    return array('version' => 1);
  }
}

/**
 * Implements hook_openlayers_layers().
 *
 * Called via ctools in the heart of openlayers_build_map().
 * This function has no arguments, so layers are added unconditionally, unless
 * globals are used.
 */
function ip_geoloc_openlayers_layers() {
  $layers = array();
  $visitor_layer = new stdClass();
  $visitor_layer->api_version = 1;
  $visitor_layer->name = IP_GEOLOC_VISITOR_MARKER_LAYER;
  $visitor_layer->title = 'Current visitor marker';
  $visitor_layer->description = "Layer to mark visitor's current position.";
  $visitor_layer->weight = -10;
  $visitor_layer->data = array(
    'isBaselayer' => FALSE,
    'layer_type' => 'openlayers_layer_type_raw',
    'projection' => array('EPSG:900913'),
  );
  // Add the visitor's location as a single feature in its own layer, so
  // that it may be separately styled (eg marker colour) and activated on page
  // admin/structure/openlayers/maps/<map-name>/edit
  $location = ip_geoloc_get_visitor_location();
  if (!empty($location) && isset($location['longitude'])) {
    $longitude = $location['longitude'];
    $latitude  = $location['latitude'];
    $visitor_layer->data['features'][] = array(
      'attributes' => array('name' => t('Your approximate location.')),
      'wkt' => "POINT($longitude $latitude)",
      'projection' => 'EPSG:4326',
    );
  }
  $layers[$visitor_layer->name] = $visitor_layer;

  $num_marker_layers = variable_get('ip_geoloc_num_location_marker_layers', IP_GEOLOC_DEF_NUM_MARKER_LAYERS);
  for ($layer = 1; $layer <= $num_marker_layers; $layer++) {
    $marker_layer = new stdClass();
    $marker_layer->api_version = 1;
    $marker_layer->name = IP_GEOLOC_MARKER_LAYER . $layer;
    $marker_layer->title = "Location markers #$layer";
    $marker_layer->description = "Layer to map a view's locations.";
    $marker_layer->weight = $visitor_layer->weight + $layer;
    $marker_layer->data = array(
      'isBaselayer' => FALSE,
      'layer_type' => 'openlayers_layer_type_raw',
      'projection' => array('EPSG:900913'),
    );
    // Features, ie. location markers, are added based on the view output, see
    // file ip_geoloc_plugin_style_openlaysers.inc
    $layers[$marker_layer->name] = $marker_layer;
  }

  // cache_set('ip_geoloc_layer_cache', $layers);
  return $layers;
}

/**
 * Implements hook_openlayers_layer_types().
 *
 * Allows user to change the title and save the layer.
 *
 * See http://drupal.org/node/1824696
 */
function ip_geoloc_openlayers_layer_types() {
  return array(
    'openlayers_layer_type_ip_geoloc' => array(
      'title' => t('Raw'),
      'description' => t('Layer type for raw data input'),
      'type' => 'layer',
      'path' => drupal_get_path('module', 'openlayers') . '/plugins/layer_types',
      'file' => 'openlayers_layer_type_raw.inc',
      'layer_type' => array(
        'file' => 'openlayers_layer_type_raw.inc',
        'class' => 'openlayers_layer_type_raw',
        'parent' => 'openlayers_layer_type',
      ),
    ),
  );
}

/**
 * Implements hook_openlayers_map_alter().
 *
 * Called at the end of openlayers_build_map().
 */
function ip_geoloc_openlayers_map_alter(&$map) {
}
