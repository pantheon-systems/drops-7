<?php
/**
 * @file
 * Bootstraps Drupal 7 site.
 */

use Drupal\Driver\DrupalDriver;
use Drupal\Driver\Cores\Drupal7;

require 'vendor/autoload.php';

$dir = explode('/sites/', getcwd());

// No split may mean we are in a profiles folder:
if (count($dir) == 1) {
  $dir = explode('/profiles/', getcwd());
}

// Path to Drupal.
$path = $dir[0];

// Host.
$uri = 'http://localhost';

$driver = new DrupalDriver($path, $uri);
$driver->setCoreFromVersion();

// Bootstrap Drupal.
$driver->bootstrap();
