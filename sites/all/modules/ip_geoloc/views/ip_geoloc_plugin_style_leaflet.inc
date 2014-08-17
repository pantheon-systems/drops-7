<?php

/**
 * @file
 * ip_geoloc_plugin_style_leaflet.inc
 *
 * Views Style plugin extension for Leaflet (if enabled).
 */

require_once 'ip_geoloc_plugin_style.inc';

define('LEAFLET_MARKERCLUSTER_EXCLUDE_FROM_CLUSTER', 0x01);

class ip_geoloc_plugin_style_leaflet extends views_plugin_style {

  /**
   * Set default Leaflet options.
   */
  public function option_definition() {
    $options = parent::option_definition();

    $latitude  = module_exists('location') ? 'location_latitude' : 'ip_geoloc_latitude';
    // For field-based modules.
    $longitude = module_exists('location') ? 'location_longitude' : ($latitude == 'ip_geoloc_latitude' ? 'ip_geoloc_longitude' : $latitude);
    $options['ip_geoloc_views_plugin_latitude'] = array('default' => $latitude);
    $options['ip_geoloc_views_plugin_longitude'] = array('default' => $longitude);

    $options['differentiator'] = array(
      'contains' => array(
        'differentiator_field' => array('default' => ''),
      ),
    );
    $options['default_marker_color'] = array('default' => '');
    $options['default_marker_special_char'] = array('default' => '');
    $options['center_option'] = array('default' => 0);
    $options['empty_map_center'] = array('default' => '');
    $options['visitor_marker_color'] = array('default' => 'none');
    // The leaflet.module default.
    $options['map'] = array('default' => 'OSM Mapnik');
    $options['map_height'] = array('default' => 300);
    $options['cluster_radius'] = array('default' => module_exists('leaflet_markercluster') ? 80 : '');
    $options['tooltips'] = array(
      'contains' => array(
        'marker_tooltip' => array('default' => ''),
      ),
    );
    $options['tags'] = array(
      'contains' => array(
        'marker_tag' => array('default' => ''),
        'tag_css_class' => array('default' => 'tag-inside-marker'),
      ),
    );
    $options['map_options'] = array(
      'contains' => array(
        'maxzoom' => array('default' => 18),
        'zoom' => array('default' => 2),
        'scrollwheelzoom' => array('default' => TRUE),
        'dragging' => array('default' => TRUE),
      ),
    );
    return $options;
  }

