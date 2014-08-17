<?php

/**
 * @file
 * API functions of IP geolocation module
 *
 * Reusable functions that may be called from other modules.
 */

// Google's gears_init.js is now included at the top of geo.js
//define('IP_GEOLOC_GEO_JS', 'https://geo-location-javascript.googlecode.com/svn/trunk/js/geo.js');

// Note: secure http was chosen, as plain http may cause warnings popping up in
// environments using certificates.
// Size 5k.
define('IP_GEOLOC_GOOGLE_MAPS', 'https://maps.googleapis.com/maps/api/js?sensor=false');
// Size 5k.
define('IP_GEOLOC_GOOGLE_MAPS_GPS', 'https://maps.googleapis.com/maps/api/js?sensor=true');
define('IP_GEOLOC_GOOGLE_MAPS_SERVER', 'https://maps.googleapis.com/maps/api/geocode/json?sensor=false');

/**
 * Store the supplied IP geolocation info on the database.
 *
 * This will overwrite any existing info for the IP address in question.
 *
 * @param array $location
 *   array with up to 13 location info fields; must at least
 *   contain a non-empty $location['ip_address'] and a non-empty
 *   $location['formatted_address'] for anything to happen
 *
 * @return int|bool
 *   0, when no insert or update was necessary
 *   SAVED_NEW (=1), when a new location record was inserted into the db
 *   SAVED_UPDATED (=2), when an existing location record was updated
 *   FALSE, when a db insert or db update failed
 */
function ip_geoloc_store_location($location) {
  // Give contributed modules a chance to add their 2 cents by implementing
  // hook_get_ip_geolocation_alter()
  drupal_alter('get_ip_geolocation', $location);

  $result = 0;
  if (empty($location['ip_address']) || empty($location['formatted_address'])) {
    // ip_geoloc_debug('IPGV&M: location object must contain both IP address
    // and formatted address -- not stored.');
  }
  else {
    if ($location['ip_address'] != '127.0.0.1' && (!isset($location['latitude']) || !isset($location['latitude']))) {
      watchdog('IPGV&M', 'latitude or longitude missing for IP address %ip (location still stored)', array('%ip' => $location['ip_address']), WATCHDOG_WARNING);
    }
    // See if this IP is already on the db.
    $result = db_query('SELECT * FROM {ip_geoloc} WHERE ip_address = :ip', array(':ip' => $location['ip_address']));
    $existing_location = $result->fetchAssoc();

    if (!$existing_location) {
      // New entry, insert.
      $location['city'] = utf8_encode($location['city']);
      $location['formatted_address'] = utf8_encode($location['formatted_address']);
      ip_geoloc_debug(t('IP Geolocaton: adding new record to db: !location', array('!location' => ip_geoloc_pretty_print($location))));
      $full_location = &$location;
    }
    else {
      // When updating, drupal_write_record() does not erase fields not present
      // in $location.
      $empty_location['latitude'] = '';
      $empty_location['longitude'] = '';
      $empty_location['country'] = '';
      $empty_location['country_code'] = '';
      $empty_location['region'] = '';
      $empty_location['region_code'] = '';
      $empty_location['city'] = '';
      $empty_location['locality'] = '';
      $empty_location['route'] = '';
      $empty_location['street_number'] = '';
      $empty_location['postal_code'] = '';
      $empty_location['administrative_area_level_1'] = '';
      $empty_location['formatted_address'] = '';
      $location['id'] = $existing_location['id'];
      $full_location = array_merge($empty_location, $location);
      ip_geoloc_debug(t('IPGV&M: updating db with above location'));
    }
    try {
      $result = drupal_write_record('ip_geoloc', $full_location, $existing_location ? array('id') : array());
    }
    catch (PDOException $e) {
      // May happen when a fields contains illegal characters.
      drupal_set_message(check_plain($e->getMessage()), 'error');
      $result = FALSE;
    }
    if ($result === FALSE) {
      drupal_set_message(t('IPGV&M: could not save location to db: !location', array('!location' => ip_geoloc_pretty_print($full_location))), 'error');
    }
  }
  return $result;
}

