<?php

/**
 * @file
 * Features module drush integration.
 */

/**
 * Implements hook_drush_command().
 *
 * @return
 *   An associative array describing your command(s).
 *
 * @see drush_parse_command()
 */
function atlas_statistics_drush_command() {
  $items = array();

  $items['atlas-statistics'] = array(
    'description' => "Update the Atlas Statistic item.",
    'options' => array(
      'no-put' => "Show the Atlas Statistics, but do not send a PUT to Atlas.",
    ),
    'drupal dependencies' => array('atlas_statistics'),
    'aliases' => array('atlas-stats', 'atst'),
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 */
function atlas_statistics_drush_help($section) {
  switch ($section) {
    case 'drush:atlas-statistics':
      return dt("Send a PUT to the Atlas Statistic item for this site.");
  }
}

/**
 * PUT the Atlas Statistic item for this site.
 */
function drush_atlas_statistics()
{
  $atlas_id = variable_get('atlas_id', NULL);
  $site_array = atlas_api_request('sites', $atlas_id);
  $statistics_id = $site_array['statistics'];
  $statistics_array = atlas_api_request('statistics', $statistics_id);

  module_load_include('inc', 'atlas_statistics');
  $request_data = atlas_statistics_get_statistics();

  $no_put = drush_get_option('no-put');

  if ($no_put) {
    drush_print_r($request_data);
    return;
  }

  $request_data['_etag'] = $statistics_array['_etag'];
  $request_data['site'] = $atlas_id;

  $request = atlas_api_request('statistics', $statistics_id, 'PUT', $request_data);

  if ($request['_status'] == 'OK') {
    drush_print(t('Statistics item has been updated.'));
  }
}
