#!/usr/bin/env php
<?php
/**
 * @file
 * Implement video rendering scheduling.
 * If you are not using sites/default/settings.php as your settings file,
 * add an optional parameter for the drupal site url:
 * "php video_scheduler.php http://example.com/" or
 * "php video_scheduler.php http://example.org/drupal/"
 *
 * @author Heshan Wanigasooriya <heshan at heidisoft dot com>
 */
/**
 * Drupal shell execution script
 */
$script = basename(array_shift($_SERVER['argv']));
$script_name = realpath($script);
$php_exec = realpath($_SERVER['PHP_SELF']);

$shortopts = 'hr:s:v';
$longopts = array('help', 'root:', 'site:', 'verbose');

$args = @getopt($shortopts, $longopts);

if (isset($args['h']) || isset($args['help'])) {
  echo <<<EOF
Drupal Video Module Transcoding Scheduler

Usage:        {$script} [OPTIONS]
Example:      {$script}

All arguments are long options.

  -h, --help  This page.

  -r, --root  Set the working directory for the script to the specified path.
              To execute Drupal this has to be the root directory of your
              Drupal installation, f.e. /home/www/foo/drupal (assuming Drupal
              running on Unix). Current directory is not required.
              Use surrounding quotation marks on Windows.

  -s, --site  Used to specify which hostname will be used to invoke Drupal.
              This option is required when your site is using the sites/default
              directory or when you are not executing this script from a
              sites/<sitename> directory.

To run this script without --root argument invoke it from the root directory
of your Drupal installation with

  ./{$script}

\n
EOF;
  if (version_compare(phpversion(), '5.3.0', 'le')) {
    echo "Warning: This version of PHP doesn't support long options\n";
  }
  exit;
}

// define default settings
$_SERVER['HTTP_HOST'] = 'localhost';
$_SERVER['PHP_SELF'] = '/index.php';
$_SERVER['REMOTE_ADDR'] = '127.0.0.1';
$_SERVER['SERVER_SOFTWARE'] = 'PHP CLI';
$_SERVER['REQUEST_METHOD'] = 'GET';
$_SERVER['QUERY_STRING'] = '';
$_SERVER['PHP_SELF'] = $_SERVER['REQUEST_URI'] = '/index.php';
$_SERVER['SCRIPT_NAME'] = '/' . basename($_SERVER['SCRIPT_NAME']);
$_SERVER['HTTP_USER_AGENT'] = 'console';

// Starting directory
$cwd = realpath(getcwd());

// parse invocation arguments
if (isset($args['r']) || isset($args['root'])) {
  // change working directory
  $path = isset($args['r']) ? $args['r'] : $args['root'];
  if (is_dir($path)) {
    chdir($path);
  }
  else {
    echo "\nERROR: {$path} not found.\n\n";
    exit(1);
  }
}
else {
  $path = $cwd;
  while ($path && !(file_exists($path . '/index.php') && file_exists($path . '/includes/bootstrap.inc'))) {
    $path = dirname($path);
  }

  if (!(file_exists($path . '/index.php') && file_exists($path . '/includes/bootstrap.inc'))) {
    echo "Unable to locate Drupal root, use -r option to specify path to Drupal root\n";
    exit(1);
  }
  chdir($path);
}

if (isset($args['s']) || isset($args['site'])) {
  $_SERVER['HTTP_HOST'] = isset($args['s']) ? $args['s'] : $args['site'];
}
else if (preg_match('/' . preg_quote($path . '/sites/', '/') . '(.*?)\//i', $cwd, $matches)) {
  if ($matches[1] != 'all' && file_exists('./sites/' . $matches[1])) {
    $_SERVER['HTTP_HOST'] = $matches[1];
  }
}

if ($_SERVER['HTTP_HOST'] == 'default') {
  echo "ERROR: You must use the --site option to set the hostname of your website.\n";
  exit(1);
}

define('DRUPAL_ROOT', realpath(getcwd()));

ini_set('display_errors', 0);
include_once DRUPAL_ROOT . '/includes/bootstrap.inc';
// Check if the site setting seems to be OK.
drupal_bootstrap(DRUPAL_BOOTSTRAP_CONFIGURATION);
if (empty($GLOBALS['databases'])) {
  if (isset($args['s']) || isset($args['site'])) {
    echo "ERROR: Drupal was unable to find the database settings. Check the --site setting.\n";
  }
  else {
    echo "ERROR: Drupal was unable to find the database settings. Use the --site setting to indicate to Drupal which site to use.\n";
  }
  exit(1);
}
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
ini_set('display_errors', 1);

// turn off the output buffering that drupal is doing by default.
ob_end_flush();

if (!class_exists('Transcoder', TRUE)) {
  echo "ERROR: The Video module doesn't seem to be installed.\n";
  exit(1);
}

// include our conversion class (also contains our defines)
$transcoder = new Transcoder();
$transcoder->runQueue();