/**
 * Outputs an HTML div placeholder into which will be injected a map.
 *
 * The map is centered on the supplied lat,long coordinates.
 * Handy for use in Views.
 *
 * @param string|float $latitude
 *   e.g. "-37.87" or -37.87
 * @param string|float $longitude
 *   e.g. "144.98" or 144.98
 * @param string $div_id
 *   id of the div placeholder, can be anything as long as it's unique
 * @param string $style
 *   CSS style applied to the div, e.g "height:250px; width:300px"
 */
function ip_geoloc_output_map($latitude, $longitude, $div_id = 'ip-geoloc-map', $style = '', $balloon_text = '') {
  $javascript_files = drupal_add_js();
  if (empty($javascript_files[IP_GEOLOC_GOOGLE_MAPS_GPS])) {
    // Only add non-GPS version when GPS version isn't already there.
    drupal_add_js(IP_GEOLOC_GOOGLE_MAPS, array('group' => JS_LIBRARY));
  }
  drupal_add_js(drupal_get_path('module', 'ip_geoloc') . '/js/ip_geoloc_gmap.js');

  $script_code = "displayGMap($latitude, $longitude, '$div_id', '$balloon_text');";
  drupal_add_js($script_code, array('type' => 'inline', 'scope' => 'footer'));

  $map_placeholder = "<div id='$div_id'" . (empty($style) ? '' : " style='$style'") . '></div>';
  return $map_placeholder;
}

/**
 * Outputs an HTML div placeholder into which is injected a map.
 *
 * The map is centered on the visitor's current location and features a position
 * marker, which when clicked reveals latitude and longitude, as well as the
 * street address and the accuracy of the position shown.
 *
 * Note this function will result in the visitor being prompted to share their
 * location.
 *
 * @param string $div_id
 *   id of the div placeholder, can be anything as long as it's unique
 * @param string $map_options
 *   as a JSON string e.g '{"mapTypeId":"roadmap", "zoom":15}'
 * @param string $map_style
 *   CSS style applied to the div, e.g "height:200px; width:300px"
 * @param float $latitude
 *   if empty and HTML5 location retrieval will be initiated
 * @param float $longitude
 *   if empty and HTML5 location retrieval will be initiated
 * @param string $info_text
 *   optional string to display in the marker balloon, e.g. formatted address
 */
function ip_geoloc_output_map_current_location($div_id = 'ip-geoloc-map-current-location',
    $map_options = NULL, $map_style = NULL, $latitude = NULL, $longitude = NULL, $info_text = NULL) {
  drupal_add_js(IP_GEOLOC_GOOGLE_MAPS_GPS, array('group' => JS_LIBRARY));
  drupal_add_js(drupal_get_path('module', 'ip_geoloc') . '/js/ip_geoloc_gmap_current_loc.js');

  if (!isset($map_options)) {
    $map_options = IP_GEOLOC_CURRENT_VISITOR_MAP_OPTIONS;
  }
  $settings = array(
    'ip_geoloc_current_location_map_div' => $div_id,
    'ip_geoloc_current_location_map_options' => drupal_json_decode($map_options),
    'ip_geoloc_current_location_map_latlng' => array($latitude, $longitude),
    'ip_geoloc_current_location_map_info_text' => $info_text,
  );
  drupal_add_js($settings, 'setting');

  if (!isset($map_style)) {
    $map_style = IP_GEOLOC_MAP_DIV_DEFAULT_STYLE;
  }
  $map_placeholder = "<div id='$div_id'" . (empty($map_style) ? '' : " style='$map_style'") . '></div>';
  return $map_placeholder;
}

/**
 * Outputs an HTML div placeholder into which will be injected a map.
 *
 * The locations to be mapped are supplied as an array of lat,long coordinates.
 *
 * @param array $locations
 *   array of location objects each containing lat/long pair and optionally
 *   address, visit count and last visit to appear when the marker is clicked
 * @param string $div_id
 *   id of the div placeholder, can be anything as long as it's unique
 * @param string $map_options
 *   as a JSON string, .e.g '{"mapTypeId":"roadmap", "zoom":15}'
 * @param string $map_style
 *   CSS style applied to the div, e.g "height:250px; width:300px"
 */
