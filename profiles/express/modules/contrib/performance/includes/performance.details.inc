<?php

/**
 * @file
 * Performance module detail logging related functions. Can be overridden by
 * adding $conf['performance_detail_logging'] = 'path/to/your/file' in
 * settings.php.
 */

/**
 * Helper function to store detailed data in database.
 * @param $params
 *  an array containing the following keys: mem, timer, query_count, query_timer
 *  anon, path, language, data.
 * @return void
 */
function performance_log_details($params) {
  $fields = array(
    'timestamp' => REQUEST_TIME,
    'bytes' => $params['mem'],
    'ms' => (int)$params['timer'],
    'query_count' => $params['query_count'],
    'query_timer' => (int)$params['query_timer'],
    'anon' => $params['anon'],
    'path' => $params['path'],
    'language' => $params['language'],
    'data' => $params['data'],
  );

  try {
    db_insert('performance_detail')
      ->fields($fields)
      ->execute();
  }
  catch (Exception $e) {
    watchdog_exception('performance', $e, NULL, array(), WATCHDOG_ERROR);
  }
}

/**
 * Detail page callback.
 * @return array
 *  Drupal render array.
 */
function performance_view_details() {
  drupal_set_title(t('Performance logs: Details'));

  $header = array(
    array('data' => t('#'), 'field' => 'pid', 'sort' => 'desc'),
    array('data' => t('Path'), 'field' => 'path'),
    array('data' => t('Date'), 'field' => 'timestamp'),
    array('data' => t('Memory (MB)'), 'field' => 'bytes'),
    array('data' => t('ms (Total)'), 'field' => 'ms'),
    array('data' => t('Language'), 'field' => 'language'),
    array('data' => t('Anonymous?'), 'field' => 'anon'),
  );
  if (variable_get(PERFORMANCE_QUERY_VAR, 0)) {
    $header[] = array('data' => t('# Queries'), 'field' => 'query_count');
    $header[] = array('data' => t('Query ms'),  'field' => 'query_timer');
  }

  $pager_height = 50;
  $result = db_select('performance_detail', 'p')
    ->fields('p')
    ->extend('PagerDefault')
    ->limit($pager_height)
    ->extend('TableSort')
    ->orderByHeader($header)
    ->execute();

  $rows = array();

  foreach ($result as $data) {
    $row_data = array();
    $row_data[] = $data->pid;
    $row_data[] = l($data->path, $data->path);
    $row_data[] = format_date($data->timestamp, 'small');
    $row_data[] = number_format($data->bytes / 1024 / 1024, 2);
    $row_data[] = $data->ms;
    $row_data[] = $data->language;
    $row_data[] = ($data->anon) ? t('Yes') : t('No');

    if (variable_get(PERFORMANCE_QUERY_VAR, 0)) {
      $row_data[] = $data->query_count;
      $row_data[] = $data->query_timer;
    }

    $rows[] = array('data' => $row_data);
  }

  if (empty($rows) && !variable_get('performance_detail', 0)) {
    return array(
      'content' => array(
        '#markup' => t('Detail performance log is not enabled. Go to the !link to enable it.', array('!link' => l(t('settings page'), PERFORMANCE_SETTINGS, array('query' => drupal_get_destination()))))
      ),
    );
  }
  elseif (!variable_get('performance_detail', 0)) {
    drupal_set_message(t('Detail performance log is not enabled! Showing stored logs.'), 'warning');
  }

  // Return a renderable array.
  return array(
    'query_data_detail' => array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#sticky' => TRUE,
      '#empty' => t('No log messages available.'),
    ),
    'clear' => array(
      '#markup' => l(t('Clear logs'), 'admin/reports/performance-logging/clear/details'),
    ),
    'pager' => array(
      '#theme' => 'pager',
      '#quantity' => $pager_height,
    ),
  );
}

/**
 * Helper function to clear the detail logs.
 * @return void
 */
function performance_clear_details() {
  db_truncate('performance_detail')->execute();
}

/**
 * Helper function to prune detail log on cron.
 * @return void
 */
function performance_prune_details() {
  db_delete('performance_detail')
    ->condition('timestamp', REQUEST_TIME - (24 * 60 * 60), '<=')
    ->execute();
}
