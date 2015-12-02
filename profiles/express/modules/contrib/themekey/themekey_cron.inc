<?php

/**
 * @file
 * Provides everything that ThemeKey has to do when cron runs
 *
 * @author Markus Kalkbrenner | Cocomore AG
 *   @see http://drupal.org/user/124705
 */

/**
 * Checks rules containing time-based properties when cron runs.
 * ThemeKey will carefully clean up the page cache, if necessary,
 * to provide the right theme to anonymous users automatically,
 * e.g. a Christmas theme.
 */
function themekey_cron_clear_page_cache() {
  $clear_page_cache = FALSE;
  $rules_processed_new = array();

  if ($result = db_select('themekey_properties', 'tp')
      ->fields('tp')
      ->condition('enabled', 1)
      ->execute()
  ) {
    module_load_include('inc', 'themekey', 'themekey_base');

    $rules_processed = variable_get('themekey_cron_rules_processed', array());
    $attributes = variable_get('themekey_attributes', array());
    $parameters = themekey_get_global_parameters();

    foreach ($result as $item) {

      if (THEMEKEY_PAGECACHE_TIMEBASED == $attributes[$item->property]['page cache']) {
        $md5 = md5(serialize($item)); // use md5 instead of item id, because we want a track if an item's weight changes
        $match = themekey_match_condition($item, $parameters);
        $processed = in_array($md5, $rules_processed);
        if ($match && !$processed) {
          $clear_page_cache = TRUE;
          $rules_processed_new[] = $md5;
        }
        elseif (!$match && $processed) {
          $clear_page_cache = TRUE;
        }
        elseif ($match && $processed) {
          $rules_processed_new[] = $md5;
        }
      }

    }

  }

  if ($clear_page_cache) {
    // fast deletion of page cache (truncate)
    cache_clear_all('*', 'cache_page', TRUE);
  }

  variable_set('themekey_cron_rules_processed', $rules_processed_new);
}