function ip_geoloc_output_map_multi_visitor($locations, $div_id = 'ip-geoloc-map-multi-locations', $map_options = NULL, $map_style = NULL) {
  $javascript_files = drupal_add_js();
  if (empty($javascript_files[IP_GEOLOC_GOOGLE_MAPS_GPS])) {
    // Only add non-GPS version when GPS version isn't already there.
    drupal_add_js(IP_GEOLOC_GOOGLE_MAPS, array('group' => JS_LIBRARY));
  }
  drupal_add_js(drupal_get_path('module', 'ip_geoloc') . '/js/ip_geoloc_gmap_multi_visitor.js');

  if (!isset($map_options)) {
    $map_options = IP_GEOLOC_RECENT_VISITORS_MAP_OPTIONS;
  }
  $settings = array(
    'ip_geoloc_locations' => $locations,
    'ip_geoloc_multi_location_map_div' => $div_id,
    'ip_geoloc_multi_location_map_options' => drupal_json_decode($map_options),
  );
  drupal_add_js($settings, 'setting');

  if (!isset($map_style)) {
    $map_style = IP_GEOLOC_MAP_DIV_DEFAULT_STYLE;
  }
  $map_placeholder = "<div id='$div_id'" . (empty($map_style) ? '' : " style='$map_style'") . '></div>';
  return $map_placeholder;
}

/**
 * Outputs an HTML div placeholder into which will be injected a Google map.
 *
 * The locations to be mapped are supplied as an array of location objects,
 * each with lat,long coordinates and optional balloon text.
 *
 * Note this function will result in the visitor being prompted to share their
 * location.
 *
 * @param array $locations
 *   array of location objects each containing lat/long pair
 *   and optional balloon text.
 *
 * @param string $div_id
 *   id of the div placeholder, can be anything as long as it's unique
 *
 * @param string $map_options
 *   as a JSON string, .e.g '{"mapTypeId":"roadmap", "zoom":15}'
 *
 * @param string $map_style
 *   CSS style applied to the div, e.g "height:250px; width:300px"
 *
 * @param string $marker_color
 *   default color used for all locations that haven't had their
 *   color overridden, defaults to Google default (i.e. red marker with dot).
 *
 * @param bool|string $visitor_marker
 *   FALSE (no marker), TRUE (standard red marker) or a 'RRGGBB' color code
 *
 * @param int $center_option
 *   0: fixed cenrer, must be provided thorugh $map_options)
 *   1: auto-center the map on the first location in the $locations array
 *   2: auto-center the map on the visitor's current location
 *
 * @param array $center_latlng
 *   array of length 2 with lat/long coords used as a backup
 *   when $visitor_marker is set or $center_option == 2 and location could not
 *   be determined or $visitor_location_gps == FALSE
 *
 * @param bool $visitor_location_gps
 *   whether the HTML5-style location provider is to be
 *   used, if FALSE $center_latlng is used
 *
 * @return string
 *   map placeholder div
 */
