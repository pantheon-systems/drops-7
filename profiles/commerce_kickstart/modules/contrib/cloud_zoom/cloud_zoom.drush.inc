<?php

/**
 * @file
 * This file integrates Cloud Zoom with Drush and provides a helper function for getting the library.
 */

function cloud_zoom_drush_command() {
  return array(
    'cloud-zoom-download' => array(
      'callback' => 'cloud_zoom_drush_download',
      'description' => dt('Downloads the required Cloud Zoom library from the authors website.'),
      'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
      'arguments' => array(
        'path' => dt('Optional. The path to the download folder. If omitted, Drush will use the default location (<code>sites/all/libraries/cloud-zoom</code>).'),
      ),
      'aliases' => array('cz-download', 'czd'),
    ),
  );
}


function cloud_zoom_drush_help($section) {
  switch ($section) {
    case 'drush:cloud-zoom-download' :
      return dt('Downloads the required Cloud Zoom library from the authors website, default location is sites/all/libraries/cloud-zoom.');
  }
}


function cloud_zoom_drush_download() {
  // Get the path from the argument, if site, or use the default.
  $args = func_get_args();
  if ($args[0]) {
    $path = $args[0];
  }
  else {
    $path = drush_get_context('DRUSH_DRUPAL_ROOT') . '/sites/all/libraries/cloud-zoom';
  }

  drush_log(dt('Download destination path: "@path"', array('@path' => $path)), 'notice');

  // Build the path, folder by folder. drush_mkdir doesn't seem to be recursive
  $path_components = array_filter(explode('/', $path));
  $subpath = '';
  while (count($path_components) && $subpath .= '/' . array_shift($path_components)) {
    // Create the path, if it doesn't exist
    if (!is_dir($subpath)) {
      drush_op('mkdir', $subpath);
      drush_log(dt('Directory @subpath was created', array('@subpath' => $subpath)), 'notice');
    }
  }


  // Set the directory to the download location.
  $olddir = getcwd();
  chdir($path);

  $filename = basename(CLOUD_ZOOM_DOWNLOAD_URI);
  $dirname = basename(CLOUD_ZOOM_DOWNLOAD_URI, '.zip');

  // Remove any existing Cloud Zoom plugin directory
  if (is_dir($dirname)) {
    drush_log(dt('A existing Cloud Zoom plugin was overwritten at @path', array('@path' => $path)), 'notice');
  }
  // Remove any existing Cloud Zoom plugin zip archive
  if (is_file($filename)) {
    drush_op('unlink', $filename);
  }

  // Download the zip archive
  if (!drush_shell_exec('wget ' . CLOUD_ZOOM_DOWNLOAD_URI)) {
    drush_shell_exec('curl -O ' . CLOUD_ZOOM_DOWNLOAD_URI);
  }

  if (is_file($filename)) {
    // Decompress the zip archive
    drush_shell_exec('unzip -qq -o ' . $filename);
    // Remove the zip archive
    drush_op('unlink', $filename);
  }

  // Set working directory back to the previous working directory.
  chdir($olddir);

  if (is_dir($path . '/' . $dirname)) {
    drush_log(dt('Drush was unable to download the Cloud Zoom plugin to @path', array('@path' => $path)), 'error');
  }
  else {
    drush_log(dt('Cloud Zoom plugin has been downloaded to @path', array('@path' => $path)), 'success');
  }
}
