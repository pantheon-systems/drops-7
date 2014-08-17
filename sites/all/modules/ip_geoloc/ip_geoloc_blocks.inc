<?php

/**
 * @file
 * Blocks available in IP Geolocation Views & Maps.
 */

/**
 * Implements hook_block_info().
 */
function ip_geoloc_block_info() {
  $blocks['current_visitor_map'] = array(
    'info' => t("Map (Google) centered on visitor's location"),
    'cache' => DRUPAL_NO_CACHE,
  );
  $blocks['recent_visitors_map'] = array(
    'info' => t('Map (Google) of @n most recent visitors', array('@n' => variable_get('ip_geoloc_recent_visitors_map_number_of_visitors', 20))),
    'cache' => DRUPAL_NO_CACHE,
  );
  $blocks['address_lookup'] = array(
    'info' => t('Address lookup based on IP'),
    'cache' => DRUPAL_NO_CACHE,
  );
  $blocks['geocode_address'] = array(
    'info' => t('Refine location via street address'),
    'cache' => DRUPAL_NO_CACHE,
  );
  return $blocks;
}

/**
 * Implements hook_block_configure().
 */
function ip_geoloc_block_configure($delta = '') {
  $form = array();
  if ($delta == 'address_lookup') {
    return $form;
  }
  if ($delta == 'geocode_address') {
    $form['ip_geoloc_visitor_address_editable'] = array(
      '#type' => 'checkbox',
      '#title' => t('Address editable'),
      '#default_value' => variable_get('ip_geoloc_visitor_address_editable', TRUE),
      '#description' => t('If ticked the visitor may tweak and re-submit their address. It will be geocoded to lat/long to become their new location.<br/>If not ticked, this field simply displays the current reverse-geocoded address, without a submit button.'),
    );
    return $form;
  }

  if ($delta == 'recent_visitors_map') {
    $form['ip_geoloc_recent_visitors_map_number_of_visitors'] = array(
      '#type' => 'textfield',
      '#title' => t('Number of recent visitor locations to show on the map.'),
      '#default_value' => variable_get('ip_geoloc_recent_visitors_map_number_of_visitors', 20),
      '#description' => t('Must be greater than 0. Clustering is not supported, all markers are shown individually.'),
    );
  }
  $form['ip_geoloc_' . $delta . '_div_style'] = array(
    '#type' => 'textfield',
    '#title' => t('CSS style attribute(s) applied to the HTML DIV element that is placeholding the map'),
    '#default_value' => variable_get('ip_geoloc_' . $delta . '_div_style', IP_GEOLOC_MAP_DIV_DEFAULT_STYLE),
    '#description' => t('The default %default_style results in a map of 300 pixels high, with a width bounded by the element that contains it. Do not enter quotes or equal signs.',
      array('%default_style' => IP_GEOLOC_MAP_DIV_DEFAULT_STYLE)),
  );

  switch ($delta) {

    case 'current_visitor_map':
      $form['ip_geoloc_current_visitor_map_options'] = array(
        '#type' => 'textfield',
        '#size' => 120,
        '#title' => t('Map options'),
        '#default_value' => variable_get('ip_geoloc_current_visitor_map_options', IP_GEOLOC_CURRENT_VISITOR_MAP_OPTIONS),
        '#description' => t("The default %default_options produces a street map zoomed in to level 15. A list of map options can be found <a href='!google_map_docs'>here</a>. Remember to separate options with comma's, not semi-colons, and make sure your quotes match.",
          array(
            '%default_options' => IP_GEOLOC_CURRENT_VISITOR_MAP_OPTIONS,
            '!google_map_docs' => IP_GEOLOC_DOC_GOOGLE_MAP_OPTIONS,
          )),
      );
      break;

    case 'recent_visitors_map':
      $form['ip_geoloc_recent_visitors_map_options'] = array(
        '#type' => 'textfield',
        '#size' => 120,
        '#title' => t('Map options'),
        '#default_value' => variable_get('ip_geoloc_recent_visitors_map_options', IP_GEOLOC_RECENT_VISITORS_MAP_OPTIONS),
        '#description' => t("The default %default_options produces a world map zoomed in to level 2. A list of map options can be found <a href='!google_map_docs'>here</a>. Remember to separate options with comma's, not semi-colons, and make sure your quotes match.",
          array(
            '%default_options' => IP_GEOLOC_RECENT_VISITORS_MAP_OPTIONS,
            '!google_map_docs' => IP_GEOLOC_DOC_GOOGLE_MAP_OPTIONS,
          )),
      );
      break;
  }
  return $form;
}