function ip_geoloc_output_map_multi_location($locations, $div_id = 'ip-geoloc-map-multi-locations',
    $map_options = NULL, $map_style = NULL, $marker_color = NULL, $visitor_marker = TRUE,
    $center_option = 0, $center_latlng = array(0, 0), $visitor_location_gps = TRUE) {

  if ($visitor_location_gps) {
    drupal_add_js(IP_GEOLOC_GOOGLE_MAPS_GPS, array('group' => JS_LIBRARY));
  }
  else {
    $javascript_files = drupal_add_js();
    if (empty($javascript_files[IP_GEOLOC_GOOGLE_MAPS_GPS])) {
      // Only add non-GPS version when GPS version isn't already there.
      drupal_add_js(IP_GEOLOC_GOOGLE_MAPS, array('group' => JS_LIBRARY));
    }
  }
  drupal_add_js(drupal_get_path('module', 'ip_geoloc') . '/js/ip_geoloc_gmap_multi_loc.js');

  if (!isset($map_options)) {
    $map_options = IP_GEOLOC_RECENT_VISITORS_MAP_OPTIONS;
  }

  $marker_directory = variable_get('ip_geoloc_marker_directory', drupal_get_path('module', 'ip_geoloc') . '/markers');
  $marker_directory = file_create_url($marker_directory);

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

  ip_geoloc_debug(t('IPGV&M: passing the following to Google Maps:'));
  ip_geoloc_debug(t('- map options %options:', array('%options' => $map_options)));
  ip_geoloc_debug(t('- center option: @option', array('@option' => $center_option)));
  ip_geoloc_debug(t('- visitor marker: %marker', array('%marker' => $visitor_marker)));
  ip_geoloc_debug(t('- use GPS: @gps', array('@gps' => (bool) $visitor_location_gps)));
  ip_geoloc_debug(t('- visitor location fallback: (@lat, @lng)', array(
    '@lat' => empty($center_latlng[0]) ? '-' : $center_latlng[0],
    '@lng' => empty($center_latlng[1]) ? '-' : $center_latlng[1],
  )));
  ip_geoloc_debug(t('- marker directory : %dir', array('%dir' => $marker_directory)));
  ip_geoloc_debug(t('- marker dimensions : w@w x h@h px, anchor: @a px',
    array('@w' => $marker_width, '@h' => $marker_height, '@a' => $marker_anchor)));
  ip_geoloc_debug(t('- @count locations found', array('@count' => count($locations))));

  // Locations transferred to JS via the settings array aren't refreshed in
  // AJAX contexts. That's why we are doing it this messy way. @todo: refactor.
  print "\n<script>\nif (typeof(ip_geoloc_locations) === 'undefined') {\n ip_geoloc_locations = new Array();\n}\n";
  print "ip_geoloc_locations['$div_id'] = [\n";

  foreach ($locations as $location) {
    $balloon_text = isset($location->balloon_text) ? str_replace("\n", '', $location->balloon_text) : '';
    print '{ "type":"point"' .
      (empty($location->marker_color) ? '' : ', "marker_color":"' . $location->marker_color . '"') .
      ', "latitude":' . $location->latitude .
      ', "longitude":' . $location->longitude .
      ', "balloon_text":\'' . $balloon_text . '\'},';

    $color = empty($location->marker_color) ? t('default') . " [$marker_color]" : $location->marker_color;
    $coords = isset($location->latitude) ? $location->latitude . ', ' . $location->longitude : '';
    $msg = '- ' . t('marker') . " $color ($coords)<br/>$balloon_text<br/>";
    ip_geoloc_debug($msg);
  }
  print "\n];\n</script>\n";

  $settings = array(
    'ip_geoloc_multi_location_map_div' => $div_id,
    'ip_geoloc_multi_location_map_options' => drupal_json_decode($map_options),
    'ip_geoloc_multi_location_center_option' => (int)$center_option,
    'ip_geoloc_multi_location_center_latlng' => $center_latlng,
    'ip_geoloc_multi_location_visitor_marker' => $visitor_marker,
    'ip_geoloc_multi_location_visitor_location_gps' => $visitor_location_gps,
    'ip_geoloc_multi_location_marker_directory' => $marker_directory,
    'ip_geoloc_multi_location_marker_width' => (int)$marker_width,
    'ip_geoloc_multi_location_marker_height' => (int)$marker_height,
    'ip_geoloc_multi_location_marker_anchor' => (int)$marker_anchor,
    'ip_geoloc_multi_location_marker_default_color' => $marker_color,
  );
  drupal_add_js(array($settings), 'setting');

  if (!isset($map_style)) {
    $map_style = IP_GEOLOC_MAP_DIV_DEFAULT_STYLE;
  }
  $map_placeholder = "<div id='$div_id'" . (empty($map_style) ? '' : " style='$map_style'") . '></div>';
  return $map_placeholder;
}

