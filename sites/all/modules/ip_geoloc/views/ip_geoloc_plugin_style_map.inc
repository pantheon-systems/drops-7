<?php

/**
 * @file
 * ip_geoloc_plugin_style_map.inc
 *
 * Views Style plugin extension.
 */

require_once 'ip_geoloc_plugin_style.inc';

class ip_geoloc_plugin_style_map extends views_plugin_style {

  /**
   * Set default map options.
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
    $options['center_option'] = array('default' => IP_GEOLOC_MAP_CENTER_ON_FIRST_LOCATION);
    $options['visitor_marker'] = array('default' => '<none>');
    $options['gps_roles'] = array(
      'default' => array(
        DRUPAL_ANONYMOUS_RID => DRUPAL_ANONYMOUS_RID,
        DRUPAL_AUTHENTICATED_RID => DRUPAL_AUTHENTICATED_RID,
      ),
    );
    $options['map_options'] = array('default' => '');
    $options['map_div_style'] = array('default' => '');
    $options['empty_map_center'] = array('default' => '');
    return $options;
  }

  /**
   * Create the options form.
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form_state['renderer'] = 'google';
    ip_geoloc_plugin_style_bulk_of_form($this, $form, $form_state);

    $form['map_options'] = array(
      '#title' => t('Map options'),
      '#type' => 'textarea',
      '#size' => 3,
      '#default_value' => $this->options['map_options'],
      '#description' => t("If left blank, the default %default_options will produce a world map zoomed in to level 2 (subject to the centering option selected). A list of map options can be found <a href='!google_map_docs'>here</a>. Remember to separate options with comma's, not semi-colons, and make sure your (double) quotes match.<br/>These options apply a grey hue to all roads:<br/>%example_options", array(
        '%default_options' => IP_GEOLOC_RECENT_VISITORS_MAP_OPTIONS,
        '%example_options' => IP_GEOLOC_EXAMPLE_MAP_OPTIONS,
        '!google_map_docs' => IP_GEOLOC_DOC_GOOGLE_MAP_OPTIONS)),
      '#weight' => 35,
    );

    $form['map_div_style'] = array(
      '#title' => t('Map style (CSS attributes)'),
      '#type' => 'textfield',
      '#size' => 127,
      '#maxlength' => 511,
      '#default_value' => $this->options['map_div_style'],
      '#description' => t('If left blank, the default %default_style will result in a map of 300 pixels high, with a width bounded by the element that contains it. Separate style settings with semi-colons. Do not enter quotes or equal signs.',
        array('%default_style' => IP_GEOLOC_MAP_DIV_DEFAULT_STYLE)),
      '#weight' => 37,
    );

    $form['visitor_marker'] = array(
      '#title' => t('Visitor marker color'),
      '#type' => 'textfield',
      '#size' => 6,
      '#default_value' => $this->options['visitor_marker'],
      '#description' => t("Show visitor marker in the color specified. %color_code is bright green. Note that an additional web service call will be made to retrieve the marker image, unless this field is left blank, which will result in a standard red marker. Enter <em>&lt;none&gt;</em> if you do not wish to display the visitor's current location.",
        array('%color_code' => '00FF00')),
      '#weight' => 90,
    );

    $form['gps_roles'] = array(
      '#title' => t('Roles for which HTML5-style visitor location retrieval is to be applied'),
      '#type' => 'checkboxes',
      '#default_value' => $this->options['gps_roles'],
      '#options' => user_roles(),
      '#description' => t('This is subject to the visitor willing to share their location when prompted by their browser. If they decline, or if their roles aren\'t ticked, an IP address lookup will be attempted instead. In case the lookup fails, for instance because you have not enabled Smart IP or GeoIP, make sure you specify "centerLat" and "centerLng" in the <strong>Map options</strong> below to avoid an empty canvas.'),
      '#weight' => 95,
    );

  }

  /**
   * Validate the options form.
   */
  public function options_validate(&$form, &$form_state) {
    ip_geoloc_plugin_style_bulk_of_form_validate($form, $form_state);
  }

