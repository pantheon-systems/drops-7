<?php
/**
 * @file
 *    helper functions for the Search Google Appliance module
 *
 * @see google_search_appliance.module
 */

/**
 * default module settings
 */
define('SGA_DEFAULT_HOSTNAME', '');
define('SGA_DEFAULT_COLLECTION', 'default_collection');
define('SGA_DEFAULT_FRONTEND', 'default_frontend');
define('SGA_DEFAULT_TIMEOUT', 10);
define('SGA_DEFAULT_AUTOFILTER', 1);
define('SGA_DEFAULT_LANGUAGE_FILTER_TOGGLE', FALSE);
define('SGA_DEFAULT_LANGUAGE_FILTER_OPTIONS', '');
define('SGA_DEFAULT_QUERY_INSPECTION', 0);
define('SGA_DEFAULT_SEARCH_TITLE', 'Search this site');
define('SGA_DEFAULT_RESULTS_PER_PAGE', 10);
define('SGA_DEFAULT_ADVANCED_SEARCH_REPORTING', FALSE);
define('SGA_DEFAULT_SPELLING_SUGGESTIONS', FALSE);
define('SGA_DEFAULT_ONEBOX_MODULES', '');
define('SGA_DEFAULT_BLOCK_VISIBILITY_SETTINGS', '');
define('SGA_DEFAULT_SITELINKS_SEARCH_BOX', FALSE);
define('SGA_DEFAULT_DRUPAL_PATH', 'gsearch');
define('SGA_DEFAULT_ERROR_GSA_NO_RESULTS', "<div class=\"item-list\">\n  <ul>\n    <li>Check if your spelling is correct.</li>\n    <li>Remove quotes around phrases: <em>bike parking</em> will often show more results than <em>\"bike parking\"</em>.</li>\n    <li>Try more general or different keywords.</li>\n  </ul>\n</div>");
define('SGA_DEFAULT_ERROR_GSA_NO_RESULTS_FORMAT', NULL);
define('SGA_DEFAULT_ERROR_CURL_ERROR', '<p>We cannot connect to the search service at this time. Please try your search again in a few minutes. If the problem persists, please contact us.</p>');
define('SGA_DEFAULT_ERROR_CURL_ERROR_FORMAT', NULL);
define('SGA_DEFAULT_ERROR_LIB_XML_PARSE_ERROR', '<p>The response from the search service was unreadable. Please try your search again. If the problem persists, please contact us.</p>');
define('SGA_DEFAULT_ERROR_LIB_XML_PARSE_ERROR_FORMAT', NULL);

/**
 * Returns all module settings for the Google Appliance module.
 *
 * @return array
 *    An associative array of module settings keyed by $field_keys.
 */
function _google_appliance_get_settings() {
  $field_keys =  array(
    'hostname',
    'collection',
    'frontend',
    'timeout',
    'autofilter',
    'language_filter_toggle',
    'language_filter_options',
    'query_inspection',
    'search_title',
    'results_per_page',
    'advanced_search_reporting',
    'spelling_suggestions',
    'onebox_modules',
    'block_visibility_settings',
    'sitelinks_search_box',
    'drupal_path',
    'error_gsa_no_results',
    'error_gsa_no_results_format',
    'error_curl_error',
    'error_curl_error_format',
    'error_lib_xml_parse_error',
    'error_lib_xml_parse_error_format',
  );

  // Settings fall back to default constants.
  foreach ($field_keys as $field) {
    $settings[$field] = variable_get(
      'google_appliance_' . $field,
      constant('SGA_DEFAULT_' . strtoupper($field))
    );
  }

  // Convert onebox_modules from string to array.
  $settings['onebox_modules'] = explode("\n", $settings['onebox_modules']);
  $settings['onebox_modules'] = array_map('trim', $settings['onebox_modules']);
  $settings['onebox_modules'] = array_filter($settings['onebox_modules'], 'strlen');

  // The empty string in the define block above for block visibility
  // settings is really just a flag that nothing has been set yet.
  // Using an empty string is problematic, so we just set it to an
  // empty array instead.
  if ($settings['block_visibility_settings'] === '') {
    $settings['block_visibility_settings'] = array();
  }
  if ($settings['language_filter_options'] === '') {
    $settings['language_filter_options'] = array();
  }

  return $settings;
}

