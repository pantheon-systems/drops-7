<?php


/**
 * @file
 *   ThemeKey API documentation
 */

/**
 * By Implementing hook_themekey_properties() it's
 * possible to add new properties to ThemeKey.
 *
 * Two assign a value to a property during a page request
 * you have three possibilities:
 * 1. Provide a mapping function from one property
 *    to another and tell ThemeKey about it using this hook
 * 2. Implement hook_themekey_global()
 * 3. Implement hook_themekey_paths()
 *
 * There's an example implementation of this hook,
 * @see themekey_example_themeley_properties()
 *
 * @return
 *   An array of ThemeKey properties and mapping functions:
 *     array of ThemeKey property attributes:
 *       key:   namespace:property
 *       value: array(
 *                description => Readable name of property (required)
 *                validator   => Callback function to validate a rule starting with that property (optional)
 *                               TODO: describe validator arguments and return value
 *                file        => File that provides the validator function (optional)
 *                path        => Alternative path relative to dupal's doc root to load the file (optional)
 *                static      => true/false, static properties don't occur in properties drop down
 *                               and have fixed operator and value (optional)
 *                page cache  => Level of page caching support:
 *                               - THEMEKEY_PAGECACHE_SUPPORTED
 *                               - THEMEKEY_PAGECACHE_UNSUPPORTED
 *                               - THEMEKEY_PAGECACHE_TIMEBASED
 *                               Default is THEMEKEY_PAGECACHE_UNSUPPORTED (optional)
 *              )
 *     array of mapping functions
 *       key:    none (indexed)
 *       value: array(
 *                src       => Source property path (required)
 *                dst       => Destination property path (required)
 *                callback  => Mapping callback (required)
 *                file      => File that provides the callback function (optional)
 *                path      => Alternative path relative to dupal's doc root to load the file (optional)
 *              )
 */
function hook_themekey_properties() {
  // Attributes of properties
  $attributes = array();

  $attributes['example:number_one'] = array(
    'description' => t('Example: always returns "1"'),
    'validator' => 'themekey_example_validator_number_one',
    'file' => 'themekey_example_validators.inc',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );

  $attributes['example:global_one'] = array(
    'description' => t('Example: always returns "1"'),
    'validator' => 'themekey_example_validator_number_one',
    'file' => 'themekey_example_validators.inc',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );

  $attributes['example:path_number'] = array(
    'description' => t('Example: always returns "1"'),
    'validator' => 'themekey_validator_ctype_digit',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );

  // Mapping functions
  $maps = array();

  $maps[] = array(
    'src' => 'system:dummy',
    'dst' => 'example:number_one',
    'callback' => 'themekey_example_dummy2number_one',
    'file' => 'themekey_example_mappers.inc',
  );

  return array('attributes' => $attributes, 'maps' => $maps);
}


/**
 * Functions implementing hook_themekey_global()
 * set some properties on every page request.
 *
 * So only easy stuff with low time and memory
 * consumtion should be done by
 * implementing hook_themekey_global().
 */
function hook_themekey_global() {

  $parameters = array();

  $parameters['example:global_one'] = "1";

  return $parameters;
}


/**
 * Functions implementing hook_themekey_paths()
 * set some properties on every page request.
 *
 * Using this function you directly map parts of
 * the path to property values.
 */
function hook_themekey_paths() {
  $paths = array();

  // a path like 'example/27/foo will set property example:path_number to '27'
  $paths[] = array('path' => 'example/#example:path_number');

  return $paths;
}
