<?php
#!/usr/bin/env drush

// Get all data the performance module has logged.
$results = db_query('SELECT * FROM {performance_detail} p')->fetchAll();

// Average results for aggregate output.
$average_results['memory'] = number_format(db_query('SELECT AVG(bytes) AS mem FROM {performance_detail} p')->fetchField() / 1000000, 2, '.', '');
$average_results['load'] = number_format(db_query('SELECT AVG(ms) AS lt FROM {performance_detail} p')->fetchField(), 2, '.', '');

// Removed query stats due to memory load.
// $average_results['query_count'] = number_format(db_query('SELECT AVG(query_count) AS qc FROM {performance_detail} p')->fetchField(), 2, '.', '');
// $average_results['query_time'] = number_format(db_query('SELECT AVG(query_timer) AS qt FROM {performance_detail} p')->fetchField(), 2, '.', '');

// Start outing results to the screen.
$output = array();

// Stash content for each path.
foreach ($results as $result) {
  $output[$result->path]['path'] = $result->path;
  $output[$result->path]['memory'][] = $result->bytes;
  $output[$result->path]['load'][] = $result->ms;

  // Removed query stats due to memory load.
  // $output[$result->path]['query_count'][] = $result->query_count;
  // $output[$result->path]['query_timer'][] = $result->query_timer;
}

// Sort the output of individual pages.
// It doesn't matter which stat is used to sort as the number of keys in the array
// tells you the number of times the page was accessed.
usort($output, function ($a, $b) {
  return count($a['memory']) < count($b['memory']);
});

// Print out aggregate stats.
print_r('Average Memory Consumption: ' . $average_results['memory'] . "MB\n");
print_r('Average Load Time: ' . $average_results['load'] . " Milliseconds\n");

// Removed query stats due to memory load.
// print_r('Average Query Count: ' . $average_results['query_count'] . " Queries\n");
// print_r('Average Query Time: ' . $average_results['query_time'] . " Milliseconds\n");

print_r("\n");

// Build individual page output to screen. Only list top 15 pages by access count.
$i = 0;
foreach ($output as $key => $path) {
  if ($i >= 15) {
    return;
  }

  $count = count($path['memory']);
  $memory_sum = array_sum($path['memory']);
  $load_sum = array_sum($path['load']);

  // Removed query stats due to memory load.
  // $query_count_sum = array_sum($path['query_count']);
  // $query_time_sum = array_sum($path['query_timer']);
  // $query_count_average = number_format(($query_count_sum / $count), 2, '.', '');
  // $query_time_average = number_format(($query_time_sum / $count), 2, '.', '');

  $memory_average = number_format(($memory_sum / $count / 1000000), 2, '.', '');
  $load_average = $load_sum / $count;

  print_r('Path: ' . $path['path'] . "\n");
  print_r('Accessed: ' . $count . "\n");
  print_r('Memory Consumption: ' . $memory_average . "MB\n");
  print_r('Load Time: ' . $load_average . " Milliseconds\n");

  // Removed query stats due to memory load.
  // print_r('Query Count: ' . $query_count_average . " Queries\n");
  // print_r('Query Time: ' . $query_time_average . " Milliseconds\n");
  print_r("\n");

  $i++;
}
