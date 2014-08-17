<?php

/**
 * @file
 * API documentation for Administration menu.
 */

/**
 * Define one or map definitions to be used when rendering a map.
 *
 * leaflet_map_get_info() will grab every defined map, and the returned
 * associative array is then passed to leaflet_render_map(), along with a
 * collection of features.
 *
 * The settings array maps to the settings available to leaflet map object,
 * http://leaflet.cloudmade.com/reference.html#map-properties
 *
 * Layers are the available base layers for the map and, if you enable the
 * layer control, can be toggled on the map.
 *
 * @return array
 *   Associative array containing a complete leaflet map definition.
 */
function hook_leaflet_map_info() {
  return array(
    'OSM Mapnik' => array(
      'label' => 'OSM Mapnik',
      'description' => t('Leaflet default map.'),
      'settings' => array(
        'dragging' => TRUE,
        'touchZoom' => TRUE,
        'scrollWheelZoom' => TRUE,
        'doubleClickZoom' => TRUE,
        'zoomControl' => TRUE,
        'attributionControl' => TRUE,
        'trackResize' => TRUE,
        'fadeAnimation' => TRUE,
        'zoomAnimation' => TRUE,
        'closePopupOnClick' => TRUE,
        'layerControl' => TRUE,
        // 'minZoom' => 10,
        // 'maxZoom' => 15,
        // 'zoom' => 15, // set the map zoom fixed to 15
      ),
      'layers' => array(
        'earth' => array(
          'urlTemplate' => 'http://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png',
          'options' => array(
            'attribution' => 'OSM Mapnik',
          ),
        ),
      ),
      // Uncomment the lines below to use a custom icon
      // 'icon' => array(
      //   'iconUrl'       => '/sites/default/files/icon.png',
      //   'iconSize'      => array('x' => '20', 'y' => '40'),
      //   'iconAnchor'    => array('x' => '20', 'y' => '40'),
      //   'popupAnchor'   => array('x' => '-8', 'y' => '-32'),
      //   'shadowUrl'     => '/sites/default/files/icon-shadow.png',
      //   'shadowSize'    => array('x' => '25', 'y' => '27'),
      //   'shadowAnchor'  => array('x' => '0', 'y' => '27'),
      // ),
    ),
  );
}
