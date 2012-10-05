<?php
// $Id$
/**
 * @file pantheon.api.inc
 * API functions for Pantheon
 */

define('YGGDRASIL_PORT', 8443);
define('VALHALLA_PORT', 448);

/**
 * Bulk delete in valhalla.
 *
 * @variable $paths = an array of file paths
 */
function pantheon_api_bulk_file_delete($paths) {
  $host = 'files.'. variable_get('pantheon_tier', 'live') .'.getpantheon.com';
  $url = "https://$host/sites/self/environments/self/files";
  $data = json_encode($paths);
  $result = pantheon_api_curl($url, $data, VALHALLA_PORT, 'DELETE');
}

/**
 * Helper function for running CURLs
 */
function pantheon_api_curl($url, $data = NULL, $port = 443, $verb = 'GET') {
  // create a new cURL resource
  $ch = curl_init();

  // set URL and other appropriate options
  $cert_path = '/srv/bindings/'. variable_get('pantheon_binding') .'/certs/binding.pem';
  $opts = array(
    CURLOPT_URL => $url,
    CURLOPT_HEADER => 1,
    CURLOPT_PORT => $port,
    CURLOPT_RETURNTRANSFER => 1,
    CURLOPT_SSLCERT => $cert_path,
    CURLOPT_HTTPHEADER => array('Content-Type: application/json'),
  );
  curl_setopt_array($ch, $opts);

  // If we are posting data...
  if ($data) {
    if ($verb == 'POST') {
      curl_setopt($ch, CURLOPT_POST, 1);
    }
    curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
  }

  curl_setopt($ch, CURLOPT_CUSTOMREQUEST, $verb);

  // grab URL and pass it to the browser
  $result = curl_exec($ch);

  if (curl_errno($ch) != 0) {
    $error = curl_error($ch);
    curl_close($ch);
    watchdog('pantheon', 'Error contacting API: !error', array('!error' => $error), WATCHDOG_EMERGENCY);
    return FALSE;
  }

  list($headers, $body) = explode("\r\n\r\n", $result);

  $return = array(
    'result' => $result,
    'headers' => $headers,
    'body' => $body,
    'opts' => $opts,
    'data' => print_r($data, 1),
  );

  // close cURL resource, and free up system resources
  curl_close($ch);

  return $return;
}