/**
 * Uses AJAX to return in $_POST info about the visitor's current location.
 *
 * Note: this function will result in the browser prompting the visitor to share
 * their location, which they may or may not accept.
 *
 * This works via an asynchronous javascript call, so the result is not
 * immediately available on return from this function, hence the $menu_callback.
 * Upon page load the included javascript will, when ready, instigate an AJAX
 * call to the $menu_callback, which should invoke a function to retrieve the
 * lat/long and address values from the $_POST variable.
 * See ip_geoloc_current_location_ajax_recipient() for an example.
 *
 * Note: will result in a HTTP error 503 when the site is in maintenance mode,
 *       as in maintenance mode menu items are not available.
 */
function ip_geoloc_get_current_location($menu_callback = 'js/ip_geoloc/current_location') {
  drupal_add_js(IP_GEOLOC_GOOGLE_MAPS_GPS, array('group' => JS_LIBRARY));
  drupal_add_js(drupal_get_path('module', 'ip_geoloc') . '/js/ip_geoloc_current_location.js');

  $refresh_page = variable_get('ip_geoloc_page_refresh', FALSE) && !path_is_admin($_GET['q']);
  $settings = array(
    'ip_geoloc_menu_callback' => $menu_callback,
    'ip_geoloc_refresh_page' => $refresh_page,
  );
  drupal_add_js($settings, 'setting');
}

/**
 * Returns the location details associated with the supplied IP address.
 *
 * Performs a lookup in IPGV&M's own database to see if the supplied IP
 * address has visited already and if so returns their location details (as an
 * array). If the IP address is not yet in the IP geolocation database, then
 * retrieve lat/long using either Smart IP or GeoIP API (if enabled) and
 * reverse-geocode the lat/long (if the Google Maps service is enabled) into a
 * location. If the third argument is TRUE, then store the new location.
 *
 * @param string $ip_address
 *   The IP address to locate
 * @param bool $resample
 *   if set to TRUE, ignore any existing location data for this
 *   IP address and retrieve the latest
 * @param bool $store
 *   if TRUE, store the new or resampled location on the db
 * @param bool|null $reverse_geocode
 *   applies only when the supplied IP address is not
 *   yet on the database or $resample=TRUE; use TRUE, FALSE or NULL; TRUE will
 *   produce greater detail in the location returned; if NULL or omitted the
 *   value is taken from the tick box on the IP Geolocation configuration page.
 *   Reverse-geocoding is subject to a Google-imposed limit of 2500 calls per
 *   day from the same server IP address.
 *
 * @return array
 *   location as an array
 */
function ip_geoloc_get_location_by_ip($ip_address, $resample = FALSE, $store = FALSE, $reverse_geocode = NULL) {
  $location = $resample ? NULL : db_query('SELECT * FROM {ip_geoloc} WHERE ip_address = :ip_address', array(':ip_address' => $ip_address))->fetchAssoc();
  if (empty($location)) {
    $location = array('ip_address' => $ip_address);
    if (ip_geoloc_use_smart_ip_if_enabled($location) || ip_geoloc_use_geoip_api_if_enabled($location)) {
      if (!isset($reverse_geocode)) {
        $reverse_geocode = variable_get('ip_geoloc_google_to_reverse_geocode', TRUE);
      }
      if ($reverse_geocode && isset($location['latitude']) && isset($location['longitude'])) {
        if ($google_address = ip_geoloc_reverse_geocode($location['latitude'], $location['longitude'])) {
          // Should we clear out whatever Smart IP or GeoIP put in the $location
          // to avoid fields contradicting eachother? Eg. Google normally
          // returns 'locality', whereas Smart IP and GeoIP return 'city'.
          // $location = array('ip_address' => $ip_address);
          ip_geoloc_flatten_google_address($google_address, $location);
        }
      }
      if ($store) {
        // Calls drupal_alter().
        ip_geoloc_store_location($location);
      }
      else {
        drupal_alter('get_ip_geolocation', $location);
      }
    }
  }
  return $location;
}

