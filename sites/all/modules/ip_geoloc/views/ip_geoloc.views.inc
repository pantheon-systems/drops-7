<?php
/**
 * @file
 * ip_geoloc.views.inc
 *
 * Views data from IP Geolocation Views & Maps module
 */

/**
 * Implements hook_views_data().
 */
function ip_geoloc_views_data() {
  $data = array();
  $data['ip_geoloc']['table']['group']  = t('IP geolocations');

  $data['ip_geoloc']['table']['base'] = array(
    'field' => 'ip_address',
    'title' => t('IP geolocations'),
    'help' => t('Stores IP geolocation information.'),
  );

  $data['ip_geoloc']['table']['join'] = array(
    // Make all of our fields instantly available in an Access Log view.
    // ip_geoloc.ip_address connects to accesslog.hostname
    'accesslog' => array(
      'left_field' => 'hostname',
      'field' => 'ip_address',
      'type' => 'INNER',
    ),
    // Make all of our fields instantly available in an Orders view.
    // ip_geoloc.ip_address connects to commerce_order.hostname
    'commerce_order' => array(
      'left_field' => 'hostname',
      'field' => 'ip_address',
      'type' => 'LEFT',
    ),
    // Make all of our fields instantly available in an Order Revisions view.
    // ip_geoloc.ip_address connects to
    // commerce_order_revision.revision_hostname
    'commerce_order_revision' => array(
      'left_field' => 'revision_hostname',
      'field' => 'ip_address',
      'type' => 'LEFT',
    ),
  );

  $data['ip_geoloc']['ip_address'] = array(
    'title' => t('IP address'),
    'help' => t('IP address'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['ip_geoloc']['latitude'] = array(
    'title' => t('Latitude'),
    'help' => t('Latitude'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['ip_geoloc']['longitude'] = array(
    'title' => t('Longitude'),
    'help' => t('Longitude'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['ip_geoloc']['formatted_address'] = array(
    'title' => t('Address'),
    'help' => t('Address formatted as a string'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['ip_geoloc']['country'] = array(
    'title' => t('Country'),
    'help' => t('Country'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['ip_geoloc']['country_code'] = array(
    'title' => t('Country code'),
    'help' => t('Country code'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['ip_geoloc']['region'] = array(
    'title' => t('Region'),
    'help' => t('Region'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['ip_geoloc']['region_code'] = array(
    'title' => t('Region code'),
    'help' => t('Region code'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['ip_geoloc']['city'] = array(
    'title' => t('City'),
    'help' => t('City'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['ip_geoloc']['postal_code'] = array(
    'title' => t('Postal code'),
    'help' => t('Post code or ZIP'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['ip_geoloc']['locality'] = array(
    'title' => t('Suburb'),
    'help' => t('Locality (e.g. suburb)'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['ip_geoloc']['route'] = array(
    'title' => t('Street'),
    'help' => t('Route (street)'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['ip_geoloc']['street_number'] = array(
    'title' => t('Street no.'),
    'help' => t('Street number'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['ip_geoloc']['administrative_area_level_1'] = array(
    'title' => t('State'),
    'help' => t('administrative area level 1 (state or province)'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  return $data;
}

/**
 * Implements hook_views_plugins().
 *
 * 3 Views style/format plugins and 1 contextual filter default argument plugin.
 */
function ip_geoloc_views_plugins() {
  $path = drupal_get_path('module', 'ip_geoloc');
  $plugins = array(
    'style' => array(
      'ip_geoloc_map' => array(
        'title' => t('Map (Google API, via IPGV&M)'),
        'help' => t('Displays the view as a Google map.'),
        'help topic' => 'style-ip-geoloc-map',
        'handler' => 'ip_geoloc_plugin_style_map',
        'path' => $path . '/views',
        // Uses ip-geoloc-map.tpl.php.
        'theme' => 'ip_geoloc_map',
        'theme path' => $path . '/views',
        'uses fields' => TRUE,
        // Map style and map options.
        'uses options' => TRUE,
        'uses grouping' => TRUE,
        'uses row plugin' => FALSE,
        'type' => 'normal',
        // May be overridden in ip_geoloc_plugin_style_map::render().
        'even empty' => TRUE,
      ),
    ),
  );
  if (module_exists('leaflet')) {
    $plugins['style']['ip_geoloc_leaflet'] = array(
      'title' => t('Map (Leaflet API, via IPGV&M)'),
      'help' => t('Displays the view as a map via the Leaflet API.'),
      'handler' => 'ip_geoloc_plugin_style_leaflet',
      'path' => $path . '/views',
      // Uses ip-geoloc-leaflet.tpl.php.
      'theme' => 'ip_geoloc_leaflet',
      'theme path' => $path . '/views',
      'uses fields' => TRUE,
      'uses options' => TRUE,
      'uses grouping' => TRUE,
      'uses row plugin' => FALSE,
      'type' => 'normal',
      // May be overridden in ip_geoloc_plugin_style_leaflet::render().
      'even empty' => TRUE,
    );
  }
  if (module_exists('openlayers')) {
    $plugins['style']['ip_geoloc_openlayers'] = array(
      'title' => t('Map (OpenLayers API, via IPGV&M)'),
      'help' => t('Displays the view as a map via the OpenLayers API.'),
      'handler' => 'ip_geoloc_plugin_style_openlayers',
      'path' => $path . '/views',
      // Uses ip-geoloc-openlayers.tpl.php.
      'theme' => 'ip_geoloc_openlayers',
      'theme path' => $path . '/views',
      'uses fields' => TRUE,
      'uses options' => TRUE,
      'uses grouping' => TRUE,
      'uses row plugin' => FALSE,
      'type' => 'normal',
      // May be overridden in ip_geoloc_plugin_style_openlayers::render().
      'even empty' => TRUE,
    );
  }

  $plugins['argument default'] = array(
    'location_attribute' => array(
      'title' => t('Visitor location attribute (via IPGV&M)'),
      'handler' => 'ip_geoloc_plugin_argument_default_ip_geoloc',
      'path' => $path . '/views',
    ),
  );

  return $plugins;
}
