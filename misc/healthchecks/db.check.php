<?php
// Load environmental config, if present.
if (isset($_SERVER['PRESSFLOW_CONFIG'])) {
  $pressflow_config = json_decode($_SERVER['PRESSFLOW_CONFIG'], TRUE);
  $db = $pressflow_config['databases']['default']['default'];
  $link = mysql_connect($db['host'] . ':' . $db['port'], $db['username'], $db['password']);
  if (!$link) {
      die('Could not connect: ' . mysql_error());
  }
  echo "Connected successfully\n";
  mysql_close($link);
}
else {
  die("NO CONFIG FOUND\n");
}