/**
 * Returns the formatted address reverse-geocoded from the supplied lat,long.
 *
 * See the CALLER BEWARE note at ip_geoloc_reverse_geocode().
 *
 * @param string|double $latitude
 *   e.g. "-37.87" or -37.87
 *
 * @param string|double $longitude
 *   e.g. "144.98" or 144.98
 *
 * @return string
 *   Formatted address component as received from Google or empty string.
 */
function ip_geoloc_get_address($latitude, $longitude) {
  return $google_address = ip_geoloc_reverse_geocode($latitidue, $longitude) ? $google_address['formatted_address'] : '';
}

/**
 * Uses the Google webservice to retrieve address information based on lat/long.
 *
 * Effectively makes calls of this form:
 * http://maps.googleapis.com/maps/api/geocode/json?sensor=false&latlng=-37,144
 *
 * CALLER BEWARE:
 * This is a server-side, as opposed to client-side call. If you want to call
 * this function repeatedly, remember that Google imposes a limit of 2500 calls
 * from the same IP address per 24 hours. It may return an OVER_QUERY_LIMIT
 * response.
 *
 * @param string|double $latitude
 *   e.g. "-37.87" or -37.87
 *
 * @param string|double $longitude
 *   e.g. "144.98" or 144.98
 *
 * @return array|bool
 *   Array of address components as received from Google or FALSE.
 */
function ip_geoloc_reverse_geocode($latitude, $longitude) {
  if (empty($latitude) || empty($latitude)) {
    drupal_set_message(t('IPGV&M: cannot reverse-geocode to address as no lat/long was specified.'), 'warning');
    return FALSE;
  }
  $query_start = microtime(TRUE);
  $url = IP_GEOLOC_GOOGLE_MAPS_SERVER . "&latlng=$latitude,$longitude";
  $response = drupal_http_request($url);
  if (!empty($response->error)) {
    $msg_args = array(
      '%url' => $url,
      '@code' => $response->code,
      '%error' => $response->error,
    );
    drupal_set_message(t('IPGV&M: the HTTP request %url returned the following error (code @code): %error.', $msg_args), 'error');
    watchdog('IPGV&M', 'Error (code @code): %error. Request: %url', $msg_args, WATCHDOG_ERROR);
    return FALSE;
  }
  $data = drupal_json_decode($response->data);
  if ($data['status'] == 'OVER_QUERY_LIMIT') {
    $msg_args = array('%url' => $url);
    if (user_access('administer site configuration')) {
      drupal_set_message(t('IPGV&M: Server is over its query limit. Request: %url', $msg_args), 'error');
    }
    watchdog('IPGV&M', 'Server is over its query limit. Request: %url', $msg_args, WATCHDOG_ERROR);
    return FALSE;
  }
  if ($data['status'] == 'ZERO_RESULTS' || !isset($data['results'][0])) {
    $msg_args = array('@protocol' => $response->protocol, '%url' => $url);
    drupal_set_message(t('IPGV&M: the @protocol request %url succeeded, but returned no results.', $msg_args), 'warning');
    watchdog('IPGV&M', 'No results from @protocol request %url.', $msg_args, WATCHDOG_WARNING);
    return FALSE;
  }
  if ($data['status'] != 'OK') {
    $msg_args = array('%url' => $url, '%error' => $data['status']);
    drupal_set_message(t('IPGV&M: unknown error %error. Request: %url..', $msg_args), 'error');
    watchdog('IPGV&M', 'Unknown error %error. Request: %url.', $msg_args, WATCHDOG_ERROR);
    return FALSE;
  }
  $google_address = $data['results'][0];
  if (empty($google_address['formatted_address'])) {
    $msg_args = array('@lat' => $latitude, '@long' => $longitude);
    ip_geoloc_debug(t('IPGV&M: (@lat, @long) could not be reverse-geocoded to a street address.', $msg_args), 'warning');
    watchdog('IPGV&M', '(@lat, @long) could not be reverse-geocoded to a street address..', $msg_args, WATCHDOG_WARNING);
  }
  else {
    $sec = number_format(microtime(TRUE) - $query_start, 1);
    $msg_args = array(
      '@lat' => $latitude, '@long' => $longitude,
      '%sec' => $sec,
      '%address' => $google_address['formatted_address'],
    );
    ip_geoloc_debug(t('IPGV&M: %address reverse-geocoded from (@lat, @long) in %sec s.', $msg_args));
    watchdog('IPGV&M', '%address reverse-geocoded from (@lat, @long) in %sec s.', $msg_args, WATCHDOG_INFO);
  }
  return $google_address;
}

