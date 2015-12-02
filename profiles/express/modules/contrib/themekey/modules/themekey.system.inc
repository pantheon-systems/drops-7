<?php

/**
 * @file
 * Provides some ThemeKey properties.
 *
 * @author Markus Kalkbrenner | Cocomore AG
 *   @see http://drupal.org/user/124705
 *
 * @author profix898
 *   @see http://drupal.org/user/35192
 */


/**
 * Implements hook_themekey_properties().
 *
 * Provides additional properties for module ThemeKey:
 * - system:host
 * - drupal:path
 * - drupal:path:wildcard
 *
 * @return
 *   array of themekey properties and mapping functions
 */
function themekey_system_themekey_properties() {
  // Attributes for properties
  $attributes = array();

  $attributes['system:host'] = array(
    'description' => t('System: HTTP_HOST - The hostname/domain of the site without http:// or https://, like "www.drupal.org" or "drupal.cocomore.com"'),
    'validator' => 'themekey_validator_http_host',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );

  $attributes['system:query_param'] = array(
    'description' => t("System: Query Parameter - Every single query parameter other than 'q' and its value, if present. Note that values are url decoded. Example: '?q=node&foo=bar&dummy&filter=tid%3A27' will cause three entries 'foo=bar', 'dummy' and 'filter=tid:27'. For 'q', see property drupal:get_q."),
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );

  $attributes['system:query_string'] = array(
    'description' => t("System: Query String - Complete query string except parameter 'q'. Example: '?q=node&foo=bar&dummy&filter=tid%3A27' will result in 'foo=bar&dummy&filter=tid%3A27'. For 'q' see property drupal:get_q."),
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );

  $attributes['system:cookie'] = array(
    'description' => t("System: Cookie - Every single cookie and its value if present will be formatted like 'COOKIE_NAME=COOKIE_VALUE'"),
    'page cache' => THEMEKEY_PAGECACHE_UNSUPPORTED,
  );

  $attributes['system:server_ip'] = array(
    'description' => t("System: 'SERVER_ADDR' - The IP address of the server under which the current script is executing."),
    'validator' => 'themekey_validator_ip_address',
    'page cache' => THEMEKEY_PAGECACHE_UNSUPPORTED,
  );

  $attributes['system:server_port'] = array(
    'description' => t("System: 'SERVER_PORT' - The port on the server machine being used by the web server for communication. For default setups, this will be '80'; using SSL, for instance, will change this to whatever your defined secure HTTP port is."),
    'validator' => 'themekey_validator_ctype_digit',
    'page cache' => THEMEKEY_PAGECACHE_UNSUPPORTED,
  );

  $attributes['system:server_name'] = array(
    'description' => t("System:  'SERVER_NAME' - The name of the server host under which the current script is executing. If the script is running on a virtual host, this will be the value defined for that virtual host."),
    'page cache' => THEMEKEY_PAGECACHE_UNSUPPORTED,
  );

  $attributes['system:https'] = array(
    'description' => t("System: 'HTTPS' - Set to 'true' value if the script was queried through the HTTPS protocol, otherwise 'false'."),
    'validator' => 'themekey_validator_string_boolean',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );

  $attributes['system:remote_ip'] = array(
    'description' => t("System: 'REMOTE_ADDR' - The IP address from which the user is viewing the current page."),
    'validator' => 'themekey_validator_ip_address',
    'page cache' => THEMEKEY_PAGECACHE_UNSUPPORTED,
  );

  $attributes['system:referer'] = array(
    'description' => t("System: 'HTTP_REFERER' - The address of the page (if any) which referred the user agent to the current page. This is set by the user agent. Not all user agents will set this, and some provide the ability to modify HTTP_REFERER as a feature. In short, it cannot really be trusted."),
    'page cache' => THEMEKEY_PAGECACHE_UNSUPPORTED,
  );

  $attributes['system:user_agent'] = array(
    'description' => t("System: 'HTTP_USER_AGENT' - Contents of the User-Agent: header from the current request, if there is one. This is a string denoting the user agent which is accessing the page. A typical example is: Mozilla/4.5 [en] (X11; U; Linux 2.2.9 i586)."),
    'page cache' => THEMEKEY_PAGECACHE_UNSUPPORTED,
  );

  $attributes['system:user_browser'] = array(
    'description' => t("System: Browser - Information about user's browser. Possible values: 'Blazer [VERSION]', 'Internet Explorer [VERSION]', 'Mozilla Firefox [VERSION]', 'Netscape [VERSION]', 'Google Chrome [VERSION]', 'Safari [VERSION]', 'Galeon [VERSION]', 'Konqueror [VERSION]', 'Gecko based', 'Opera [VERSION]', 'Lynx [VERSION]', 'Fennec [VERSION]', 'Maemo [VERSION]', 'unknown'"),
    'page cache' => THEMEKEY_PAGECACHE_UNSUPPORTED,
  );

  $attributes['system:user_browser_simplified'] = array(
    'description' => t("System: Browser - Simplified information about user's browser. Possible values: 'Blazer', 'Internet Explorer', 'Mozilla Firefox', 'Netscape', 'Google Chrome', 'Safari', 'Galeon', 'Konqueror', 'Gecko based', 'Opera', 'Lynx', 'Fennec', 'Maemo', 'unknown'"),
    'page cache' => THEMEKEY_PAGECACHE_UNSUPPORTED,
  );

  $attributes['system:user_os'] = array(
    'description' => t("System: Operating System - Information about user's browser. Possible values: 'Windows XP', 'Windows Vista', 'Windows 98', 'Windows 2000', 'Windows 2003 server', 'Windows NT',  'Windows ME', 'Windows CE', 'Windows ME', 'iPhone', 'iPad', 'Mac OS X', 'Macintosh', 'Linux', 'Free BSD', 'Symbian', 'webOS', 'Android', 'Blackberry', 'unknown'"),
    'page cache' => THEMEKEY_PAGECACHE_UNSUPPORTED,
  );

  $attributes['system:user_os_simplified'] = array(
    'description' => t("System: Operating System - Simplified information about user's browser. Possible values: 'Windows', 'iPhone', 'iPad', 'Mac OS X', 'Macintosh', 'Linux', 'Free BSD', 'Symbian', 'webOS', 'Android', 'Blackberry', 'unknown'"),
    'page cache' => THEMEKEY_PAGECACHE_UNSUPPORTED,
  );

  $attributes['system:date_time'] = array(
    'description' => t("System: Date Time - Current time formatted as Y-m-d H:i:s (example: 2009-12-24 18:30:10)"),
    'validator' => 'themekey_validator_date_time',
    'page cache' => THEMEKEY_PAGECACHE_TIMEBASED,
  );

  $attributes['system:date'] = array(
    'description' => t("System: Date - Current time formatted as Y-m-d (example: 2009-12-24)"),
    'validator' => 'themekey_validator_date',
    'page cache' => THEMEKEY_PAGECACHE_TIMEBASED,
  );

  $attributes['system:time'] = array(
    'description' => t("System: Time - Current time formatted as H:i:s (example: 18:30:10)"),
    'validator' => 'themekey_validator_time',
    'page cache' => THEMEKEY_PAGECACHE_TIMEBASED,
  );

  $attributes['system:day_of_week'] = array(
    'description' => t("System: Day of Week - Current day of the week formatted as three letters 'Mon' through 'Sun'"),
    'validator' => 'themekey_validator_day_of_week',
    'page cache' => THEMEKEY_PAGECACHE_TIMEBASED,
  );

  $attributes['system:dummy'] = array(
    'description' => t("System: Dummy - Dummy property. Value is always 'dummy'"),
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );

  $attributes['system:session'] = array(
    'description' => t("System: Session - Every single session parameter and its value, if a value is present and a simple type (string, integer, boolean). Booleans are represented as integers; '1' as TRUE and '0' as FALSE."),
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );

  $attributes['system:random'] = array(
    'description' => t("System: Random - A random value that could be '0' or 1'."),
    'validator' => 'themekey_validator_nummeric_boolean',
    'page cache' => THEMEKEY_PAGECACHE_UNSUPPORTED,
  );

  $attributes['drupal:path'] = array(
    'description' => t('Drupal: Drupal path like "node/add/story" or path alias with support for wildcards.<br />
Query parameters are stripped off before the path gets examined. P.e. "node/add/story?destination=node" becomes "node/add/story" first. If you want to access query parameters, have a look at the system:query_param property provided by !link.<br />
Wildcard characters are "#" for numeric parts and "%" for all characters. To match conditions against a certain part, use an identifier with the wildcard. For example "comment/reply/#xyz" matches all paths with "comment/reply" and a numeric third argument. You can then specify conditions for every wildcard argument using the property "drupal:path:wildcard" and the identifier you choose ("xyz" in this example).<br />
These are the possible wildcard replacements for foo/bar/42/test.html:', array('!link' => l(t('!path', array('!path' => 'ThemeKey Properties')), 'http://drupal.org/project/themekey_properties'))) .
'<pre>
foo/bar/42/test.html
foo/bar/42/%
foo/bar/42
foo/bar/%/test.html
foo/bar/%/%
foo/bar/%
foo/bar/#/test.html
foo/bar/#/%
foo/bar/#
foo/bar
foo/%/42/test.html
foo/%/42/%
foo/%/42
foo/%/%/test.html
foo/%/%/%
foo/%/%
foo/%/#/test.html
foo/%/#/%
foo/%/#
foo/%
foo
%/bar/42/test.html
%/bar/42/%
%/bar/42
%/bar/%/test.html
%/bar/%/%
%/bar/%
%/bar/#/test.html
%/bar/#/%
%/bar/#
%/bar
%/%/42/test.html
%/%/42/%
%/%/42
%/%/%/test.html
%/%/%/%
%/%/%
%/%/#/test.html
%/%/#/%
%/%/#
%/%
%
</pre>',
    'validator' => 'themekey_validator_drupal_path',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );
  $attributes['drupal:path:wildcard'] = array(
    'description' => t('Wildcard of "drupal:path". See explanation of "drupal:path" for details.'),
    'validator' => 'themekey_validator_wildcard',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );
  $attributes['drupal:get_q'] = array(
    'description' => t('Drupal: $_GET[\'q\'] - Current value of Drupal\'s query parameter "q"'),
    'validator' => '',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );

  $attributes['drupal:base_path'] = array(
    'description' => t("Drupal: Base path - If Drupal is installed in a subdirectory, the base path will be '/[subdirectory]/', otherwise just '/'."),
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );

  $attributes['drupal:is_front_page'] = array(
    'description' => t("Drupal: Is front page - 'true' if current page is front page, otherwise just 'false'."),
    'validator' => 'themekey_validator_string_boolean',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );

  $maps = array();
  $maps[] = array('src'       => 'system:dummy',
                  'dst'       => 'system:query_param',
                  'callback'  => 'themekey_dummy2query_param');
  $maps[] = array('src'       => 'system:dummy',
                  'dst'       => 'system:cookie',
                  'callback'  => 'themekey_dummy2cookie');
  $maps[] = array('src'       => 'system:dummy',
                  'dst'       => 'system:query_string',
                  'callback'  => 'themekey_dummy2query_string');
  $maps[] = array('src'       => 'system:dummy',
                  'dst'       => 'system:date_time',
                  'callback'  => 'themekey_dummy2date_time');
  $maps[] = array('src'       => 'system:dummy',
                  'dst'       => 'system:date',
                  'callback'  => 'themekey_dummy2date');
  $maps[] = array('src'       => 'system:dummy',
                  'dst'       => 'system:time',
                  'callback'  => 'themekey_dummy2time');
  $maps[] = array('src'       => 'system:dummy',
                  'dst'       => 'system:day_of_week',
                  'callback'  => 'themekey_dummy2day_of_week');
  $maps[] = array('src'       => 'system:dummy',
                  'dst'       => 'system:user_browser',
                  'callback'  => 'themekey_dummy2user_browser');
  $maps[] = array('src'       => 'system:dummy',
                  'dst'       => 'system:user_os',
                  'callback'  => 'themekey_dummy2user_os');
  $maps[] = array('src'       => 'system:user_browser',
                  'dst'       => 'system:user_browser_simplified',
                  'callback'  => 'themekey_user_browser2user_browser_simplified');
  $maps[] = array('src'       => 'system:user_os',
                  'dst'       => 'system:user_os_simplified',
                  'callback'  => 'themekey_user_os2user_os_simplified');
  $maps[] = array('src'       => 'system:dummy',
                  'dst'       => 'system:session',
                  'callback'  => 'themekey_dummy2session');


  return array('attributes' => $attributes, 'maps' => $maps);
}


/**
 * Implements hook_themekey_global().
 */
function themekey_system_themekey_global() {
  global $user;

  $parameters = array();
  $parameters['system:host'] = $_SERVER['HTTP_HOST'];
  $parameters['drupal:get_q'] = themekey_get_q();
  $parameters['system:server_ip'] = !empty($_SERVER['SERVER_ADDR']) ? $_SERVER['SERVER_ADDR']: NULL;
  $parameters['system:server_port'] = !empty($_SERVER['SERVER_PORT']) ? $_SERVER['SERVER_PORT']: NULL;
  $parameters['system:server_name'] = !empty($_SERVER['SERVER_NAME']) ? $_SERVER['SERVER_NAME']: NULL;
  // Note that when using ISAPI with IIS, the value will be 'off' if the request was not made through the HTTPS protocol.
  $parameters['system:https'] = (!empty($_SERVER['HTTPS']) && 'off' != $_SERVER['HTTPS']) ? 'true' : 'false';
  $parameters['system:remote_ip'] = !empty($_SERVER['REMOTE_ADDR']) ? $_SERVER['REMOTE_ADDR']: NULL;
  $parameters['system:referer'] = !empty($_SERVER['HTTP_REFERER']) ? $_SERVER['HTTP_REFERER']: NULL;
  $parameters['system:user_agent'] = !empty($_SERVER['HTTP_USER_AGENT']) ? $_SERVER['HTTP_USER_AGENT']: NULL;
  $parameters['system:dummy'] = 'dummy';
  $parameters['system:random'] = rand(0, 1);

  $parameters['drupal:base_path'] = base_path();
  $parameters['drupal:is_front_page'] = drupal_is_front_page() ? 'true' : 'false';

  return $parameters;
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: system:dummy
 * dst: system:query_param
 *
 * @param $dummy
 *   string containing current value of ThemeKey property system:dummy
 *
 * @return
 *   array of system:query_param values
 *   or NULL if no value could be mapped
 */
function themekey_dummy2query_param($dummy) {
  $filtered_params = array();
  $query_params = $_GET;
  unset($query_params['q']);
  foreach ($query_params as $key => $value) {
    $filtered_params[] = $key . (!empty($value) ? '=' . $value : '');
  }
  return count($filtered_params) ? array_unique($filtered_params) : NULL;
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: system:dummy
 * dst: system:cookie
 *
 * @param $dummy
 *   string containing current value of ThemeKey property system:dummy
 *
 * @return
 *   array of system:cookie values
 *   or NULL if no value could be mapped
 */
function themekey_dummy2cookie($dummy) {
  $filtered_cookies = array();
  foreach ($_COOKIE as $key => $value) {
    $filtered_cookies[] = $key . (!empty($value) ? '=' . $value : '');
  }
  return count($filtered_cookies) ? array_unique($filtered_cookies) : NULL;
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: system:dummy
 * dst: system:query_string
 *
 * @param $dummy
 *   string containing current value of ThemeKey property system:dummy
 *
 * @return
 *   system:query_string as string
 *   or NULL if no value could be mapped
 */
function themekey_dummy2query_string($dummy) {
  if (!empty($_SERVER['QUERY_STRING'])) {
    $query_string = trim(preg_replace("/q=[^&]*/", '', $_SERVER['QUERY_STRING']), '&');
    if (!empty($query_string)) {
      return $query_string;
    }
  }

  return NULL;
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: system:dummy
 * dst: system:date_time
 *
 * @param $dummy
 *   string containing current value of ThemeKey property system:dummy
 *
 * @return
 *   current time as string formatted like "2009-12-24 23:56:15"
 */
function themekey_dummy2date_time($dummy) {
  return format_date(time(), 'custom', 'Y-m-d H:i:s', variable_get('date_default_timezone', 0));
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: system:dummy
 * dst: system:date
 *
 * @param $dummy
 *   string containing current value of ThemeKey property system:dummy
 *
 * @return
 *   current date as string formatted like "2009-12-24"
 */
function themekey_dummy2date($dummy) {
  return format_date(time(), 'custom', 'Y-m-d', variable_get('date_default_timezone', 0));
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: system:dummy
 * dst: system:time
 *
 * @param $dummy
 *   string containing current value of ThemeKey property system:dummy
 *
 * @return
 *   current time as string formatted like "23:56:15"
 */
function themekey_dummy2time($dummy) {
  return format_date(time(), 'custom', 'H:i:s', variable_get('date_default_timezone', 0));
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: system:dummy
 * dst: system:user_browser
 *
 * @param $dummy
 *   string containing current value of the ThemeKey property, system:dummy
 *
 * @return
 *   current user's browser as string
 */
function themekey_dummy2user_browser($dummy) {
  return ThemekeyBrowserDetection::getBrowser($_SERVER['HTTP_USER_AGENT']);
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: system:dummy
 * dst: system:user_os
 *
 * @param $dummy
 *   string containing current value of ThemeKey property system:dummy
 *
 * @return
 *   current user's operating system as string
 */
function themekey_dummy2user_os($dummy) {
  return ThemekeyBrowserDetection::getOs($_SERVER['HTTP_USER_AGENT']);
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: system:user_browser
 * dst: system:user_browser_simplified
 *
 * @param $dummy
 *   string containing current value of ThemeKey property system:dummy
 *
 * @return
 *   current user's browser as simplified string
 */
function themekey_user_browser2user_browser_simplified($user_browser) {
  return ThemekeyBrowserDetection::getBrowserSimplified($user_browser);
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: system:user_os
 * dst: system:user_os_simplified
 *
 * @param $dummy
 *   string containing current value of ThemeKey property system:user_os
 *
 * @return
 *   current user's operating system as simplified string
 */
function themekey_user_os2user_os_simplified($user_os) {
  return ThemekeyBrowserDetection::getOsSimplified($user_os);
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: system:dummy
 * dst: system:session
 *
 * @param $dummy
 *   string containing current value of ThemeKey property system:dummy
 *
 * @return
 *   array of system:session values
 *   or NULL if no value could be mapped
 */
function themekey_dummy2session($dummy) {
  $filtered_params = array();
  if (!empty($_SESSION)) {
    foreach ($_SESSION as $key => $value) {
      if (is_bool($value)) {
        $filtered_params[] = $key . '=' . ($value) ? '1' : '0';
      }
      elseif (!empty($value) && (is_numeric($value) || is_string($value))) {
        $filtered_params[] = $key . '=' . $value;
      }
      else {
        $filtered_params[] = $key;
      }
    }
  }
  return count($filtered_params) ? array_unique($filtered_params) : NULL;
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: system:dummy
 * dst: system:day_of_week
 *
 * @param $dummy
 *   string containing current value of ThemeKey property system:dummy
 *
 * @return
 *   current day of the week as three letters from "Mon" through "Sun"
 */
function themekey_dummy2day_of_week($dummy) {
  return format_date(time(), 'custom', 'D', variable_get('date_default_timezone', 0));
}