/**
 * Implements hook_block_save().
 */
function ip_geoloc_block_save($delta = '', $edit = array()) {
  if ($delta == 'address_lookup') {
    return;
  }
  if ($delta == 'geocode_address') {
    variable_set('ip_geoloc_visitor_address_editable', $edit['ip_geoloc_visitor_address_editable']);
    return;
  }
  // Save options common to all maps.
  variable_set('ip_geoloc_' . $delta . '_div_style', $edit['ip_geoloc_' . $delta . '_div_style']);
  ip_geoloc_save_map_options('ip_geoloc_' . $delta . '_options', $edit['ip_geoloc_' . $delta . '_options']);

  switch ($delta) {
    case 'recent_visitors_map':
      variable_set('ip_geoloc_recent_visitors_map_number_of_visitors', $edit['ip_geoloc_recent_visitors_map_number_of_visitors']);
      break;
  }
}

/**
 * Implements hook_block_view().
 */
function ip_geoloc_block_view($delta = '') {
  $block = array();
  $map_style = variable_get('ip_geoloc_' . $delta . '_div_style', IP_GEOLOC_MAP_DIV_DEFAULT_STYLE);

  switch ($delta) {

    case 'current_visitor_map':
      $block['subject'] = t('Your location');
      $map_options = variable_get('ip_geoloc_current_visitor_map_options', IP_GEOLOC_CURRENT_VISITOR_MAP_OPTIONS);
      $location = ip_geoloc_get_visitor_location();
      $block['content'] = theme(array('ip_geoloc_map_current_visitor'), array(
        'div_id' => 'ip-geoloc-block-current-visitor-map',
        'map_options' => $map_options,
        'map_style' => $map_style,
        // Lat,Lon NULL values will instigate HTML5 position retrieval. The
        // user will be prompted by the browser to give permission.
        'latitude'  => isset($location['latitude']) ? (float)$location['latitude'] : (empty($location['fixed_address']) ? NULL : 0.0),
        'longitude' => isset($location['longitude']) ? (float)$location['longitude'] : (empty($location['fixed_address']) ? NULL : 0.0),
        'info_text' => isset($location['formatted_address']) ? $location['formatted_address'] : NULL,
      ));
      break;

    case 'recent_visitors_map':
      $block['subject'] = t('Recent visitors');
      $how_many = variable_get('ip_geoloc_recent_visitors_map_number_of_visitors', 20);
      if ($how_many > 0) {
        $locations = ip_geoloc_get_recent_visitor_locations($how_many);
        $map_options = variable_get('ip_geoloc_recent_visitors_map_options', IP_GEOLOC_RECENT_VISITORS_MAP_OPTIONS);
        $block['content'] = theme(array('ip_geoloc_visitor_map'), array(
          'locations' => $locations,
          'div_id' => 'ip-geoloc-block-recent-visitors-map',
          'map_options' => $map_options,
          'map_style' => $map_style,
        ));
      }
      break;

    case 'address_lookup':
      $block['subject'] = t('Global address lookup');
      $block['content'] = drupal_get_form('ip_geoloc_address_lookup');
      break;

    case 'geocode_address':
      $block['subject'] = t('Refine your location');
      $block['content'] = drupal_get_form('ip_geoloc_geocode_address');
      break;

  }
  return $block;
}

