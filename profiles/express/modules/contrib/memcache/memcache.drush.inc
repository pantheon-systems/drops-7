<?php
/**
 * @file
 * Provides a drush interface to Memcached.
 */

/**
 * Implements hook_drush_command().
 */
function memcache_drush_command() {
  $items['memcache-stats'] = array(
    'description' => dt('Retrieve statistics from memcache.'),
    'arguments' => array(
      'bin' => dt('Optionally specify which bin; defaults to \'cache\'.'),
      'type' => dt('Optionally specify type of statistics; one of {default, reset, malloc, slabs, cachedump, items, sizes}.'),
    ),
    'options' => array(
      'interactive' => dt('Interactivly specify which type of statistics to display.'),
      'aggregate' => dt('Included combined statistics from all servers.'),
    ),
    'required-arguments' => 0,
    'examples' => array(
      'memcache-stats' => 'Display raw statistics.',
      'memcache-stats --aggregate' => 'Display raw statistics for all servers and combined totals',
      'memcache-stats --interactive' => 'Interactively select which type of statistics to display.',
      'memcache-stats cache slabs' => 'Display slabs allocated for cache bin.',
    ),
    'aliases' => array('mcs'),
  );

  $items['memcache-flush'] = array(
    'description' => dt('Flush all objects from a bin.'),
    'arguments' => array(
      'bin' => dt('Optionally specify which bin to flush; defaults to \'cache\'.'),
    ),
    'required-arguments' => 0,
    'examples' => array(
      'memcache-clear' => 'Flush all items in \'cache\' bin.',
      'memcache-clear cache-page' => 'Flush all items in \'cache-page\' bin.',
    ),
    'aliases' => array('mcf'),
  );

  return $items;
}

/**
 * Display memcache statistics.
 *
 * @param string $bin
 *   The bin to retrieve statistics for.
 * @param string $stats_type
 *   The type of statistics to retrieve when using the Memcache extension.
 */
function drush_memcache_stats($bin = 'cache', $stats_type = 'default') {
  if (_memcache_is_available()) {
    $interactive = drush_get_option('interactive', 0);
    $aggregate = drush_get_option('aggregate', 0);
    if ($interactive) {
      $options = array(
        'default' => 'default',
        'reset' => 'reset',
        'malloc' => 'malloc',
        'slabs' => 'slabs',
        'cachedump' => 'cachedump',
        'items' => 'items',
        'sizes' => 'sizes',
      );
      $stats_type = drush_choice($options, dt('What type of statistics would you like to see?'));
      // stats_type of FALSE === Cancel
      if ($stats_type === FALSE) {
        return;
      }
    }
    $stats = dmemcache_stats($bin, $stats_type, $aggregate);

    drush_print_r($stats);
  }
}

/**
 * Invalidate all items in specified bin.
 *
 * @param string $bin
 *   The bin to flush. Note that this will flush all bins mapped to the same
 *   server as $bin. There is no way at this time to empty just one bin.
 *
 * @return bool
 *   Returns TRUE on success or FALSE on failure.
 */
function drush_memcache_flush($bin = 'cache') {
  if (_memcache_is_available()) {
    $flushed = dmemcache_flush($bin);

    if ($flushed === TRUE) {
      drush_log(dt('Successfully cleared !bin bin.', array('!bin' => $bin)), 'ok');
    }
    else {
      drush_log(dt('Failed to clear !bin bin.', array('!bin' => $bin)), 'error');
    }
  }
}

/**
 * Implements drush_hook_COMMAND_validate().
 */
function drush_memcache_flush_validate() {
  return _memcache_is_available();
}

/**
 * Implements drush_hook_COMMAND_validate().
 */
function drush_memcached_stats_validate() {
  return _memcache_is_available();
}

/**
 * Check if memcache.inc has been included.
 */
function _memcache_is_available() {
  if (!class_exists('MemCacheDrupal', FALSE)) {
    return drush_set_error("MemCacheDrupal class is not available", "Please review README.txt and properly enable memcache.");;
  }
  return TRUE;
}
