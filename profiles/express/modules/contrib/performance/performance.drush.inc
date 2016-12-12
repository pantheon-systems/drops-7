<?php

/**
 * @file
 * Drush integration for the Performance module.
 */

/**
 * Implements hook_drush_command().
 */
function performance_drush_command() {
  $items = array();

  $limit = "Limit the number of records. Defaults to 25.";
  $direction = "Sort direction, defaults to descending: asc or desc.";

  $items['performance-summary'] = array(
    'callback' => 'drush_performance_summary',
    'description' => "Display the Performance Logging and Monitoring summary entries.",
    'aliases' => array('perf-sm'),
    'arguments' => array(
      'limit' => $limit,
      'orderby' => "The column name to sort on, defaults to last_access: path, last_access, bytes_max, bytes_avg, ms_max, ms_avg, [query_count_max, query_count_avg, query_timer_max, query_timer_avg] or num_accesses.",
      'direction' => $direction,
    ),
    'examples' => array(
      'performance-summary 25' => 'Retrieve last 25 entries.',
      'performance-summary 10 path' => 'Retrieve last 10 entries sorted by path, descending.',
      'performance-summary 40 bytes_avg asc' => 'Retrieve last 40 entries sorted by average size, ascending.',
    ),
  );

  $items['performance-detail'] = array(
    'callback' => 'drush_performance_details',
    'description' => "Display the Performance Logging and Monitoring detail entries.",
    'aliases' => array('perf-dt'),
    'arguments' => array(
      'limit' => $limit,
      'orderby' => "The column name to sort on, defaults to timestamp: pid, timestamp, bytes, ms, [query_count, query_timer], anon or path.",
      'direction' => $direction,
    ),
    'examples' => array(
      'performance-detail 15' => 'Retrieve last 15 entries.',
      'performance-detail 20 query_count' => 'Retrieve last 20 entries sorted by the number of queries, descending.',
      'performance-detail 35 bytes asc' => 'Retrieve last 35 entries sorted by size, ascending.',
    ),
  );

  return $items;
}

/**
 * Summary page callback.
 * Differs a little from the version in the module because drush_print_table()
 * works differently.
 */
