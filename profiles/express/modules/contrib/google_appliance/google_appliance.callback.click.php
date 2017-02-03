<?php

/**
 * @file
 * The callback for the Google Appliance module's /click service.
 *
 * We do this rather than a Drupal menu callback to use as lightweight a Drupal
 * bootstrap as possible.
 */

/**
 * Returns parent directory.
 *
 * @param $path
 *   Path to start from.
 *
 * @return
 *   Parent path of given path.
 *
 * @see _drush_shift_path_up()
 */
function _google_appliance_shift_path_up($path) {
  if (empty($path)) {
    return FALSE;
  }
  $path = explode('/', $path);
  // Move one directory up.
  array_pop($path);
  return implode('/', $path);
}

/**
 * Checks whether given path qualifies as a Drupal root.
 *
 * @param $path
 *   Path to check.
 *
 * @return
 *   TRUE if given path seems to be a Drupal root, otherwise FALSE.
 *
 * @see drush_valid_drupal_root()
 */
function google_appliance_valid_drupal_root($path) {
  return !empty($path) && is_dir($path) && file_exists($path . '/includes/bootstrap.inc');
}

/**
 * Exhaustive depth-first search to try and locate the Drupal root directory.
 *
 * @param $start_path
 *   Search start path. Defaults to current working directory.
 *
 * @return
 *   A path to Drupal root, or FALSE if not found.
 *
 * @see drush_locate_root()
 */
function google_appliance_locate_root($start_path = NULL) {
  $drupal_root = FALSE;

  if (empty($start_path)) {
    $parts = parse_url($_SERVER['REQUEST_URI']);
    $start_path = $_SERVER['DOCUMENT_ROOT'] . $parts['path'];
  }
  foreach (array(TRUE, FALSE) as $follow_symlinks) {
    $path = $start_path;
    if ($follow_symlinks && is_link($path)) {
      $path = realpath($path);
    }
    // Check the start path.
    if (google_appliance_valid_drupal_root($path)) {
      $drupal_root = $path;
      break;
    }
    else {
      // Move up dir by dir and check each.
      while ($path = _google_appliance_shift_path_up($path)) {
        if ($follow_symlinks && is_link($path)) {
          $path = realpath($path);
        }
        if (google_appliance_valid_drupal_root($path)) {
          $drupal_root = $path;
          break 2;
        }
      }
    }
  }

  return $drupal_root;
}

// Try and load Drupal's bootstrap.inc.
define('DRUPAL_ROOT', google_appliance_locate_root());

require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
require_once DRUPAL_ROOT . '/includes/common.inc';

// Bootstrap Drupal and pull GSA configuration.
drupal_bootstrap(DRUPAL_BOOTSTRAP_VARIABLES);
$gsa_hostname = variable_get('google_appliance_hostname', FALSE);
$gsa_timeout  = variable_get('google_appliance_timeout', 10);

// Make sure we have a valid GSA host to connect to.
if (!$gsa_hostname) {
  watchdog('google_appliance', 'No valid GSA host to connect to.', array(), WATCHDOG_ERROR);
  exit;
}

// Filter parameters.
// @see https://www.google.com/support/enterprise/static/gsa/docs/admin/70/gsa_doc_set/xml_reference/advanced_search_reporting.html#1080237
$parameters_allowed = array(
  'cd' => 1,
  'ct' => 1,
  'q' => 1,
  'r' => 1,
  's' => 1,
  'url' => 1,
  'site' => 1,
);
$parameters = array_intersect_key($_GET, $parameters_allowed);

// Fire off the request to the GSA.
// Re-inject GET parameters received from the browser.
$url = url($gsa_hostname . '/click', array('query' => $parameters));
$result = drupal_http_request($url, array('timeout' => $gsa_timeout));

// If GSA did     return 204, then everything went well. Forward the response and prevent it from being cached.
// If GSA did not return 204, then something went wrong. Return 404 and log the error.
// @see https://www.google.com/support/enterprise/static/gsa/docs/admin/70/gsa_doc_set/xml_reference/advanced_search_reporting.html#1080347
switch ($result->code) {
  case 204:
    drupal_add_http_header('Status', '204 No Content');
    drupal_add_http_header('Content-Type', 'image/gif');
    drupal_add_http_header('Cache-Control', 'no-cache, no-store, must-revalidate');
    drupal_add_http_header('Pragma', 'no-cache');
    drupal_add_http_header('Expires', '0');
    break;
  default:
    drupal_add_http_header('Status', '404 Not Found');
    drupal_add_http_header('Cache-Control', 'no-cache, no-store, must-revalidate');
    drupal_add_http_header('Pragma', 'no-cache');
    drupal_add_http_header('Expires', '0');
    $message = 'GSA returned response code @code. Request: !url. Response: @response';
    $vars = array(
      '@code' => $result->code,
      '!url' => l($url, $url),
      '@response' => $result->status_message,
    );
    watchdog('google_appliance', $message, $vars, WATCHDOG_WARNING);
}