  /**
   * Implements options_form().
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $maps = array();
    foreach (ip_geoloc_plugin_style_leaflet_map_get_info() as $key => $map) {
      $maps[$key] = $map['label'];
    }
    $form['map'] = array(
      '#title' => t('Map'),
      '#type' => 'select',
      '#options' => $maps,
      '#default_value' => $this->options['map'],
      '#required' => TRUE,
      '#weight' => -2,
    );
    $form['map_height'] = array(
      '#title' => t('Map height'),
      '#type' => 'textfield',
      '#field_suffix' => t('px'),
      '#size' => 4,
      '#default_value' => $this->options['map_height'],
      '#description' => t('The default produces a map of 300 pixels high with a width extending to its bounding container.'),
      '#weight' => -1,
    );

    $form_state['renderer'] = 'leaflet';
    ip_geoloc_plugin_style_bulk_of_form($this, $form, $form_state);

    $desc = t('<strong>Leaflet</strong> only: optionally enter a special character representing an icon to be superimposed on the marker image. You may copy and paste from <a target="_font_awesome" href="!url1">Font Awesome</a> (requires installation, see README) or any other source, such as <a target="_fsymbols" href="!url2">fsymbols</a> (no installation required). Copy and paste either the font icon or the unicode, like <strong>&amp;#xf015;</strong>', array(
      '!url1' => url('http://fortawesome.github.io/Font-Awesome/cheatsheet'),
      '!url2' => url('http://fsymbols.com'),
    ));
    $path = drupal_get_path('module', 'ip_geoloc');
    if (!ip_geoloc_get_ccs_library_path('font-awesome')) {
      $desc .= ' ' . t('Please follow instructions in the <a href="@README">README</a> to install the <em>Font Awesome</em> library.', array(
        '@README' => url("$path/README.txt"),
      ));
    }
    $form['default_marker_special_char'] = array(
      '#title' => t('Default location marker special character'),
      '#type' => 'textfield',
      '#size' => 8,
      '#default_value' => $this->options['default_marker_special_char'],
      '#description' => $desc,
      '#weight' => 22,
    );

    $form['center_option']['#options'][0] = t('Auto-box to fit all markers (include visitor marker if color <strong>not</strong> set to &lt;none&gt;)');
    unset($form['center_option']['#description']);

    $fields = ip_geoloc_get_display_fields($this->display->handler, FALSE);
    $form['tooltips'] = array(
      '#title' => t('Marker tooltips'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => empty($this->options['tooltips']['marker_tooltip']),
      '#description' => t('In addition to its balloon each marker may have a tooltip. A tooltip is a short text that appears when visitors hover their mouse over a marker.'),
      '#weight' => 60,
    );
    $form['tooltips']['marker_tooltip'] = array(
      '#title' => t('Views field to populate tooltips'),
      '#type' => 'select',
      '#default_value' => $this->options['tooltips']['marker_tooltip'],
      '#options' => $fields,
      '#description' => t('Example: "Content: Title"'),
    );

    $form['tags'] = array(
      '#title' => t('Marker tags'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => empty($this->options['tags']['marker_tag']),
      '#description' => t('Each marker may have a tag. A tag is a number or short text shown permanently above, below or inside the marker.'),
      '#weight' => 70,
    );
    $form['tags']['marker_tag'] = array(
      '#title' => t('Views field to populate tags'),
      '#type' => 'select',
      '#default_value' => $this->options['tags']['marker_tag'],
      '#options' => $fields,
      '#description' => t('Example: "Content: Title". Use "Global: View result counter" if you want to number your locations.'),
    );
    $form['tags']['tag_css_class'] = array(
      '#title' => t('Tag position and style'),
      '#type' => 'textfield',
      '#default_value' => $this->options['tags']['tag_css_class'],
      '#description' => t('The CSS class or classes applied to each tag. Tagged marker CSS classes coming with this module are <strong>tag-above-marker</strong>, <strong>tag-below-marker</strong> and <strong>tag-inside-marker</strong>. If you opted to have <em>no markers</em>, i.e. tags only, you may use <strong>tag-rounded-corners</strong> or <strong>tag-pointy-circle</strong>, which is recommended for numbers. You may also create your own CSS classes and use them here.'),
    );
    $selected_map = ip_geoloc_plugin_style_leaflet_map_get_info($this->options['map']);
    $zoom_top = 18;
    if (isset($selected_map['settings']['maxZoom'])) {
      $zoom_top = $selected_map['settings']['maxZoom'];
    }
    $form['map_options'] = array(
      '#title' => t('More map options'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      // Could be empty($this->options['map_options']['zoom']), ?
      '#collapsed' => TRUE,
      '#weight' => 200,
    );
    $form['map_options']['maxzoom'] = array(
      '#title' => t('Maximum zoom level (0..@zoomtop)', array('@zoomtop' => $zoom_top)),
      '#type' => 'textfield',
      '#size' => 2,
      '#default_value' => $this->options['map_options']['maxzoom'],
      '#description' => t('Note that not all maps support all zoom levels.'),
    );
    $initial_zoom_max = $zoom_top;
    if (is_numeric($this->options['map_options']['maxzoom'])) {
      $initial_zoom_max = min($zoom_top, $this->options['map_options']['maxzoom']);
    }
    $form['map_options']['zoom'] = array(
      '#title' => t('Initial zoom level (0..@maxzoom)', array('@maxzoom' => $initial_zoom_max)),
      '#type' => 'textfield',
      '#size' => 2,
      '#default_value' => $this->options['map_options']['zoom'],
      '#description' => t('Does not apply to auto-box centering except when only one or no markers are shown.'),
    );
    $form['map_options']['scrollwheelzoom'] = array(
      '#title' => t('Enable scroll wheel zoom'),
      '#type' => 'select',
      '#default_value' => $this->options['map_options']['scrollwheelzoom'],
      '#options' => array(TRUE => t('Yes'), FALSE => t('No')),
    );
    $form['map_options']['dragging'] = array(
      '#title' => t('Dragging/Panning of the map'),
      '#type' => 'select',
      '#default_value' => $this->options['map_options']['dragging'],
      '#options' => array(TRUE => t('Yes'), FALSE => t('No')),
    );

    $visitor_marker_colors = array(
      'none' => '<' . t('none') . '>') +
      ip_geoloc_marker_colors();
    unset($visitor_marker_colors['0']);

    $css_file = strpos(variable_get('ip_geoloc_marker_directory'), 'amarkers') ? 'ip_geoloc_admin_a.css' : 'ip_geoloc_admin.css';

    $form['visitor_marker_color'] = array(
      '#title' => t('Visitor marker'),
      '#type' => 'select',
      '#multiple' => FALSE,
      '#default_value' => $this->options['visitor_marker_color'],
      '#options' => $visitor_marker_colors,
      '#attributes' => array('class' => array('marker-color')),
      '#attached' => array('css' => array("$path/css/$css_file")),
      '#weight' => 90,
    );

    $form['cluster_radius'] = array(
      '#title' => t('Marker cluster radius'),
      '#type' => 'textfield',
      '#field_suffix' => t('px'),
      '#size' => 4,
      '#default_value' => $this->options['cluster_radius'],
      '#description' => module_exists('leaflet_markercluster') ? t('Leave empty to disable clustering.') : t('Requires the <a href="!url">Leaflet MarkerCluster</a> module and corresponding javascript library.', array(
        '!url' => url('http://drupal.org/project/leaflet_markercluster'),
      )),
      '#weight' => 100,
    );
  }

  /**
   * Validate the options form.
   */
  public function options_validate(&$form, &$form_state) {
    ip_geoloc_plugin_style_bulk_of_form_validate($form, $form_state);
    $map_height = $form_state['values']['style_options']['map_height'];
    if (!is_numeric($map_height) || $map_height <= 0) {
      form_error($form['map_height'], t('Map height cannot be blank and must be a positive number.'));
    }
    $selected_map = ip_geoloc_plugin_style_leaflet_map_get_info($form_state['values']['style_options']['map']);
    $zoom_top = 18;
    if (isset($selected_map['settings']['maxZoom'])) {
      $zoom_top = $selected_map['settings']['maxZoom'];
    }
    $max_zoom = $form_state['values']['style_options']['map_options']['maxzoom'];
    if ($max_zoom != '' && (!is_numeric($max_zoom) || $max_zoom < 0 || $max_zoom > $zoom_top)) {
      form_error($form['map_options']['maxzoom'], t('Maximum zoom level for %map must be in range 0..@zoomtop', array(
        '%map' => $selected_map['label'], '@zoomtop' => $zoom_top)));
    }
    $zoom = $form_state['values']['style_options']['map_options']['zoom'];
    if ($zoom != '' && (!is_numeric($zoom) || $zoom < 0 || $zoom > $max_zoom)) {
      form_error($form['map_options']['zoom'], t('Initial zoom level must be a non-negative number less than or equal to Maximum zoom level.'));
    }
  }