/**
 * Save map options to a Drupal variable.
 *
 * @param string $var_name
 *   The variable name to save
 *
 * @param string $map_options
 *   The map options to save
 */
function ip_geoloc_save_map_options($var_name, $map_options) {
  $map_options_decoded = drupal_json_decode($map_options);
  if ($map_options_decoded == NULL) {
    drupal_set_message(t("Sytax error in map options. These map options may not work: '%map_options'", array('%map_options' => $map_options)), 'warning');
  }
  variable_set($var_name, $map_options);
}

/**
 * Get recent visitor locations.
 *
 * @param int $how_many
 *   how many locations to retrieve
 *
 * @return object
 *   object of locations found
 */
function ip_geoloc_get_recent_visitor_locations($how_many) {
  $locations = array();
  if (db_table_exists('accesslog')) {
    // A LEFT JOIN would also pick up new IP addresses that are about to be
    // inserted into the {accesslog}.
    // However a LEFT JOIN in this query can easily make it 100 times slower
    // than the INNER JOIN used below and would only be relevant for the very
    // first click from a new IP address or in the case where the IP address was
    // removed from the {accesslog}.
    $result = db_query_range('SELECT DISTINCT ip_address, latitude, longitude, formatted_address, COUNT(a.timestamp) AS visit_count, MAX(a.timestamp) AS last_visit FROM {ip_geoloc} i INNER JOIN {accesslog} a ON i.ip_address = a.hostname GROUP BY i.ip_address ORDER BY last_visit DESC', 0, $how_many);
    foreach ($result as $location) {
      // Prevent older IP address locations overwriting the latest location.
      if (!isset($locations[$location->ip_address])) {
        $loc_rendered = new stdClass();
        $loc_rendered->latitude = $location->latitude;
        $loc_rendered->longitude = $location->longitude;
        $loc_rendered->balloon_text = t('IP address') . ' ' . $location->ip_address . '<br/>'
         . $location->formatted_address . '<br/>'
         . t('#visits') . ' ' . (empty($location->visit_count) ? '?' : $location->visit_count)
         . ', ' . t('last visit') . ' ' . (empty($location->last_visit) ? '?' : format_date($location->last_visit, 'short'));
        $locations[$location->ip_address] = $loc_rendered;
      }
    }
  }
  return $locations;
}

/**
 * Generates a simple form for collecting the IP address to be reverse-geocoded.
 *
 * @ingroup forms
 */