function drush_performance_summary() {
  $args = func_get_args();

  // Collect arguments.
  $orderby = 'last_access';
  $columns = array('path', 'last_access', 'bytes_max', 'bytes_avg', 'ms_max', 'ms_avg', 'num_accesses');
  if (variable_get(PERFORMANCE_QUERY_VAR, 0)) {
    $columns[] = 'query_count_max';
    $columns[] = 'query_count_avg';
    $columns[] = 'query_timer_max';
    $columns[] = 'query_timer_avg';
  }
  $arguments = drush_performance_parse_args($args, $orderby, $columns);
  // Error thrown, abort.
  if (!is_array($arguments)) {
    return $arguments;
  }

  // Go back no more than 1 hour.
  $arguments['timestamp'] = REQUEST_TIME - 60 * 60;

  // Get actual data.
  $data_list = performance_traverse_cache('drush_performance_get_summary', $arguments);
  $data_list = performance_sort_summary($data_list, $arguments['direction'], $arguments['orderby']);

  if (empty($data_list) && !variable_get('performance_summary', 0)) {
    return drush_set_error(dt('Summary performance log is not enabled. Go to the settings page to enable it.'));
  }
  elseif (empty($data_list)) {
    drush_print("\033[1m" . dt('No log messages available.') . "\033[0m\n", 1);
    drush_print(dt('Possible causes:'), 1);
    drush_print('- ' . dt('no data stored yet'), 2);
    drush_print('- ' . dt('all data stored is older than one hour'), 2);
    return drush_print('- $base_url ' . dt('not properly set: run drush with the --uri parameter'), 2);
  }
  elseif (!variable_get('performance_summary', 0)) {
    drush_print("\033[1;33m" . dt('Summary performance log is not enabled! Showing stored logs.') . "\033[0m\n", 1);
  }

  // Build table header.
  $header = array(
    dt('Path'),
    dt('Last access'),
    dt('# accesses'),
    dt('MB Memory (Max)'),
    dt('MB Memory (Avg)'),
    dt('ms (Max)'),
    dt('ms (Avg)'),
  );
  if (variable_get(PERFORMANCE_QUERY_VAR, 0)) {
    $header[] = dt('Query ms (Max)');
    $header[] = dt('Query ms (Avg)');
    $header[] = dt('Query Count (Max)');
    $header[] = dt('Query Count (Avg)');
  }
  $rows[] = $header;

  // Format data into table.
  $threshold = variable_get('performance_threshold_accesses', 0);
  $total_rows = $shown = $last_max = $total_bytes = $total_ms = $total_accesses = 0;
  $last_min = REQUEST_TIME;

  foreach ($data_list as $data) {
    $total_rows++;
    $last_max = max($last_max, $data['last_access']);
    $last_min = min($last_min, $data['last_access']);

    // Calculate running averages.
    $total_bytes += $data['bytes_sum'] / $data['num_accesses'];
    $total_ms += $data['ms_sum'] / $data['num_accesses'];
    $total_accesses += $data['num_accesses'];

    $row_data = array();
    if ($data['num_accesses'] > $threshold) {
      $shown++;
      $row_data[] = check_plain($data['path']);
      $row_data[] = format_date($data['last_access'], 'small');
      $row_data[] = $data['num_accesses'];
      $row_data[] = number_format($data['bytes_max'] / 1024 / 1024, 2);
      $row_data[] = number_format($data['bytes_avg'] / 1024 / 1024, 2);
      $row_data[] = number_format($data['ms_max'], 1);
      $row_data[] = number_format($data['ms_avg'], 1);
      if (variable_get(PERFORMANCE_QUERY_VAR, 0)) {
        $row_data[] = number_format($data['query_timer_max'], 1);
        $row_data[] = number_format($data['query_timer_avg'], 1);
        $row_data[] = $data['query_count_max'];
        $row_data[] = $data['query_count_avg'];
      }
    }
    $rows[] = $row_data;
  }

  if (!$rows) {
    $rows[] = dt('No statistics available yet.');
  }

  if ($threshold) {
    drush_print("\n" . dt('Showing !shown paths with more than !threshold accesses, out of !total total paths.',
      array('!threshold' => $threshold, '!shown' => $shown, '!total' => $total_rows)));
  }
  else {
    drush_print("\n" . dt('Showing all !total paths.', array('!total' => $total_rows)));
  }

  // Protect against divide by zero.
  if ($total_rows > 0) {
    $mb_avg = number_format($total_bytes / $total_rows / 1024 / 1024, 1);
    $ms_avg = number_format($total_ms / $total_rows, 2);
  }
  else {
    $mb_avg = 'n/a';
    $ms_avg = 'n/a';
  }

  drush_print(dt('Average memory per page: !mb_avg MB', array('!mb_avg' => $mb_avg)));
  drush_print(dt('Average duration per page: !ms_avg ms', array('!ms_avg' => $ms_avg)));
  drush_print(dt('Total number of page accesses: !accesses', array('!accesses' => $total_accesses)));
  drush_print(dt('First access: !access.', array('!access' => format_date($last_min, 'small'))));
  drush_print(dt('Last access: !access.',  array('!access' => format_date($last_max, 'small'))) . "\n");

  drush_print("\033[1m" . dt('Performance log summary: !rows entries ordered by !column, !direction', array('!rows' => count($rows) - 1, '!column' => $arguments['orderby'], '!direction' => $arguments['direction'])) . "\033[0m", 1);

  // TODO: add 'running averages' here, like in the Drupal backend.
  drush_print_table($rows, TRUE);
}