/**
 * Send a POST requst using cURL
 * @param string $url to request
 * @param array $post values to send
 * @param array $options for cURL
 * @return string
 */
function _curl_post($url, array $post = NULL, array $options = array()) {
    $defaults = array(
        CURLOPT_POST => 1,
        CURLOPT_HEADER => 0,
        CURLOPT_URL => $url,
        CURLOPT_FRESH_CONNECT => 1,
        CURLOPT_RETURNTRANSFER => 1,
        CURLOPT_FORBID_REUSE => 1,
        CURLOPT_TIMEOUT => 4,
        CURLOPT_POSTFIELDS => http_build_query($post, '', '&')
    );

    $ch = curl_init();
    curl_setopt_array($ch, ($options + $defaults));
    $result = array(
      'is_error' => FALSE,
      'response' => curl_exec($ch),
    );
    if ($result['response'] === FALSE) {
      $result['is_error'] = TRUE;
      $result['response'] = curl_error($ch);
    }
    curl_close($ch);
    return $result;
}

/**
 * Send a GET requst using cURL
 * @param string $url to request
 * @param array $get values to send
 * @param array $options for cURL
 * @return string
 */
function _curl_get($url, array $get = NULL, array $options = array(), $sga_timeout = 30) {

  $defaults = array(
    CURLOPT_URL => $url . (strpos($url, '?') === FALSE ? '?' : '') . http_build_query($get, '', '&'),
    CURLOPT_HEADER => 0,
    CURLOPT_RETURNTRANSFER => TRUE,
    CURLOPT_TIMEOUT => check_plain($sga_timeout)
  );

  $ch = curl_init();
  curl_setopt_array($ch, ($options + $defaults));
  $result = array(
    'is_error' => FALSE,
    'response' => curl_exec($ch),
  );
  if ($result['response'] === FALSE) {
    $result['is_error'] = TRUE;
    $result['response'] = curl_error($ch);
  }
  curl_close($ch);
  return $result;
}

/**
 * report search errors to the log
 */
function _google_appliance_log_search_error($search_keys = NULL, $error_string = NULL) {
  $settings = _google_appliance_get_settings();

  // build log entry
  $type = 'google_appliance';
  $message = 'Search for %keys produced error: %error_string';
  $vars = array(
    '%keys' => isset($search_keys) ? $search_keys : 'nothing (empty search submit)',
    '%error_string' => isset($error_string) ? $error_string : 'undefinded error',
  );
  $link = l(t('view reproduction'), $settings['drupal_path'] . '/' . check_plain($search_keys));

  watchdog($type, $message, $vars, WATCHDOG_NOTICE, $link);
}

/**
 * Get googleon/googleoff tags.
 */
function _google_appliance_get_googleonoff() {
  return array(
    'index' => array(
      'prefix' => '<!--googleoff: index-->',
      'suffix' => '<!--googleon: index-->',
    ),
    'anchor' => array(
      'prefix' => '<!--googleoff: anchor-->',
      'suffix' => '<!--googleon: anchor-->',
    ),
    'snippet' => array(
      'prefix' => '<!--googleoff: snippet-->',
      'suffix' => '<!--googleon: snippet-->',
    ),
    'all' => array(
      'prefix' => '<!--googleoff: all-->',
      'suffix' => '<!--googleon: all-->',
    ),
  );
}

/**
 * Format the value of the 'lr' parameter appropriately.
 *
 * @param $options
 *   Array of languages to filter, as defined by the config
 *   variable google_appliance_language_filter_options.
 * @return
 *   String to be passed to the GSA using the 'lr' parameter.
 */
function _google_appliance_get_lr($options) {
  $langcodes = array();
  $options = array_filter($options);
  foreach ($options as $option) {
    switch ($option) {
      case '***CURRENT_LANGUAGE***':
        global $language;
        $langcode = $language->language;
        break;
      case '***DEFAULT_LANGUAGE***':
        $langcode = language_default('language');
        break;
      default:
        $langcode = $option;
    }
    $langcodes[$langcode] = "lang_$langcode";
  }
  return implode('|', $langcodes);
}