  /**
   * Transform the View result in a list of marker locations and render on map.
   */
  public function render() {
    if (empty($this->options['map']) || !($map = ip_geoloc_plugin_style_leaflet_map_get_info($this->options['map']))) {
      return t('No Leaflet map was selected or map configuration was not found.');
    }
    if (!empty($this->view->live_preview)) {
      return t('The preview function is incompatible with Leaflet maps so cannot be used. Please visit the page path or the block to view your map.');
    }

    ip_geoloc_plugin_style_render_fields($this);
    $locations = ip_geoloc_plugin_style_extract_locations($this);

    $marker_color = $this->options['default_marker_color'];
    $center_option = !isset($this->options['center_option']) ? 0 : $this->options['center_option'];
    $visitor_marker_color = $this->options['visitor_marker_color'];
    $cluster_radius = $this->options['cluster_radius'];
    $marker_tooltip = $this->options['tooltips']['marker_tooltip'];
    $marker_tag = $this->options['tags']['marker_tag'];
    $tag_css_classes = $this->options['tags']['tag_css_class'];

    $module_path = drupal_get_path('module', 'ip_geoloc');
    $marker_path = variable_get('ip_geoloc_marker_directory', $module_path . '/markers');
    $marker_path = file_create_url($marker_path);

    $max_zoom = (int) $this->options['map_options']['maxzoom'];
    $zoom = (int) $this->options['map_options']['zoom'];
    $scroll_wheel_zoom = (bool) $this->options['map_options']['scrollwheelzoom'];
    $dragging = (bool) $this->options['map_options']['dragging'];

    if (!empty($locations) &&
        ($center_option == IP_GEOLOC_MAP_CENTER_ON_FIRST_LOCATION ||
        ($visitor_marker_color == 'none' && count($locations) == 1))) {
      $map['center'] = _ip_geoloc_get_center(reset($locations));
    }
    elseif (($center_option == IP_GEOLOC_MAP_CENTER_OF_LOCATIONS || $center_option == IP_GEOLOC_MAP_CENTER_OF_LOCATIONS_WEIGHTED) && !empty($locations)) {
      list($center_lat, $center_lon) = ip_geoloc_center_of_locations($locations, $center_option == IP_GEOLOC_MAP_CENTER_OF_LOCATIONS_WEIGHTED);
      $map['center'] = array(
        'lat' => $center_lat,
        'lon' => $center_lon,
      );
    }

    uasort($locations, '_ip_geoloc_plugin_style_leaflet_compare');

    $visitor_location = ip_geoloc_get_visitor_location();
    if (!isset($visitor_location['latitude'])) {
      $visitor_location = db_query('SELECT * FROM {ip_geoloc} WHERE ip_address = :ip_address', array(':ip_address' => ip_address()))->fetchAssoc();
    }
    
    if ((empty($locations) || $center_option == IP_GEOLOC_MAP_CENTER_ON_VISITOR) && isset($visitor_location['latitude'])) {
      $map['center'] = array(
        'lat' => $visitor_location['latitude'],
        'lon' => $visitor_location['longitude'],
      );
    }
    if (empty($locations)) {
      $ll = trim($this->options['empty_map_center']);
      if (empty($ll)) {
        // No map whatsoever.
        return;
      }
      if ($ll != t('visitor')) {
        // Empty map centered on coordinates provided.
        list($map['center']['lat'], $map['center']['lon']) = preg_split("/[\s,]+/", $ll);
      }
      // Else Empty map centered on visitor location, as set above.
    }

    $marker_dimensions = explode('x', variable_get('ip_geoloc_marker_dimensions', '21 x 34'));
    $marker_width  = (int) $marker_dimensions[0];
    $marker_height = (int) $marker_dimensions[1];

    switch (variable_get('ip_geoloc_marker_anchor_pos', 'bottom')) {
      case 'top':
        $marker_anchor = 0;
        break;

      case 'middle':
        $marker_anchor = (int) (($marker_height + 1) / 2);
        break;

      default:
        $marker_anchor = $marker_height;
    }

    $features = array();
    foreach ($locations as $key => $location) {
      $feature = array();
      if (isset($location->latitude) || isset($location->lat)) {
        $feature['type'] = 'point';
        $feature['lat'] = isset($location->latitude) ? $location->latitude : $location->lat;
        $feature['lon'] = isset($location->longitude) ? $location->longitude : $location->lon;
      }
      elseif (isset($location->component)) {
        // Possibly parsed by leaflet_process_geofield()
        // see _ip_geoloc_plugin_style_extract_lat_lng().
        $feature['type'] = $location->type;
        $feature['component'] = $location->component;
      }
      elseif (isset($location->points)) {
        $feature['type'] = $location->type;
        $feature['points'] = $location->points;
      }

      if (isset($location->id)) {
        // Allow marker events to identify the corresponding node.
        $feature['feature_id'] = $location->id;
      }

      // At this point $feature['type'] should be set.
      if (!empty($feature['type']) && $feature['type'] != 'point') {
        // Linestring, polygon ...
        $feature['flags'] = LEAFLET_MARKERCLUSTER_EXCLUDE_FROM_CLUSTER;
      }
      elseif (!isset($feature['lat'])) {
        // Points must have coords.
        continue;
      }
      $feature['popup'] = $location->balloon_text;

      if (!empty($location->marker_special_char) || !empty($location->marker_special_char_class)) {
        $has_special_markers = TRUE;
        if (!empty($location->marker_special_char)) {
          $feature['specialChar'] = $location->marker_special_char;
        }
        if (!empty($location->marker_special_char_class)) {
          $feature['specialCharClass'] = $location->marker_special_char_class;
          if (strpos($feature['specialCharClass'], 'fa fa-') !== FALSE || strpos($feature['specialCharClass'], 'icon-') !== FALSE /* Font-Awesome v3 */) {
            // Load the 18k of CSS that comes with Font Awesome, which in return
            // tells the browser to fetch either the WOFF (44k), TTF (80k) or
            // SVG (202k) files that define the font faces.
            drupal_add_library('ip_geoloc', 'ip_geoloc_font_awesome');
          }
        }
      }
      elseif (!empty($this->options['default_marker_special_char'])) {
        $feature['specialChar'] = $this->options['default_marker_special_char'];
        // Assume Font Awesome, add .fa class to make the special char work.
        $feature['specialCharClass'] = 'fa';
        drupal_add_library('ip_geoloc', 'ip_geoloc_font_awesome');
      }
      if (!empty($location->marker_tooltip)) {
        $has_special_markers = TRUE;
        $feature['tooltip'] = $location->marker_tooltip;
      }
      if (!empty($location->marker_tag)) {
        $has_special_markers = TRUE;
        $feature['tag'] = $location->marker_tag;
      }
      if (!empty($tag_css_classes)) {
        $feature['cssClass'] = $tag_css_classes;
      }
      if ((isset($location->marker_color) &&_ip_geoloc_is_no_marker($location->marker_color)) ||
          (!isset($location->marker_color) && _ip_geoloc_is_no_marker($marker_color))) {
        // "No marker" as opposed to "default" marker.
        $has_special_markers = TRUE;
        $feature['icon'] = FALSE;
      }
      elseif (!empty($location->marker_color) || !empty($marker_color)) {
        // Switch from default icon to specified color.
        $color = empty($location->marker_color) ? $marker_color : $location->marker_color;
        $feature['icon'] = array(
          'iconUrl' => $marker_path . "/$color.png",
          // Width and height.
          'iconSize' => array('x' => $marker_width, 'y' => $marker_height),
          'iconAnchor' => array('x' => (int) (($marker_width + 1) / 2), 'y' => $marker_anchor),
          // Just above topline, center.
          'popupAnchor' => array('x' => 0, 'y' => -$marker_height - 1),
        // 'shadowUrl'
        // 'shodowSize'
        // 'shadowAnchor
        );
      }
      $features[] = $feature;
    }
    if ($visitor_marker_color != 'none' && isset($visitor_location['latitude'])) {
      // See leaflet/README.txt for examples of Leaflet "features"
      $visitor_feature = array(
        'type' => 'point',
        'lat' => $visitor_location['latitude'],
        'lon' => $visitor_location['longitude'],
        'popup' => t('Your approximate location'),
        'tooltip' => t('Your approximate location'),
        // See leaflet_markercluster.drupal.js.
        'flags' => LEAFLET_MARKERCLUSTER_EXCLUDE_FROM_CLUSTER,
      );
      if ($visitor_marker_color != '') {
        $visitor_feature['icon'] = array(
          'iconUrl' => $marker_path . "/$visitor_marker_color.png",
          'iconSize' => array('x' => $marker_width, 'y' => $marker_height),
          'iconAnchor' => array('x' => (int) (($marker_width + 1) / 2), 'y' => $marker_anchor),
          // Just above topline, center.
          'popupAnchor' => array('x' => 0, 'y' => -$marker_height - 1),
        );
      }
      $features[] = $visitor_feature;
    }
    // If auto-box is chosen ($center_option==0), zoom only when there are
    // 0 or 1 markers [#1863374]
    if (empty($center_option) && count($features) > 1) {
      unset($map['settings']['zoom']);
    }
    else {
      $map['settings']['zoom'] = $zoom;
    }
    $map['settings']['maxZoom'] = $max_zoom;
    $map['settings']['scrollWheelZoom'] = $scroll_wheel_zoom;
    $map['settings']['dragging'] = $dragging;

    if (!empty($cluster_radius)) {
      $map['settings']['maxClusterRadius'] = $cluster_radius;
    }
    $map['settings']['animateAddingMarkers'] = TRUE;

    // See [#1802732].
    $map_id = 'ip-geoloc-map-of-view-' . $this->view->name . '-' . $this->display->id . '-' . md5(serialize($features));

    drupal_add_js(drupal_get_path('module', 'leaflet') . '/leaflet.drupal.js');
    $settings = array(
      'mapId' => $map_id,
      'map' => $map,
      'features' => $features,
    );
    drupal_add_js(array('leaflet' => array($settings)), 'setting');
    libraries_load('leaflet');
    
    // This is hacky but can't see another way to conditionally load
    // Leaflet More Map js (for Retina support and for Bing).
    if (function_exists('leaflet_more_maps_leaflet_map_prebuild_alter')) {
      leaflet_more_maps_leaflet_map_prebuild_alter();
    }

    if (!empty($cluster_radius)) {
      if (module_exists('leaflet_markercluster')) {
        leaflet_markercluster_leaflet_map_prebuild_alter();
      }
      else {
        $display_name = $this->view->get_human_name() . ' (' . $this->display->display_title . ')';
        drupal_set_message(t('Cannot cluster markers in View %display_name - Leaflet MarkerCluster module not enabled.', array('%display_name' => $display_name)), 'warning');
      }
    }
    if (!empty($has_special_markers)) {
      drupal_add_js($module_path . '/js/ip_geoloc_leaflet_tagged_marker.js');
      drupal_add_css($module_path . '/css/ip_geoloc_leaflet_markers.css');
    }

    $output = theme('ip_geoloc_leaflet', array(
      'map_id' => $map_id,
      'height' => $this->options['map_height'],
      'view' => $this->view,
    ));
    return $output;
  }
}

