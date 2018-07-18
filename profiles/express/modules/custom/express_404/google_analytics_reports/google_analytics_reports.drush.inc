<?php

/**
 * @file
 * Drush integration for Google Analytics Reports module.
 */

/**
 * Implements hook_drush_command().
 */
function google_analytics_reports_drush_command() {
  $commands = array();

  $commands['google-analytics-reports-fields'] = array(
    'aliases' => array('garf'),
    'bootstrap' => DRUSH_BOOTSTRAP_NONE,
    'description' => dt('Import Google Analytics fields.'),
    'examples' => array(
      'drush google-analytics-reports-fields' => dt('Import Google Analytics fields for Google Analytics Reports module.'),
    ),
    'callback' => 'drush_google_analytics_reports_fields',
  );

  return $commands;
}

/**
 * Implements drush_MODULE_post_pm_enable().
 */
function drush_google_analytics_reports_post_pm_enable() {
  $modules = func_get_args();
  if (in_array('google_analytics_reports', $modules)) {
    drush_google_analytics_reports_fields();
  }
}

/**
 * Import Google Analytics fields.
 */
function drush_google_analytics_reports_fields() {
  $response = drupal_http_request('https://www.googleapis.com/analytics/v3/metadata/ga/columns');
  if ($response->code == '200') {
    // Remove old fields and clear cache.
    if (db_table_exists('google_analytics_reports_fields')) {
      db_truncate('google_analytics_reports_fields')->execute();
    }
    cache_clear_all('google_analytics_reports_fields', 'cache');

    $data = drupal_json_decode($response->data);
    // Save current time as last executed time.
    variable_set('google_analytics_reports_metadata_last_time', time());
    // Save etag identifier. It is used to check updates for the fields.
    // @see https://developers.google.com/analytics/devguides/reporting/metadata/v3/devguide#etag
    if (!empty($data['etag'])) {
      variable_set('google_analytics_reports_metadata_etag', $data['etag']);
    }

    if (!empty($data['items'])) {
      $context = array();
      foreach ($data['items'] as $item) {
        // Do not import deprecated fields.
        if ($item['attributes']['status'] == 'PUBLIC') {
          google_analytics_reports_fields_save($item, $context);
        }
      }
      drush_log(dt('Imported @count Google Analytics fields.', array('@count' => count($context['results']))), 'success');
    }
    else {
      drush_log(dt('An error has occurred during importing Google Analytics fields.'), 'error');
    }
  }
  else {
    drush_log(dt('There is a error during request to Google Analytics Metadata API: @error', array('@error' => $response->error)), 'error');
  }
}