/**
 * Return the visitor's location as currently stored in the session.
 *
 * @return array
 *   Lat/Lon array from SESSION
 */
function ip_geoloc_get_visitor_location() {
  return _ip_geoloc_get_session_value('location');
}

/**
 * Calculate the center of the supplied locations using one of two algorithms.
 *
 * The first algorithm returns the center of the rectangle whose horizontal
 * sides pass through the top and bottom locations in the set, while its
 * vertical sides pass through the left-most and right-most locations.
 *
 * The second algorithm returns the center of gravity of all supplied locations.
 * The second algorithn is therefore sensitive to location clusters. This may
 * be what you want, or it may be what you want to avoid.
 *
 * @param array $locations
 *   array of location objects each with latitude and longitude
 *
 * @param bool $center_of_gravity
 *   if TRUE use the center of gravity algorithm
 *
 * @return array
 *   containing latitude and longitude of the center
 */
function ip_geoloc_center_of_locations($locations, $center_of_gravity = FALSE) {
  if (empty($locations)) {
    return array(0.0, 0.0);
  }
  if ($center_of_gravity) {
    // Because longitude turns back on itself, cannot simply average coords.
    $count = 0;
    $x = $y = $z = 0.0;
    foreach ($locations as $location) {
      if (isset($location->lon)) {
        $lng = $location->lon;
        $lat = $location->lat;
      }
      elseif (isset($location->longitude)) {
        $lng = $location->longitude;
        $lat = $location->latitude;
      }
      else {
        continue;
      }
      $lng = deg2rad($lng);
      $lat = deg2rad($lat);
      // Convert to Cartesian coords and total the 3 dimensions independently.
      $x += cos($lat) * cos($lng);
      $y += cos($lat) * sin($lng);
      $z += sin($lat);
      $count++;
    }
    $x /= $count;
    $y /= $count;
    $z /= $count;
    $center_lat = atan2($z, sqrt($x * $x + $y * $y));
    $center_lng = atan2($y, $x);
    return array(rad2deg($center_lat), rad2deg($center_lng));
  }
  // Alternative method based on top & bottom lat and left & right lon.
  $location = reset($locations);
  $top = $bottom = isset($location->lat) ? $location->lat : $location->latitude;
  $left = $right = isset($location->lon) ? $location->lon : $location->longitude;
  while ($location = next($locations)) {
    if (isset($location->lon)) {
      $lng = $location->lon;
      $lat = $location->lat;
    }
    elseif (isset($location->longitude)) {
      $lng = $location->longitude;
      $lat = $location->latitude;
    }
    else {
      continue;
    }
    if ($lat > $top) {
      $top = $lat;
    }
    elseif ($lat < $bottom) {
      $bottom = $lat;
    }
    if ($lng < $left) {
      $left = $lng;
    }
    elseif ($lng > $right) {
      $right = $lng;
    }
  }
  $center_lat = 0.5 * ($top + $bottom);
  $center_lng = 0.5 * ($left + $right);
  if ($right - $left > 180) {
    // If the angle between right and left is greater than 180, then averaging
    // is still ok, provided we flip over to the opposite end of the world.
    $center_lng = ($center_lng > 0.0) ? $center_lng - 180.0 : $center_lng + 180.0;
  }
  return array($center_lat, $center_lng);
}