/**
 * Get the center of a lat/lon pair.
 */
function _ip_geoloc_get_center($location) {
  if (empty($location->type) || $location->type == 'point') {
    $lat = isset($location->lat) ? $location->lat : (isset($location->latitude) ? $location->latitude : 0.0);
    $lon = isset($location->lon) ? $location->lon : (isset($location->longitude) ? $location->longitude : 0.0);
    return array('lat' => $lat, 'lon' => $lon);
  }
  if (!empty($location->component[0]['points'][0])) {
    return $location->component[0]['points'][0];
  }
}

/**
 * Checks if marker color is a good value.
 *
 * @param mixed $marker_color
 *   The color of the marker.
 *
 * @return bool
 *   TRUE if marker color is "0", zero, or FALSE
 *   FALSE if marker colour equals '' or NULL
 */
function _ip_geoloc_is_no_marker($marker_color) {
  return isset($marker_color) && ($marker_color === '0' || $marker_color === 0 || $marker_color === FALSE);
}

/**
 * Wrapper around the only programmatic dependency we have on Leaflet module.
 *
 * Note: this indirectly calls ip_geoloc_leaflet_map_info_alter($map_info).
 */
function ip_geoloc_plugin_style_leaflet_map_get_info($map_name = NULL) {
  return leaflet_map_get_info($map_name);
}

/**
 * Callback to compare locations based on weight.
 */
function _ip_geoloc_plugin_style_leaflet_compare($location1, $location2) {
  $weight1 = empty($location1->weight) ? 0 : $location1->weight;
  $weight2 = empty($location2->weight) ? 0 : $location2->weight;
  return $weight2 - $weight1;
}
