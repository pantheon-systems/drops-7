<?php
/**
 * @file
 * ip_geoloc.session.inc
 *
 * Implements via Session Cache API one of several storage mechanisms for the
 * user's changing location details.
 */

/**
 * Set a value to the user's SESSION.
 *
 * @param string $name
 *   if FALSE all IPGV&M session variables are deleted;
 *   if NULL the array of all IPGV&M session variables is returned.
 *
 * @param mixed $value
 *   The value to set.
 *
 * @return array|null
 *   The requested or all session variables from ip_geoloc
 */
function _ip_geoloc_set_session_value($name, $value = NULL) {

  if (!module_exists('session_cache')) {
    if ($name === FALSE) {
      unset($_SESSION['ip_geoloc']);
    }
    elseif (isset($name)) {
      $_SESSION['ip_geoloc'][$name] = $value;
    }
    return isset($_SESSION) && isset($_SESSION['ip_geoloc']) ? $_SESSION['ip_geoloc'] : NULL;
  }

  if ($name === FALSE) {
    session_cache_set('ip_geoloc', NULL);
    return NULL;
  }

  // Efficiency: for multiple get's during the same request avoid having to go
  // to the storage mechanism each time.
  $variables = &drupal_static(__FUNCTION__, array());
  if (empty($variables)) {
    $variables = session_cache_get('ip_geoloc');
  }
  if (isset($name)) {
    $variables[$name] = $value;
    session_cache_set('ip_geoloc', $variables);
  }
  return $variables;
}

/**
 * Get a value from the user's SESSION or all values, if no name is specified.
 *
 * @param string $name
 *   The name of the variable to retrieve
 *
 * @return mixed
 *   The session value belonging to $name or all session values when name is
 *   omittted.
 */
function _ip_geoloc_get_session_value($name = NULL) {
  $variables = _ip_geoloc_set_session_value(NULL);
  return empty($name) ? $variables : (isset($variables[$name]) ? $variables[$name] : NULL);
}
