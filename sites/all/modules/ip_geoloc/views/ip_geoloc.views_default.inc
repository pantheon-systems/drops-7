<?php

/**
 * @file
 * Canned view to display an alternative visitor log of your site.
 *
 * Which contains for each IP address, the number of visits (clicks), the dates
 * of first and last visit and a map of the centered around the visitor's
 * location.
 */

/**
 * Implements hook_views_default_views().
 */
function ip_geoloc_views_default_views() {
  /*
   * View 'Map (Leaflet) of recent visitors.
   */
  $view = new view();
  $view->name = 'map_leaflet_of_visitor_locations';
  $view->description = 'Map of recent visitors to this site with zoom and cluster features.';
  $view->tag = 'IP Geolocation Views and Maps';
  $view->base_table = 'accesslog';
  $view->human_name = 'Map (Leaflet) of recent visitors';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = TRUE;

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Locations of recent visitors';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['group_by'] = TRUE;
  $handler->display->display_options['access']['type'] = 'role';
  $handler->display->display_options['access']['role'] = array(
    2 => '2',
  );
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['disable_sql_rewrite'] = TRUE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '2000';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'ip_geoloc_leaflet';
  $handler->display->display_options['style_options']['center_option'] = '0';
  $handler->display->display_options['style_options']['zoom'] = '1';
  $handler->display->display_options['style_options']['visitor_marker_color'] = 'white';
  $handler->display->display_options['style_options']['map'] = 'osm-transport';
  $handler->display->display_options['style_options']['map_height'] = '480';
  $handler->display->display_options['style_options']['cluster_radius'] = '80';
  /* Field: MAX(Access log: Timestamp) */
  $handler->display->display_options['fields']['timestamp']['id'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['table'] = 'accesslog';
  $handler->display->display_options['fields']['timestamp']['field'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['group_type'] = 'max';
  $handler->display->display_options['fields']['timestamp']['label'] = 'Last visit';
  $handler->display->display_options['fields']['timestamp']['exclude'] = TRUE;
  $handler->display->display_options['fields']['timestamp']['date_format'] = 'short';
  /* Field: IP geolocations: IP address */
  $handler->display->display_options['fields']['ip_address']['id'] = 'ip_address';
  $handler->display->display_options['fields']['ip_address']['table'] = 'ip_geoloc';
  $handler->display->display_options['fields']['ip_address']['field'] = 'ip_address';
  $handler->display->display_options['fields']['ip_address']['label'] = 'IP';
  $handler->display->display_options['fields']['ip_address']['exclude'] = TRUE;
  /* Field: IP geolocations: Address */
  $handler->display->display_options['fields']['formatted_address']['id'] = 'formatted_address';
  $handler->display->display_options['fields']['formatted_address']['table'] = 'ip_geoloc';
  $handler->display->display_options['fields']['formatted_address']['field'] = 'formatted_address';
  $handler->display->display_options['fields']['formatted_address']['label'] = '';
  $handler->display->display_options['fields']['formatted_address']['element_label_colon'] = FALSE;
  /* Field: IP geolocations: Latitude */
  $handler->display->display_options['fields']['latitude']['id'] = 'latitude';
  $handler->display->display_options['fields']['latitude']['table'] = 'ip_geoloc';
  $handler->display->display_options['fields']['latitude']['field'] = 'latitude';
  $handler->display->display_options['fields']['latitude']['label'] = '';
  $handler->display->display_options['fields']['latitude']['exclude'] = TRUE;
  $handler->display->display_options['fields']['latitude']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['latitude']['separator'] = '';
  /* Field: IP geolocations: Longitude */
  $handler->display->display_options['fields']['longitude']['id'] = 'longitude';
  $handler->display->display_options['fields']['longitude']['table'] = 'ip_geoloc';
  $handler->display->display_options['fields']['longitude']['field'] = 'longitude';
  $handler->display->display_options['fields']['longitude']['label'] = '';
  $handler->display->display_options['fields']['longitude']['exclude'] = TRUE;
  $handler->display->display_options['fields']['longitude']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['longitude']['separator'] = '';
  /* Sort criterion: MAX(Access log: Timestamp) */
  $handler->display->display_options['sorts']['timestamp']['id'] = 'timestamp';
  $handler->display->display_options['sorts']['timestamp']['table'] = 'accesslog';
  $handler->display->display_options['sorts']['timestamp']['field'] = 'timestamp';
  $handler->display->display_options['sorts']['timestamp']['group_type'] = 'max';
  $handler->display->display_options['sorts']['timestamp']['order'] = 'DESC';

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block');
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $translatables['map_leaflet_of_visitor_locations'] = array(
    t('Master'),
    t('Locations of 2000 most recent visitors'),
    t('more'),
    t('Apply'),
    t('Reset'),
    t('Sort by'),
    t('Asc'),
    t('Desc'),
    t('Last visit'),
    t('IP'),
    t('.'),
    t('Block'),
  );
  $views[$view->name] = $view;

  /*
   * View 'Map (Google) centered on visitor's location'
   */
  $view = new view();
  $view->name = 'map_google_centered_on_visitor_location';
  $view->description = 'You may set any color for the visitor location marker.';
  $view->tag = 'IP Geolocation Views and Maps';
  $view->base_table = 'node';
  $view->human_name = 'Map (Google) centered on visitor\'s location';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = TRUE;

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Your location';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '0';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'ip_geoloc_map';
  $handler->display->display_options['style_options']['grouping'] = array(
    0 => array(
      'field' => '',
      'rendered' => 1,
      'rendered_strip' => 0,
    ),
  );
  $handler->display->display_options['style_options']['ip_geoloc_views_plugin_latitude'] = '';
  $handler->display->display_options['style_options']['ip_geoloc_views_plugin_longitude'] = '';
  $handler->display->display_options['style_options']['center_option'] = '2';
  $handler->display->display_options['style_options']['visitor_marker'] = '00FF00';
  $handler->display->display_options['style_options']['gps_roles'] = array(
    1 => '1',
    2 => '2',
    3 => 0,
  );
  $handler->display->display_options['style_options']['map_options'] = '{"mapTypeId":"roadmap", "disableDefaultUI":true, "zoom":15, "zoomControl":true, "scaleControl":true}';
  $handler->display->display_options['style_options']['empty_map_center'] = 'visitor';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  /* Filter criterion: Content: Title */
  $handler->display->display_options['filters']['title']['id'] = 'title';
  $handler->display->display_options['filters']['title']['table'] = 'node';
  $handler->display->display_options['filters']['title']['field'] = 'title';
  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block');
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;

  /*
   * View 'Map (Leaflet) centered on visitor's location'
   */
  $view = new view();
  $view->name = 'map_leaflet_centered_on_visitor_location';
  $view->description = 'Enable the "Leaflet More Maps" module for a variety of attractive map styles.';
  $view->tag = 'IP Geolocation Views and Maps';
  $view->base_table = 'node';
  $view->human_name = 'Map (Leaflet) centered on visitor\'s location';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = TRUE;
  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Your location';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '0';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'ip_geoloc_leaflet';
  $handler->display->display_options['style_options']['grouping'] = array(
    0 => array(
      'field' => '',
      'rendered' => 1,
      'rendered_strip' => 0,
    ),
  );
  $handler->display->display_options['style_options']['ip_geoloc_views_plugin_latitude'] = '';
  $handler->display->display_options['style_options']['ip_geoloc_views_plugin_longitude'] = '';
  $handler->display->display_options['style_options']['center_option'] = '2';
  $handler->display->display_options['style_options']['empty_map_center'] = 'visitor';
  $handler->display->display_options['style_options']['visitor_marker_color'] = '';
  $handler->display->display_options['style_options']['map_height'] = '300';
  $handler->display->display_options['style_options']['map_options']['maxzoom'] = '18';
  $handler->display->display_options['style_options']['map_options']['zoom'] = '15';
  $handler->display->display_options['style_options']['map_options']['scrollwheelzoom'] = '1';
  $handler->display->display_options['style_options']['map_options']['dragging'] = '1';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  /* Filter criterion: Content: Title */
  $handler->display->display_options['filters']['title']['id'] = 'title';
  $handler->display->display_options['filters']['title']['table'] = 'node';
  $handler->display->display_options['filters']['title']['field'] = 'title';
  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block');
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $views[$view->name] = $view;

  /*
   * View 'Map (OpenLayers) centered on visitor's location'
   */
  $view = new view();
  $view->name = 'map_ol_centered_on_visitor_location';
  $view->description = 'Various maps and marker layers are available through OpenLayers';
  $view->tag = 'IP Geolocation Views and Maps';
  $view->base_table = 'node';
  $view->human_name = 'Map (OpenLayers) centered on visitor\'s location';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = TRUE;
  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Your location';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '0';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'ip_geoloc_openlayers';
  $handler->display->display_options['style_options']['ip_geoloc_views_plugin_latitude'] = '';
  $handler->display->display_options['style_options']['ip_geoloc_views_plugin_longitude'] = '';
  $handler->display->display_options['style_options']['default_marker_color'] = '1';
  $handler->display->display_options['style_options']['center_option'] = '2';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block');
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $translatables['map_centered_on_visitor_location'] = array(
    t('Master'),
    t('Your location'),
    t('more'),
    t('Apply'),
    t('Reset'),
    t('Sort by'),
    t('Asc'),
    t('Desc'),
    t('Block'),
  );
  $views[$view->name] = $view;

  /*
   * View 'Visitor log'
   */
  $view = new view();
  $view->name = 'visitor_log';
  $view->description = 'Shows IP and street addresses of recent visitors with visit counts and maps.';
  $view->tag = 'IP Geolocation Views and Maps';
  $view->base_table = 'accesslog';
  $view->human_name = 'Visitor log';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = TRUE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Visitor log';
  $handler->display->display_options['group_by'] = TRUE;
  $handler->display->display_options['access']['type'] = 'role';
  $handler->display->display_options['access']['role'] = array(
    2 => '2',
  );
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['sort_asc_label'] = 'Chronological';
  $handler->display->display_options['exposed_form']['options']['sort_desc_label'] = 'Reverse chronological';
  $handler->display->display_options['exposed_form']['options']['autosubmit'] = 0;
  $handler->display->display_options['exposed_form']['options']['autosubmit_hide'] = 1;
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['expose']['items_per_page_options_all'] = 0;
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['grouping'] = 'country';
  $handler->display->display_options['style_options']['columns'] = array(
    'hostname_1' => 'hostname_1',
    'hostname' => 'hostname',
    'timestamp_1' => 'timestamp_1',
    'timestamp' => 'timestamp',
    'latitude' => 'latitude',
    'longitude' => 'longitude',
    'formatted_address' => 'formatted_address',
    'php' => 'php',
    'country' => 'country',
  );
  $handler->display->display_options['style_options']['default'] = 'timestamp';
  $handler->display->display_options['style_options']['info'] = array(
    'hostname_1' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
    ),
    'hostname' => array(
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
    ),
    'timestamp_1' => array(
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
    ),
    'timestamp' => array(
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
    ),
    'latitude' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
    ),
    'longitude' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
    ),
    'formatted_address' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
    ),
    'php' => array(
      'align' => '',
      'separator' => '',
    ),
    'country' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
    ),
  );
  $handler->display->display_options['style_options']['override'] = 1;
  $handler->display->display_options['style_options']['sticky'] = 0;
  $handler->display->display_options['style_options']['order'] = 'desc';
  $handler->display->display_options['style_options']['empty_table'] = 0;
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['label'] = 'No results text';
  $handler->display->display_options['empty']['area']['empty'] = FALSE;
  $handler->display->display_options['empty']['area']['content'] = '    No IP geolocation data found.

            Either the core <strong>Statistics</strong> module is not enabled or there is no data in the <strong>access log</strong>.
            Please visit the <a href="/admin/config/system/ip_geoloc">Configuration >> IP Geolocation</a> page for more diagnostics.';
  $handler->display->display_options['empty']['area']['format'] = 'full_html';
  $handler->display->display_options['empty']['area']['tokenize'] = 0;
  /* Field: Access log: Hostname */
  $handler->display->display_options['fields']['hostname_1']['id'] = 'hostname_1';
  $handler->display->display_options['fields']['hostname_1']['table'] = 'accesslog';
  $handler->display->display_options['fields']['hostname_1']['field'] = 'hostname';
  $handler->display->display_options['fields']['hostname_1']['label'] = 'IP address';
  $handler->display->display_options['fields']['hostname_1']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['hostname_1']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['hostname_1']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['hostname_1']['alter']['external'] = 0;
  $handler->display->display_options['fields']['hostname_1']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['hostname_1']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['hostname_1']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['hostname_1']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['hostname_1']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['hostname_1']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['hostname_1']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['hostname_1']['alter']['html'] = 0;
  $handler->display->display_options['fields']['hostname_1']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['hostname_1']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['hostname_1']['hide_empty'] = 0;
  $handler->display->display_options['fields']['hostname_1']['empty_zero'] = 0;
  $handler->display->display_options['fields']['hostname_1']['hide_alter_empty'] = 0;
  /* Field: COUNT(Access log: Hostname) */
  $handler->display->display_options['fields']['hostname']['id'] = 'hostname';
  $handler->display->display_options['fields']['hostname']['table'] = 'accesslog';
  $handler->display->display_options['fields']['hostname']['field'] = 'hostname';
  $handler->display->display_options['fields']['hostname']['group_type'] = 'count';
  $handler->display->display_options['fields']['hostname']['label'] = 'No. visits';
  $handler->display->display_options['fields']['hostname']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['hostname']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['hostname']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['hostname']['alter']['external'] = 0;
  $handler->display->display_options['fields']['hostname']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['hostname']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['hostname']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['hostname']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['hostname']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['hostname']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['hostname']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['hostname']['alter']['html'] = 0;
  $handler->display->display_options['fields']['hostname']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['hostname']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['hostname']['hide_empty'] = 0;
  $handler->display->display_options['fields']['hostname']['empty_zero'] = 0;
  $handler->display->display_options['fields']['hostname']['hide_alter_empty'] = 0;
  /* Field: MIN(Access log: Timestamp) */
  $handler->display->display_options['fields']['timestamp_1']['id'] = 'timestamp_1';
  $handler->display->display_options['fields']['timestamp_1']['table'] = 'accesslog';
  $handler->display->display_options['fields']['timestamp_1']['field'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp_1']['group_type'] = 'min';
  $handler->display->display_options['fields']['timestamp_1']['label'] = 'First visited';
  $handler->display->display_options['fields']['timestamp_1']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['timestamp_1']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['timestamp_1']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['timestamp_1']['alter']['external'] = 0;
  $handler->display->display_options['fields']['timestamp_1']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['timestamp_1']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['timestamp_1']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['timestamp_1']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['timestamp_1']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['timestamp_1']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['timestamp_1']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['timestamp_1']['alter']['html'] = 0;
  $handler->display->display_options['fields']['timestamp_1']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['timestamp_1']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['timestamp_1']['hide_empty'] = 0;
  $handler->display->display_options['fields']['timestamp_1']['empty_zero'] = 0;
  $handler->display->display_options['fields']['timestamp_1']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['timestamp_1']['date_format'] = 'medium';
  /* Field: MAX(Access log: Timestamp) */
  $handler->display->display_options['fields']['timestamp']['id'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['table'] = 'accesslog';
  $handler->display->display_options['fields']['timestamp']['field'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['group_type'] = 'max';
  $handler->display->display_options['fields']['timestamp']['label'] = 'Last visited';
  $handler->display->display_options['fields']['timestamp']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['external'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['timestamp']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['timestamp']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['html'] = 0;
  $handler->display->display_options['fields']['timestamp']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['timestamp']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['timestamp']['hide_empty'] = 0;
  $handler->display->display_options['fields']['timestamp']['empty_zero'] = 0;
  $handler->display->display_options['fields']['timestamp']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['timestamp']['date_format'] = 'medium';
  /* Field: IP geolocations: Latitude */
  $handler->display->display_options['fields']['latitude']['id'] = 'latitude';
  $handler->display->display_options['fields']['latitude']['table'] = 'ip_geoloc';
  $handler->display->display_options['fields']['latitude']['field'] = 'latitude';
  $handler->display->display_options['fields']['latitude']['exclude'] = TRUE;
  $handler->display->display_options['fields']['latitude']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['latitude']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['latitude']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['latitude']['alter']['external'] = 0;
  $handler->display->display_options['fields']['latitude']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['latitude']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['latitude']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['latitude']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['latitude']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['latitude']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['latitude']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['latitude']['alter']['html'] = 0;
  $handler->display->display_options['fields']['latitude']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['latitude']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['latitude']['hide_empty'] = 0;
  $handler->display->display_options['fields']['latitude']['empty_zero'] = 0;
  $handler->display->display_options['fields']['latitude']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['latitude']['format_plural'] = 0;
  /* Field: IP geolocations: Longitude */
  $handler->display->display_options['fields']['longitude']['id'] = 'longitude';
  $handler->display->display_options['fields']['longitude']['table'] = 'ip_geoloc';
  $handler->display->display_options['fields']['longitude']['field'] = 'longitude';
  $handler->display->display_options['fields']['longitude']['exclude'] = TRUE;
  $handler->display->display_options['fields']['longitude']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['longitude']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['longitude']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['longitude']['alter']['external'] = 0;
  $handler->display->display_options['fields']['longitude']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['longitude']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['longitude']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['longitude']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['longitude']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['longitude']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['longitude']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['longitude']['alter']['html'] = 0;
  $handler->display->display_options['fields']['longitude']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['longitude']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['longitude']['hide_empty'] = 0;
  $handler->display->display_options['fields']['longitude']['empty_zero'] = 0;
  $handler->display->display_options['fields']['longitude']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['longitude']['format_plural'] = 0;
  /* Field: IP geolocations: Address */
  $handler->display->display_options['fields']['formatted_address']['id'] = 'formatted_address';
  $handler->display->display_options['fields']['formatted_address']['table'] = 'ip_geoloc';
  $handler->display->display_options['fields']['formatted_address']['field'] = 'formatted_address';
  $handler->display->display_options['fields']['formatted_address']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['external'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['formatted_address']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['formatted_address']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['html'] = 0;
  $handler->display->display_options['fields']['formatted_address']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['formatted_address']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['formatted_address']['hide_empty'] = 0;
  $handler->display->display_options['fields']['formatted_address']['empty_zero'] = 0;
  $handler->display->display_options['fields']['formatted_address']['hide_alter_empty'] = 0;
  /* Field: IP geolocations: Country */
  $handler->display->display_options['fields']['country']['id'] = 'country';
  $handler->display->display_options['fields']['country']['table'] = 'ip_geoloc';
  $handler->display->display_options['fields']['country']['field'] = 'country';
  $handler->display->display_options['fields']['country']['exclude'] = TRUE;
  $handler->display->display_options['fields']['country']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['country']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['country']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['country']['alter']['external'] = 0;
  $handler->display->display_options['fields']['country']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['country']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['country']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['country']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['country']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['country']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['country']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['country']['alter']['html'] = 0;
  $handler->display->display_options['fields']['country']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['country']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['country']['hide_empty'] = 0;
  $handler->display->display_options['fields']['country']['empty_zero'] = 0;
  $handler->display->display_options['fields']['country']['hide_alter_empty'] = 0;
  /* Field: Global: PHP */
  $handler->display->display_options['fields']['php']['id'] = 'php';
  $handler->display->display_options['fields']['php']['table'] = 'views';
  $handler->display->display_options['fields']['php']['field'] = 'php';
  $handler->display->display_options['fields']['php']['label'] = 'Map';
  $handler->display->display_options['fields']['php']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['php']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['php']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['php']['alter']['external'] = 0;
  $handler->display->display_options['fields']['php']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['php']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['php']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['php']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['php']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['php']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['php']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['php']['alter']['html'] = 0;
  $handler->display->display_options['fields']['php']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['php']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['php']['hide_empty'] = 0;
  $handler->display->display_options['fields']['php']['empty_zero'] = 0;
  $handler->display->display_options['fields']['php']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['php']['use_php_setup'] = 0;
  $handler->display->display_options['fields']['php']['php_output'] = '<?php
            echo ip_geoloc_output_map($row->latitude, $row->longitude, \'ip-geoloc-map-\' . $row->hostname_1, \'height: 150px; width: 250px\', $row->formatted_address) ;
          ?>';
  $handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
  $handler->display->display_options['fields']['php']['php_click_sortable'] = '';
  /* Filter criterion: Date range */
  $handler->display->display_options['filters']['timestamp']['id'] = 'timestamp';
  $handler->display->display_options['filters']['timestamp']['table'] = 'accesslog';
  $handler->display->display_options['filters']['timestamp']['field'] = 'timestamp';
  $handler->display->display_options['filters']['timestamp']['ui_name'] = 'Date range';
  $handler->display->display_options['filters']['timestamp']['operator'] = 'between';
  $handler->display->display_options['filters']['timestamp']['value']['min'] = '-6 months';
  $handler->display->display_options['filters']['timestamp']['value']['max'] = 'now';
  $handler->display->display_options['filters']['timestamp']['value']['type'] = 'offset';
  $handler->display->display_options['filters']['timestamp']['exposed'] = TRUE;
  $handler->display->display_options['filters']['timestamp']['expose']['operator_id'] = 'timestamp_op';
  $handler->display->display_options['filters']['timestamp']['expose']['label'] = 'Date range between';
  $handler->display->display_options['filters']['timestamp']['expose']['operator'] = 'timestamp_op';
  $handler->display->display_options['filters']['timestamp']['expose']['identifier'] = 'timestamp';
  $handler->display->display_options['filters']['timestamp']['expose']['remember'] = 1;
  $handler->display->display_options['filters']['timestamp']['expose']['multiple'] = FALSE;

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'visitor-log';
  $handler->display->display_options['menu']['title'] = 'Visitor log';
  $handler->display->display_options['menu']['description'] = 'Shows IP and street addresses of recent visitors with visit counts and maps.';
  $handler->display->display_options['menu']['weight'] = '0';

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block_1');
  $handler->display->display_options['block_description'] = 'Visitor log';
  $translatables['visitor_log'] = array(
    t('Master'),
    t('Visitor log'),
    t('more'),
    t('Apply'),
    t('Reset'),
    t('Sort by'),
    t('Chronological'),
    t('Reverse chronological'),
    t('Items per page'),
    t('- All -'),
    t('Offset'),
    t('No results text'),
    t('    No IP geolocation data found.

            Either the core <strong>Statistics</strong> module is not enabled or there is no data in the <strong>access log</strong>.
            Please visit the <a href="/admin/config/system/ip_geoloc">Configuration >> IP Geolocation</a> page for more diagnostics.'),
    t('IP address'),
    t('No. visits'),
    t('First visited'),
    t('Last visited'),
    t('Latitude'),
    t('.'),
    t(','),
    t('Longitude'),
    t('Address'),
    t('Country'),
    t('Map'),
    t('Date range between'),
    t('Page'),
    t('Block'),
  );
  $views[$view->name] = $view;

  /*
   * Visitor-log (lite)
   */
  $view = new view();
  $view->name = 'visitor_log_lite';
  $view->description = 'Shows IP and street addresses of recent visitors.';
  $view->tag = 'IP Geolocation Views and Maps';
  $view->base_table = 'accesslog';
  $view->human_name = 'Visitor log (lite)';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = TRUE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Visitor log';
  $handler->display->display_options['group_by'] = TRUE;
  $handler->display->display_options['access']['type'] = 'role';
  $handler->display->display_options['access']['role'] = array(
    2 => '2',
  );
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '20';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'hostname_1' => 'hostname_1',
    'timestamp' => 'timestamp',
    'formatted_address' => 'formatted_address',
  );
  $handler->display->display_options['style_options']['default'] = 'timestamp';
  $handler->display->display_options['style_options']['info'] = array(
    'hostname_1' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
    ),
    'timestamp' => array(
      'sortable' => 0,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
    ),
    'formatted_address' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
    ),
  );
  $handler->display->display_options['style_options']['override'] = 1;
  $handler->display->display_options['style_options']['sticky'] = 0;
  $handler->display->display_options['style_options']['order'] = 'desc';
  $handler->display->display_options['style_options']['empty_table'] = 0;
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['empty'] = FALSE;
  $handler->display->display_options['empty']['area']['content'] = ' No IP geolocation data found.

          Either the core <strong>Statistics</strong> module is not enabled or there is no data in the <strong>access log</strong>.
          Please visit the <a href="/admin/config/system/ip_geoloc">Configuration >> IP Geolocation</a> page for more diagnostics.';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';
  $handler->display->display_options['empty']['area']['tokenize'] = 0;
  /* Field: Global: View result counter */
  $handler->display->display_options['fields']['counter']['id'] = 'counter';
  $handler->display->display_options['fields']['counter']['table'] = 'views';
  $handler->display->display_options['fields']['counter']['field'] = 'counter';
  $handler->display->display_options['fields']['counter']['label'] = '#';
  $handler->display->display_options['fields']['counter']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['counter']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['counter']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['counter']['alter']['external'] = 0;
  $handler->display->display_options['fields']['counter']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['counter']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['counter']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['counter']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['counter']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['counter']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['counter']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['counter']['alter']['html'] = 0;
  $handler->display->display_options['fields']['counter']['element_label_colon'] = 0;
  $handler->display->display_options['fields']['counter']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['counter']['hide_empty'] = 0;
  $handler->display->display_options['fields']['counter']['empty_zero'] = 0;
  $handler->display->display_options['fields']['counter']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['counter']['counter_start'] = '1';
  /* Field: Access log: Hostname */
  $handler->display->display_options['fields']['hostname_1']['id'] = 'hostname_1';
  $handler->display->display_options['fields']['hostname_1']['table'] = 'accesslog';
  $handler->display->display_options['fields']['hostname_1']['field'] = 'hostname';
  $handler->display->display_options['fields']['hostname_1']['label'] = 'IP address';
  $handler->display->display_options['fields']['hostname_1']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['hostname_1']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['hostname_1']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['hostname_1']['alter']['external'] = 0;
  $handler->display->display_options['fields']['hostname_1']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['hostname_1']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['hostname_1']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['hostname_1']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['hostname_1']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['hostname_1']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['hostname_1']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['hostname_1']['alter']['html'] = 0;
  $handler->display->display_options['fields']['hostname_1']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['hostname_1']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['hostname_1']['hide_empty'] = 0;
  $handler->display->display_options['fields']['hostname_1']['empty_zero'] = 0;
  $handler->display->display_options['fields']['hostname_1']['hide_alter_empty'] = 0;
  /* Field: MAX(Access log: Timestamp) */
  $handler->display->display_options['fields']['timestamp']['id'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['table'] = 'accesslog';
  $handler->display->display_options['fields']['timestamp']['field'] = 'timestamp';
  $handler->display->display_options['fields']['timestamp']['group_type'] = 'max';
  $handler->display->display_options['fields']['timestamp']['label'] = 'Last visited';
  $handler->display->display_options['fields']['timestamp']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['external'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['timestamp']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['timestamp']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['timestamp']['alter']['html'] = 0;
  $handler->display->display_options['fields']['timestamp']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['timestamp']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['timestamp']['hide_empty'] = 0;
  $handler->display->display_options['fields']['timestamp']['empty_zero'] = 0;
  $handler->display->display_options['fields']['timestamp']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['timestamp']['date_format'] = 'medium';
  /* Field: IP geolocations: Address */
  $handler->display->display_options['fields']['formatted_address']['id'] = 'formatted_address';
  $handler->display->display_options['fields']['formatted_address']['table'] = 'ip_geoloc';
  $handler->display->display_options['fields']['formatted_address']['field'] = 'formatted_address';
  $handler->display->display_options['fields']['formatted_address']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['external'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['formatted_address']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['formatted_address']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['html'] = 0;
  $handler->display->display_options['fields']['formatted_address']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['formatted_address']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['formatted_address']['hide_empty'] = 0;
  $handler->display->display_options['fields']['formatted_address']['empty_zero'] = 0;
  $handler->display->display_options['fields']['formatted_address']['hide_alter_empty'] = 0;

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'visitor-log-lite';
  $handler->display->display_options['menu']['title'] = 'Visitor log';
  $handler->display->display_options['menu']['description'] = 'Shows IP and street addresses of recent visitors.';
  $handler->display->display_options['menu']['weight'] = '0';

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block_1');
  $handler->display->display_options['block_description'] = 'Visitor log (lite)';

  $views[$view->name] = $view;

  /* View Order Locations formatted as a map with marker balloons. */
  $view = new view();
  $view->name = 'order_locations';
  $view->description = '';
  $view->tag = 'IP Geolocation Views and Maps';
  $view->base_table = 'commerce_order';
  $view->human_name = 'Order Locations';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = TRUE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Order Locations';
  $handler->display->display_options['access']['type'] = 'role';
  $handler->display->display_options['access']['role'] = array(
    2 => '2',
  );
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'ip_geoloc_map';
  /* Field: IP geolocations: IP address */
  $handler->display->display_options['fields']['ip_address']['id'] = 'ip_address';
  $handler->display->display_options['fields']['ip_address']['table'] = 'ip_geoloc';
  $handler->display->display_options['fields']['ip_address']['field'] = 'ip_address';
  $handler->display->display_options['fields']['ip_address']['alter']['alter_text'] = 1;
  $handler->display->display_options['fields']['ip_address']['alter']['text'] = 'IP: [ip_address]';
  $handler->display->display_options['fields']['ip_address']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['ip_address']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['ip_address']['alter']['external'] = 0;
  $handler->display->display_options['fields']['ip_address']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['ip_address']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['ip_address']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['ip_address']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['ip_address']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['ip_address']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['ip_address']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['ip_address']['alter']['html'] = 0;
  $handler->display->display_options['fields']['ip_address']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['ip_address']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['ip_address']['hide_empty'] = 0;
  $handler->display->display_options['fields']['ip_address']['empty_zero'] = 0;
  $handler->display->display_options['fields']['ip_address']['hide_alter_empty'] = 1;
  /* Field: Commerce Order: Order number */
  $handler->display->display_options['fields']['order_number']['id'] = 'order_number';
  $handler->display->display_options['fields']['order_number']['table'] = 'commerce_order';
  $handler->display->display_options['fields']['order_number']['field'] = 'order_number';
  $handler->display->display_options['fields']['order_number']['alter']['alter_text'] = 1;
  $handler->display->display_options['fields']['order_number']['alter']['text'] = 'Order #[order_number]';
  $handler->display->display_options['fields']['order_number']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['order_number']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['order_number']['alter']['external'] = 0;
  $handler->display->display_options['fields']['order_number']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['order_number']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['order_number']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['order_number']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['order_number']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['order_number']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['order_number']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['order_number']['alter']['html'] = 0;
  $handler->display->display_options['fields']['order_number']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['order_number']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['order_number']['hide_empty'] = 0;
  $handler->display->display_options['fields']['order_number']['empty_zero'] = 0;
  $handler->display->display_options['fields']['order_number']['hide_alter_empty'] = 1;
  /* Field: Commerce Order: Created date */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'commerce_order';
  $handler->display->display_options['fields']['created']['field'] = 'created';
  $handler->display->display_options['fields']['created']['alter']['alter_text'] = 1;
  $handler->display->display_options['fields']['created']['alter']['text'] = 'Order placed: [created]';
  $handler->display->display_options['fields']['created']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['created']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['created']['alter']['external'] = 0;
  $handler->display->display_options['fields']['created']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['created']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['created']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['created']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['created']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['created']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['created']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['created']['alter']['html'] = 0;
  $handler->display->display_options['fields']['created']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['created']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['created']['hide_empty'] = 0;
  $handler->display->display_options['fields']['created']['empty_zero'] = 0;
  $handler->display->display_options['fields']['created']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['created']['date_format'] = 'medium';
  /* Field: Commerce Order: Order state */
  $handler->display->display_options['fields']['state']['id'] = 'state';
  $handler->display->display_options['fields']['state']['table'] = 'commerce_order';
  $handler->display->display_options['fields']['state']['field'] = 'state';
  $handler->display->display_options['fields']['state']['alter']['alter_text'] = 1;
  $handler->display->display_options['fields']['state']['alter']['text'] = 'Order state: [state]';
  $handler->display->display_options['fields']['state']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['state']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['state']['alter']['external'] = 0;
  $handler->display->display_options['fields']['state']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['state']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['state']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['state']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['state']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['state']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['state']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['state']['alter']['html'] = 0;
  $handler->display->display_options['fields']['state']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['state']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['state']['hide_empty'] = 0;
  $handler->display->display_options['fields']['state']['empty_zero'] = 0;
  $handler->display->display_options['fields']['state']['hide_alter_empty'] = 1;
  /* Field: IP geolocations: Address */
  $handler->display->display_options['fields']['formatted_address']['id'] = 'formatted_address';
  $handler->display->display_options['fields']['formatted_address']['table'] = 'ip_geoloc';
  $handler->display->display_options['fields']['formatted_address']['field'] = 'formatted_address';
  $handler->display->display_options['fields']['formatted_address']['alter']['alter_text'] = 1;
  $handler->display->display_options['fields']['formatted_address']['alter']['text'] = 'Address:
    [formatted_address]';
  $handler->display->display_options['fields']['formatted_address']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['external'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['formatted_address']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['formatted_address']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['formatted_address']['alter']['html'] = 0;
  $handler->display->display_options['fields']['formatted_address']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['formatted_address']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['formatted_address']['hide_empty'] = 0;
  $handler->display->display_options['fields']['formatted_address']['empty_zero'] = 0;
  $handler->display->display_options['fields']['formatted_address']['hide_alter_empty'] = 1;
  /* Field: IP geolocations: Latitude */
  $handler->display->display_options['fields']['latitude']['id'] = 'latitude';
  $handler->display->display_options['fields']['latitude']['table'] = 'ip_geoloc';
  $handler->display->display_options['fields']['latitude']['field'] = 'latitude';
  $handler->display->display_options['fields']['latitude']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['latitude']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['latitude']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['latitude']['alter']['external'] = 0;
  $handler->display->display_options['fields']['latitude']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['latitude']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['latitude']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['latitude']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['latitude']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['latitude']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['latitude']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['latitude']['alter']['html'] = 0;
  $handler->display->display_options['fields']['latitude']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['latitude']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['latitude']['hide_empty'] = 0;
  $handler->display->display_options['fields']['latitude']['empty_zero'] = 0;
  $handler->display->display_options['fields']['latitude']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['latitude']['format_plural'] = 0;
  /* Field: IP geolocations: Longitude */
  $handler->display->display_options['fields']['longitude']['id'] = 'longitude';
  $handler->display->display_options['fields']['longitude']['table'] = 'ip_geoloc';
  $handler->display->display_options['fields']['longitude']['field'] = 'longitude';
  $handler->display->display_options['fields']['longitude']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['longitude']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['longitude']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['longitude']['alter']['external'] = 0;
  $handler->display->display_options['fields']['longitude']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['longitude']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['longitude']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['longitude']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['longitude']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['longitude']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['longitude']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['longitude']['alter']['html'] = 0;
  $handler->display->display_options['fields']['longitude']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['longitude']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['longitude']['hide_empty'] = 0;
  $handler->display->display_options['fields']['longitude']['empty_zero'] = 0;
  $handler->display->display_options['fields']['longitude']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['longitude']['format_plural'] = 0;
  /* Sort criterion: Commerce Order: Order ID */
  $handler->display->display_options['sorts']['order_id']['id'] = 'order_id';
  $handler->display->display_options['sorts']['order_id']['table'] = 'commerce_order';
  $handler->display->display_options['sorts']['order_id']['field'] = 'order_id';
  $handler->display->display_options['sorts']['order_id']['order'] = 'DESC';
  /* Filter criterion: Commerce Order: Order state */
  $handler->display->display_options['filters']['state']['id'] = 'state';
  $handler->display->display_options['filters']['state']['table'] = 'commerce_order';
  $handler->display->display_options['filters']['state']['field'] = 'state';
  $handler->display->display_options['filters']['state']['value'] = array(
    'canceled' => 'canceled',
    'cart' => 'cart',
    'checkout' => 'checkout',
    'pending' => 'pending',
    'completed' => 'completed',
  );
  $handler->display->display_options['filters']['state']['exposed'] = TRUE;
  $handler->display->display_options['filters']['state']['expose']['operator_id'] = 'state_op';
  $handler->display->display_options['filters']['state']['expose']['label'] = 'Order state';
  $handler->display->display_options['filters']['state']['expose']['operator'] = 'state_op';
  $handler->display->display_options['filters']['state']['expose']['identifier'] = 'state';
  $handler->display->display_options['filters']['state']['expose']['reduce'] = 0;
  /* Filter criterion: IP geolocations: Country */
  $handler->display->display_options['filters']['country']['id'] = 'country';
  $handler->display->display_options['filters']['country']['table'] = 'ip_geoloc';
  $handler->display->display_options['filters']['country']['field'] = 'country';
  $handler->display->display_options['filters']['country']['exposed'] = TRUE;
  $handler->display->display_options['filters']['country']['expose']['operator_id'] = 'country_op';
  $handler->display->display_options['filters']['country']['expose']['label'] = 'Country';
  $handler->display->display_options['filters']['country']['expose']['operator'] = 'country_op';
  $handler->display->display_options['filters']['country']['expose']['identifier'] = 'country';
  $handler->display->display_options['filters']['country']['expose']['required'] = 0;
  $handler->display->display_options['filters']['country']['expose']['multiple'] = FALSE;
  /* Filter criterion: Commerce Order: Created date */
  $handler->display->display_options['filters']['created']['id'] = 'created';
  $handler->display->display_options['filters']['created']['table'] = 'commerce_order';
  $handler->display->display_options['filters']['created']['field'] = 'created';
  $handler->display->display_options['filters']['created']['operator'] = 'between';
  $handler->display->display_options['filters']['created']['value']['type'] = 'offset';
  $handler->display->display_options['filters']['created']['exposed'] = TRUE;
  $handler->display->display_options['filters']['created']['expose']['operator_id'] = 'created_op';
  $handler->display->display_options['filters']['created']['expose']['label'] = 'Order placed between';
  $handler->display->display_options['filters']['created']['expose']['operator'] = 'created_op';
  $handler->display->display_options['filters']['created']['expose']['identifier'] = 'created';
  $handler->display->display_options['filters']['created']['expose']['multiple'] = FALSE;

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'order-locations';

  $views[$view->name] = $view;

  return $views;
}
