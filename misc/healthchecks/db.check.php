<?php
// Load environmental config, if present.
if (isset($_SERVER['PRESSFLOW_SETTINGS'])) {
  $pressflow_config = json_decode($_SERVER['PRESSFLOW_SETTINGS'], TRUE);
  $db = $pressflow_config['databases']['default']['default'];
  $link = mysql_connect($db['host'] . ':' . $db['port'], $db['username'], $db['password']);
  if (!$link) {
      fail('Could not connect: ' . mysql_error());
  }
  echo "OK\n";
  mysql_close($link);
}
else {
  fail("No config found.\n");
}

/**
 * Fail with a status code.
 */
function fail($message, $code = 500) {
  header(sprintf("HTTP/1.0 %s %s", $code, $message));
  echo $message;
  exit;
}
