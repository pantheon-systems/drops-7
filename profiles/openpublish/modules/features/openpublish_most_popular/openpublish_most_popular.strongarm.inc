<?php
/**
 * @file
 * openpublish_most_popular.strongarm.inc
 */

/**
 * Implements hook_strongarm().
 */
function openpublish_most_popular_strongarm() {
  $export = array();

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ga_stats_enabled_metrics';
  $strongarm->value = array(
    'pageviews' => 'pageviews',
    'uniquePageviews' => 0,
    'avgTimeOnPage' => 0,
    'entrances' => 0,
  );
  $export['ga_stats_enabled_metrics'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ga_stats_enabled_timeframes';
  $strongarm->value = array(
    'today' => 'today',
    '2days' => '2days',
    'week' => 'week',
    'hour' => 0,
    'month' => 0,
    'forever' => 0,
  );
  $export['ga_stats_enabled_timeframes'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'ga_stats_max_results';
  $strongarm->value = '100';
  $export['ga_stats_max_results'] = $strongarm;

  return $export;
}