  /**
   * Transform the View result in a list of markers and render these on a map.
   */
  public function render() {

    if (!empty($this->view->live_preview)) {
      return t('The preview function is incompatible with Google Maps so cannot be used. Please visit the page path or block to view your map.');
    }

    ip_geoloc_plugin_style_render_fields($this);
    $locations = ip_geoloc_plugin_style_extract_locations($this);

    $map_options = empty($this->options['map_options']) ? IP_GEOLOC_RECENT_VISITORS_MAP_OPTIONS : $this->options['map_options'];
    $map_div_style = empty($this->options['map_div_style']) ? IP_GEOLOC_MAP_DIV_DEFAULT_STYLE : check_plain($this->options['map_div_style']);
    $marker_color = empty($this->options['default_marker_color']) ? '' : $this->options['default_marker_color'];

    if (empty($this->options['visitor_marker'])) {
      // Default to standard red marker.
      $visitor_marker = TRUE;
    }
    else {
      $visitor_marker = trim($this->options['visitor_marker']);
      $visitor_marker = strpos($visitor_marker, 'none') === FALSE ? check_plain($visitor_marker) : FALSE;
    }

    $center_option = !isset($this->options['center_option']) ? IP_GEOLOC_MAP_CENTER_ON_FIRST_LOCATION : $this->options['center_option'];
    $center_latlng = FALSE;

    // Set the behaviour for the case that there are no locations to map.
    if (empty($locations)) {
      $ll = trim($this->options['empty_map_center']);
      if (empty($ll)) {
        // No map whatsoever.
        return;
      }
      if ($ll == t('visitor')) {
        $center_option = IP_GEOLOC_MAP_CENTER_ON_VISITOR;
      }
      else {
        // Empty map centered on coordinates provided.
        list($center_lat, $center_lng) = preg_split("/[\s,]+/", $ll);
      }
    }

    if ($visitor_marker || $center_option == IP_GEOLOC_MAP_CENTER_ON_VISITOR) {
      // Perform database IP lookup as backup/replacement for HTML5 location
      // Visitor may be moving so ignore lat/long we have on the db.
      $resample = TRUE;
      // Do not store lat/long and city as it will obliterate the
      // reverse-geocoded one.
      $store = FALSE;
      // We only need lat/long, not full street address.
      $reverse_geocode = FALSE;
      $visitor_location = ip_geoloc_get_location_by_ip(ip_address(), $resample, $store, $reverse_geocode);
      if (!isset($visitor_location['latitude'])) {
        // If everything failed use whatever we have on the database.
        $visitor_location = db_query('SELECT * FROM {ip_geoloc} WHERE ip_address = :ip_address', array(':ip_address' => ip_address()))->fetchAssoc();
      }
      if (isset($visitor_location['latitude']) && isset($visitor_location['longitude'])) {
        $center_latlng = array($visitor_location['latitude'], $visitor_location['longitude']);
      }
    }

    if (!empty($locations) && $center_option == IP_GEOLOC_MAP_CENTER_OF_LOCATIONS || $center_option == IP_GEOLOC_MAP_CENTER_OF_LOCATIONS_WEIGHTED) {
      list($center_lat, $center_lng) = ip_geoloc_center_of_locations($locations, $center_option == IP_GEOLOC_MAP_CENTER_OF_LOCATIONS_WEIGHTED);
    }
    if (isset($center_lat) && isset($center_lng)) {
      $map_options = drupal_substr($map_options, 0, strrpos($map_options, '}'));
      $map_options = $map_options . ', "centerLat":' . $center_lat . ', "centerLng":' . $center_lng . '}';
    }

    global $user;
    $gps_roles = empty($this->options['gps_roles']) ? array(DRUPAL_ANONYMOUS_RID, DRUPAL_AUTHENTICATED_RID) : $this->options['gps_roles'];
    $gps_roles_applicable = array_intersect($gps_roles, array_keys($user->roles));

    $output = theme(array('ip_geoloc_map'), array(
      'view' => $this->view,
      'locations' => $locations,
      'div_id' => 'ip-geoloc-map-of-view-' . $this->view->name . '-' . $this->display->id,
      'map_options' => $map_options,
      'map_style' => $map_div_style,
      'marker_color' => $marker_color,
      'visitor_marker' => $visitor_marker,
      'center_option' => $center_option,
      'center_latlng' => $center_latlng,
      'visitor_location_gps' => !empty($gps_roles_applicable),
    ));
    return $output;
  }

}
