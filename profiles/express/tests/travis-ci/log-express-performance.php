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

// Get environmental variables for sending to logstash
if (getenv('TRAVIS_PULL_REQUEST_BRANCH')) {
  $build_branch = getenv('TRAVIS_PULL_REQUEST_BRANCH');
}
else {
  $build_branch = getenv('TRAVIS_BRANCH');
}
$build_number = getenv('TRAVIS_BUILD_NUMBER');
$build_type = getenv('TRAVIS_EVENT_TYPE');
$build_repo = getenv('TRAVIS_REPO_SLUG');
$build_job_number = getenv('TRAVIS_JOB_NUMBER');

// Print out aggregate stats.
print_r('Average Memory Consumption: ' . $average_results['memory'] . "MB\n");
print_r('Average Load Time: ' . $average_results['load'] . " Milliseconds\n");

// Prep data to send to logstash
$data = array(
  'test_type' => 'full_test_run',
  'average_memory_consumption' => $average_results['memory'],
  'average_loadtime' => $average_results['load'],
  'build_job_number' => $build_job_number,
  'build_repo' => $build_repo,
  'build_branch' => $build_branch,
  'build_number' => $build_number,
  'build_type' => $build_type,
);
curl_logstash($data);

// Removed query stats due to memory load.
// print_r('Average Query Count: ' . $average_results['query_count'] . " Queries\n");
// print_r('Average Query Time: ' . $average_results['query_time'] . " Milliseconds\n");
print_r("\n");

// Build individual page output to screen. Only list top 15 pages by access count.
foreach ($output as $key => $path) {
  $count = count($path['memory']);
  $memory_sum = array_sum($path['memory']);
  $load_sum = array_sum($path['load']);

  // Removed query stats due to memory load.
  // $query_count_sum = array_sum($path['query_count']);
  // $query_time_sum = array_sum($path['query_timer']);

  $memory_average = number_format(($memory_sum / $count / 1000000), 2, '.', '');
  $load_average = $load_sum / $count;
  $query_count_average = number_format(($query_count_sum / $count), 2, '.', '');
  $query_time_average = number_format(($query_time_sum / $count), 2, '.', '');
  print_r('Path: ' . $path['path'] . "\n");
  print_r('Accessed: ' . $count . "\n");
  print_r('Memory Consumption: ' . $memory_average . "MB\n");
  print_r('Load Time: ' . $load_average . " Milliseconds\n");

  // Removed query stats due to memory load.
  // print_r('Query Count: ' . $query_count_average . " Queries\n");
  // print_r('Query Time: ' . $query_time_average . " Milliseconds\n");
  print_r("\n");

  // Prep data per test to send to logstash
  $data = array(
    'test_type' => 'indvidual_test',
    'path' => $path['path'],
    'accessed' => $count,
    'memory_consumption' => $memory_average,
    'loadtime' => $load_average,
    'build_branch' => $build_branch,
    'build_number' => $build_number,
    'build_type' => $build_type,
  );
 curl_logstash($data);
}

function curl_logstash($data) {
  $data_string = json_encode($data);

  # If data is not getting into the logging stack, check the IP range of the
  # TravisCI env. https://docs.travis-ci.com/user/ip-addresses/.
  # Currently is
  # workers-nat-org-shared-2.aws-us-east-1.travisci.net (52.45.185.117/32 52.54.31.11/32 54.87.185.35/32 54.87.141.246/32)
  $ch = curl_init();
  curl_setopt($ch, CURLOPT_URL, "http://wlogstash.colorado.edu:8080");
  curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "POST");
  curl_setopt($ch, CURLOPT_POSTFIELDS, $data_string);
  curl_setopt($ch, CURLOPT_TIMEOUT, 10);
  curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-Type: application/json','User-Agent: TravisCI'));

  if( ! $result = curl_exec($ch)) {
    print_r("cURL error: " . curl_error($ch) . "\n\n");
    $starttime = microtime(true);
    $file      = fsockopen ('wlogstash.colorado.edu', 8080, $errno, $errstr, 10);
    $stoptime  = microtime(true);
    $status    = 0;

    if (!$file) $status = -1;  // Site is down
    else {
        fclose($file);
        $status = ($stoptime - $starttime) * 1000;
        $status = floor($status);
    }
    print_r("fsock result: " . $status . "\n\n");
  } else {
    print_r("cURL result: " . $result . "\n\n");
  }
  curl_close($ch);
}
