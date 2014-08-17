<?php

/**
 * @file
 * Hooks provided by "IP Geolocation Views & Maps" (ip_geoloc).
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * To hook in your own gelocation data provider or to modify the existing one.
 *
 * Note that when IPGV&M calls this function the $location object may be
 * partially fleshed out. If $location['ip_address'] is empty, this means that
 * IPGV&M is still waiting for more details to arrive from the Google
 * reverse-geocoding AJAX call. If $location['ip_address'] is not empty, then
 * IPGV&M does not expect any further details and will store the $location
 * with your modifications (if any) on the IP geolocation database. You must set
 * $location['formatted_address'] in order for the location to be stored.
 *
 * @param array $location
 *   The location to alter.
 */
function hook_get_ip_geolocation_alter(&$location) {
  if (empty($location['ip_address'])) {
    return;
  }
  $location['provider'] = 'MYMODULE';
  // ....
  $location['city'] = $location['locality'];
}

/**
 * Modify the array of locations coming from the View before they're mapped.
 *
 * @param array $marker_locations
 *   An array of marker locations.
 * @param object $view
 *   The view from which $marker_locations was generated.
 */
function hook_ip_geoloc_marker_locations_alter(&$marker_locations, &$view) {
  // The $marker_location->marker_color has to be the name (without extension)
  // of one of the files in the ip_geoloc/markers directory, or alternative,
  // if configured at admin/config/system/ip_geoloc.
  // The code below changes the color of the first two markers returned by the
  // View to orange and yellow and then prepends an additional marker, not in
  // the View.
  // Because the marker is added at the front of the location array, the map can
  // be centered on it. Or you can choose one of the other centering options, as
  // per normal.

  // Machine name of your view goes in the line below.
  if ($view->name == 'my_beautiful_view') {
    if (count($marker_locations) >= 2) {
      $marker_locations[0]->marker_color = 'orange';
      $marker_locations[1]->marker_color = 'yellow';
    }
    $observatory = new stdClass();
    $observatory->latitude = 51.4777;
    $observatory->longitude = -0.0015;
    $observatory->balloon_text = t('The zero-meridian passes through the courtyard of the <strong>Greenwich</strong> observatory.');
    $observatory->marker_color = 'white';
    
    array_unshift($marker_locations, $observatory);
  }
}

/**
 * @} End of "addtogroup hooks".
 */