/**
 * Returns the distance (in meters) between two points on the earth's surface.
 *
 * The points are defined by their lat/long coordinates. If the second point is
 * omitted, the current visitor's location is used, as taken from their session
 * data.
 *
 * @param array $location
 *   must contain 'latitude' and 'longitude' keys and values
 *
 * @param array $ref_location
 *   if an array, must contain 'latitude' and 'longitude' keys and values,
 *   otherwise defaults to ip_geoloc_get_visitor_location()
 *
 * @return float
 *   distance in meters.
 */
function ip_geoloc_distance($location, $ref_location = 'current visitor') {
  if (!is_array($ref_location)) {
    $ref_location = ip_geoloc_get_visitor_location();
  }
  if (empty($ref_location)) {
    return '?';
  }
  if (is_numeric($location['longitude']) && is_numeric($location['latitude']) && is_numeric($ref_location['longitude']) && is_numeric($ref_location['latitude'])) {
    return ip_geoloc_earth_distance($location['longitude'], $location['latitude'], $ref_location['longitude'], $ref_location['latitude']);
  }
  return '?';
}

/**
 * Returns the distance between two points on the earth's surface.
 *
 * The points are defined by their lat/long coordinates.
 *
 * Gratefully copied from the http://drupal.org/project/location module, thus
 * ensuring compatibility of results.
 *
 * @param float $longitude1
 *   Must be in range -180..180.
 *
 * @param float $latitude1
 *   Must be in range -90..90.
 *
 * @param float $longitude2
 *   Must be in range -180..180.
 *
 * @param float $latitude2
 *   Must be in range -90..90
 *
 * @return float
 *   Distance between the two points in meters.
 *
 * @see http://en.wikipedia.org/wiki/Great-circle_distance
 */
function ip_geoloc_earth_distance($longitude1, $latitude1, $longitude2, $latitude2) {
  $long1 = deg2rad($longitude1);
  $lat1  = deg2rad($latitude1);
  $long2 = deg2rad($longitude2);
  $lat2  = deg2rad($latitude2);
  // $long_factor = cos($long1) * cos($long2) + sin($long1) * sin($long2);
  // This is identical to this $long_factor = cos($long1 - $long2).
  $long_factor = cos($long1 - $long2);
  $cosangle = cos($lat1) * cos($lat2) * $long_factor + sin($lat1) * sin($lat2);
  $radius = ip_geoloc_earth_radius(0.5 * ($latitude1 + $latitude2));
  $distance = acos($cosangle) * $radius;
  /*
  if ($distance < 1000) {
  // see http://en.wikipedia.org/wiki/Haversine_formula
  $sinlat = sin(0.5*($lat1 - $lat2));
  $sinlong = sin(0.5*($long1 - $long2));
  $sinangle = $sinlat*$sinlat + cos($long1)*cos($long2)*$sinlong*$sinlong;
  $distance = 2.0 * asin(sqrt($sinangle)) * $radius;
  }
  */
  return $distance;
}

/**
 * Get radius of the Earth at a given latitude.
 *
 * @param float $latitude
 *   The latitude for which to calculate Earth's radius
 *
 * @return float
 *   The radius of Earth at the given latitude
 */
function ip_geoloc_earth_radius($latitude) {
  $lat = deg2rad($latitude);
  $x = cos($lat) / ip_geoloc_earth_radius_semimajor();
  $y = sin($lat) / ip_geoloc_earth_radius_semiminor();
  return 1.0 / (sqrt($x * $x + $y * $y));
}

/**
 * Get semimajor radius.
 */
function ip_geoloc_earth_radius_semimajor() {
  return 6378137.0;
}

/**
 * Get semiminor radius.
 */
function ip_geoloc_earth_radius_semiminor() {
  return (ip_geoloc_earth_radius_semimajor() * (1.0 - ip_geoloc_earth_flattening()));
}

/**
 * Flatten the earth.
 */
function ip_geoloc_earth_flattening() {
  return (1.0 / 298.257223563);
}