function ip_geoloc_address_lookup($form, &$form_state) {
  $last_visit = isset($form_state['storage']['last_visit']) ? $form_state['storage']['last_visit'] : '';
  $formatted_address = isset($form_state['storage']['formatted_address']) ? $form_state['storage']['formatted_address'] : '';
  if (!empty($last_visit) || !empty($formatted_address)) {
    $prefix = '<div class="ip_geoloc_address_lookup">' . $formatted_address . (empty($last_visit) ? '' : "<br/>$last_visit") . '</div>';
  }
  $form['ip_address'] = array(
    '#type' => 'textfield',
    '#title' => t('IP address'),
    '#default_value' => '',
    '#size' => 16,
    '#required' => FALSE,
    '#prefix' => isset($prefix) ? $prefix : NULL,
  );
  if (user_access('administer site configuration')) {
    $form['store'] = array(
      '#type' => 'checkbox',
      '#title' => t('If found, store on IP geolocation database for future reference.'),
      '#default_value' => FALSE,
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Lookup'),
    '#submit' => array('ip_geoloc_lookup'),
  );
  return $form;
}

/**
 * Generates a simple form for collecting the address to be geocoded.
 *
 * @ingroup forms
 */
function ip_geoloc_geocode_address($form, &$form_state) {

  $location = ip_geoloc_get_visitor_location();
  $is_address_editable = variable_get('ip_geoloc_visitor_address_editable', TRUE);

  $form['street_address'] = array(
    '#type' => 'textfield',
    '#title' => t('Current approximate address'),
    '#default_value' => isset($location['formatted_address']) ? $location['formatted_address'] : '',
    '#disabled' => !$is_address_editable,
  );
  if ($is_address_editable) {
    $form['fixed_address'] = array(
      '#type' => 'checkbox',
      '#title' => t('Fix my location at this address'),
      '#default_value' => !empty($location['fixed_address']),
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit'),
      '#submit' => array('ip_geoloc_geocode'),
    );
  }
  $form['#attributes']['class'][] = 'ip-geoloc-address';
  $form['#attached']['css'][] = drupal_get_path('module', 'ip_geoloc') . '/css/ip_geoloc_all.css';
  return $form;
}

/**
 * Lookup geoloc data.
 */
function ip_geoloc_lookup($form, &$form_state) {
  $store = !empty($form_state['values']['store']);
  $location = ip_geoloc_get_location_by_ip(trim($form_state['values']['ip_address']), TRUE, $store);
  if (db_table_exists('accesslog')) {
    $last_visit = db_query('SELECT MAX(timestamp) FROM {accesslog} WHERE hostname = :ip_address', array(':ip_address' => $location['ip_address']))->fetchField();
    if ($last_visit) {
      $last_visit = format_date($last_visit, 'short');
      $form_state['storage']['last_visit'] = t('<strong>Last visit:</strong> %date', array('%date' => $last_visit));
    }
  }
  $form_state['storage']['formatted_address']
    = isset($location['formatted_address']) ? t('%ip: <strong>%address</strong>', array('%ip' => $location['ip_address'], '%address' => $location['formatted_address'])) : '';
  // To preserve entered values and storage array.
  $form_state['rebuild'] = TRUE;
}

/**
 * Geocode, using the Geocoder API, the submitted street address.
 *
 * Stores the geocoded lat/long on the session.
 * Modules may implement their own variations by implementing hook_form_alter()
 * and appending their own handler to $form['#submit'].
 *
 * @param array $form
 *   A form array
 * @param array $form_state
 *   The form's current state
 */
function ip_geoloc_geocode($form, &$form_state) {

  $location = _ip_geoloc_get_session_value('location');

  // Only Geocde when the "Fix my location at this address" was not ticked or
  // has just been ticked.
  if (empty($form_state['values']['fixed_address']) || empty($location['fixed_address'])) {

    $entered_address = trim($form_state['values']['street_address']);
    if (empty($entered_address)) {
      $location = array(
        'provider' => 'user',
        'ip_address' => ip_address(),
      );
    }
    elseif (module_exists('geocoder')) {
      // Use Google server-side API to retrieve lat/long from entered address.
      $point = geocoder('google', $entered_address);

      if (!$point) {
        drupal_set_message(t('The address you entered could not be geocoded to a location.'), 'warning');
        return;
      }

      $location = array(
        'provider' => 'user+google',
        'ip_address' => ip_address(),
        'latitude' => $point->coords[1],
        'longitude' => $point->coords[0],
        'formatted_address' => $point->data['geocoder_formatted_address'],
        'accuracy' => $point->data['geocoder_accuracy'],
      );
      // Flatten the point object into a straight location array.
      foreach ($point->data['geocoder_address_components'] as $component) {
        if (!empty($component->long_name)) {
          $type = $component->types[0];
          $location[$type] = $component->long_name;
          if ($type == 'country' && !empty($component->short_name)) {
            $location['country_code'] = $component->short_name;
          }
        }
      }
    }
  }
  $location['fixed_address'] = !empty($form_state['values']['fixed_address']);
  _ip_geoloc_set_session_value('location', $location);

  // Form always uses location from session.
  $form_state['rebuild'] = FALSE;
}