function drush_performance_details() {
  $header = array(
    dt('#'),
    dt('Date'),
    dt('Path'),
    dt('Memory (MB)'),
    dt('ms (Total)'),
    dt('Anonymous?'),
  );
  if (variable_get(PERFORMANCE_QUERY_VAR, 0)) {
    $header[] = dt('# Queries');
    $header[] = dt('Query ms');
  }
  $rows[] = $header;

  // Collect arguments.
  $args = func_get_args();
  $orderby = 'timestamp';
  $columns = array('pid', 'timestamp', 'bytes', 'ms', 'anon', 'path');
  if (variable_get(PERFORMANCE_QUERY_VAR, 0)) {
    $columns[] = 'query_count';
    $columns[] = 'query_timer';
  }
  $arguments = drush_performance_parse_args($args, $orderby, $columns);

  // Error thrown, abort.
  if (!is_array($arguments)) {
    return $arguments;
  }

  $data_list = drush_performance_get_detail($arguments);

  if (empty($data_list) && !variable_get('performance_detail', 0)) {
    return drush_set_error(dt('Detail performance log is not enabled! Go to the settings page to enable it.'));
  }
  elseif (empty($data_list)) {
    return drush_print("\033[1m" . dt('No log messages available.') . "\033[0m", 1);
  }
  elseif (!variable_get('performance_detail', 0)) {
    drush_print("\033[1;33m" . dt('Detail performance log is not enabled! Showing stored logs.') . "\033[0m\n", 1);
  }

  foreach ($data_list as $data) {
    // Cast to object because of the DB API now returning row objects by default.
    $data = is_array($data) ? (object)$data : $data;
    $row_data = array();
    $row_data[] = $data->pid;
    $row_data[] = format_date($data->timestamp, 'small');
    $row_data[] = check_plain($data->path);
    $row_data[] = number_format($data->bytes/1024/1024, 2);
    $row_data[] = $data->ms;
    $row_data[] = ($data->anon) ? t('Yes') : t('No');

    if (variable_get(PERFORMANCE_QUERY_VAR, 0)) {
      $row_data[] = $data->query_count;
      $row_data[] = $data->query_timer;
    }

    $rows[] = $row_data;
  }

  drush_print("\033[1m" . dt('Performance log details: !rows entries ordered by !column, !direction', array('!rows' => count($rows) - 1, '!column' => $arguments['orderby'], '!direction' => $arguments['direction'])) . "\033[0m", 1);
  drush_print_table($rows, TRUE);
}

/**
 * Helper function to parse the arguments which are the same for both commands.
 * Only columns differ.
 */
function drush_performance_parse_args($args, $orderby, $columns) {
  $arguments = array();
  $default = TRUE;

  // Set limit from arguments or fall back to default.
  $arguments['limit'] = 25;
  if ((count($args) > 2 || count($args) == 1) && isset($args[0])) {
    if (is_numeric($args[0])) {
      $arguments['limit'] = $args[0];
    }
    else {
      return drush_set_error(dt('First argument must be numeric!'));
    }
  }
  else {
    // 1st parameter was most likely omitted, so we prepend the default to the
    // arguments to make the following checks check the right argument :-)
    array_unshift($args, $arguments['limit']);
  }

  // Order by column name.
  $arguments['orderby'] = $orderby;
  if (isset($args[1])) {
    if (in_array($args[1], $columns)) {
      $arguments['orderby'] = $args[1];
    }
    // Let the user know what the options are.
    else {
      return drush_set_error(dt('Unknown column name. Possible values are: !columns', array('!columns' => implode(', ', $columns))));
    }
  }

  // Sort direction.
  $arguments['direction'] = 'desc';
  $options = array('asc', 'desc');
  if (isset($args[2])) {
    if (in_array($args[2], $options)) {
      $arguments['direction'] = $args[2];
    }
    // Let the user know what the options are.
    else {
      return drush_set_error(dt('Unknown sort direction. Possible values are: !options', array('!options' => implode(', ', $options))));
    }
  }

  return $arguments;
}

/**
 * Callback used by performance_traverse_cache() for fetching summary data.
 *
 * @param $cache cache object
 * @param $arguments associative array containing limit and timestamp
 * @return the processed data or NULL
 *
 * @see performance_traverse_cache()
 */
function drush_performance_get_summary($cache, $arguments) {
  static $count = 0;

  if ($cache->data['last_access'] >= $arguments['timestamp'] && $count < $arguments['limit']) {
    $count++;
    return $cache->data;
  }

  return;
}

/**
 * Drupal version independant variation of performance_db_get_data() for easy
 * maintenance of the drush commands.
 */
function drush_performance_get_detail($arguments) {
  $data_list = array();

  $result = drush_db_select('performance_detail', '*', NULL, NULL, 0, $arguments['limit'], $arguments['orderby'], $arguments['direction']);
  if (drush_drupal_major_version() >= 7) {
    foreach ($result as $row) {
      $data_list[] = $row;
    }
  }
  else {
    while ($row = db_fetch_object($result)) {
      $data_list[] = $row;
    }
  }

  return $data_list;
}
