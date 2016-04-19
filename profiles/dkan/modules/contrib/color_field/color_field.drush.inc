<?php

/**
 * @file
 * Drush integration for color fields plugin.
 */

/**
 * The color plugin URI.
 */
define('COLOR_FIELD_SIMPLE_COLOR_PICKER_DOWNLOAD_URI', 'https://github.com/recurser/jquery-simple-color/archive/master.zip');
define('COLOR_FIELD_EYECON_COLOR_PICKER_DOWNLOAD_URI', 'http://www.eyecon.ro/colorpicker/colorpicker.zip');
define('COLOR_FIELD_DEMATTE_COLOR_PICKER_DOWNLOAD_URI', 'http://www.dematte.at/colorPicker/cP_v0.91.zip');
define('COLOR_FIELD_SPECTRUM_COLOR_PICKER_DOWNLOAD_URI', 'https://github.com/bgrins/spectrum/archive/master.zip');

/**
 * Implements hook_drush_command().
 */
function color_field_drush_command() {
  $items = array();

  $items['color-field-plugin-download'] = array(
    'description' => dt("Downloads a Color Picker plugin."),
    'arguments' => array(
      'path' => dt('Optional. A path where to install the Simple Color Picker plugin. If omitted Drush will use the default location.'),
    ),
    'aliases' => array('cfpd'),
  );

  $items['color-field-simple'] = array(
    'description' => dt("Downloads the Simple Color Picker plugin."),
    'arguments' => array(
      'path' => dt('Optional. A path where to install the Simple Color Picker plugin. If omitted Drush will use the default location.'),
    ),
  );

  $items['color-field-eyecon'] = array(
    'description' => dt("Downloads the EyeCon Color Picker plugin."),
    'arguments' => array(
      'path' => dt('Optional. A path where to install the EyeCon Color Picker plugin. If omitted Drush will use the default location.'),
    ),
  );

  $items['color-field-dematte'] = array(
    'description' => dt("Downloads the Dematte Color Picker plugin."),
    'arguments' => array(
      'path' => dt('Optional. A path where to install the Dematte Color Picker plugin. If omitted Drush will use the default location.'),
    ),
  );

  $items['color-field-spectrum'] = array(
    'description' => dt("Downloads the Spectrum Color Picker plugin."),
    'arguments' => array(
      'path' => dt('Optional. A path where to install the Spectrum Color Picker plugin. If omitted Drush will use the default location.'),
    ),
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 */
function color_field_drush_help($section) {
  switch ($section) {
    case 'drush:color-field-plugin-download':
      return dt("Downloads a Color Picker plugin, default location is sites/all/libraries.");
      break;
    case 'drush:color-field-simple':
      return dt("Downloads the Simple Color Picker plugin, default location is sites/all/libraries.");
      break;
    case 'drush:color-field-eyecon':
      return dt("Downloads the EyeCon Color Picker plugin, default location is sites/all/libraries.");
      break;
    case 'drush:color-field-dematte':
      return dt("Downloads the Dematte Color Picker plugin, default location is sites/all/libraries.");
      break;
    case 'drush:color-field-spectrum':
      return dt("Downloads the Spectrum Color Picker plugin, default location is sites/all/libraries.");
      break;
  }
}

/**
 * Commands to download a Color Picker plugin.
 */
function drush_color_field_plugin_download($path = 'sites/all/libraries', $plugin) {
  $options = array(
    'simple' => 'Simple Color Picker',
    'eyecon' => 'EyeCon Color Picker',
    'dematte' => 'Dematte Color Picker',
    'spectrum' => 'Spectrum Color Picker',
  );
  $choice = drush_choice($options, dt('Which plugin do you want to download?'));
  if ($choice) {
    drush_print(dt('Downloading @choice...', array('@choice' => $options[$choice])));
    $args = func_get_args();
    call_user_func_array('drush_color_field_' . $choice, $args);
  }
}

/**
 * Commands to download the Simple Color Picker plugin.
 */
function drush_color_field_simple($path = 'sites/all/libraries') {
  $filename = 'master.zip';
  $unzipfilename = 'jquery-simple-color-master';
  $dirname = 'jquery-simple-color';

  if (!drush_shell_exec('type unzip')) {
    return drush_set_error(dt('Missing dependency: unzip. Install it before using this command.'));
  }

  // Create the path if it does not exist.
  if (!is_dir($path)) {
    drush_op('mkdir', $path);
    drush_log(dt('Directory @path was created', array('@path' => $path)), 'notice');
  }

  // Set the directory to the download location.
  $olddir = getcwd();
  chdir($path);

  // Remove any existing jQuery Simple Color plugin directory.
  if (is_dir($dirname)) {
    drush_shell_exec('rm -rf ' . $dirname);
    drush_log(dt('An existing Simple Color Picker plugin was overwritten at @path', array('@path' => $path)), 'notice');
  }

  // Remove any existing jQuery Simple Color plugin zip archive.
  if (is_file($filename)) {
    drush_op('unlink', $filename);
  }

  // Download the zip archive.
  if (!drush_shell_exec('wget ' . COLOR_FIELD_SIMPLE_COLOR_PICKER_DOWNLOAD_URI)) {
    drush_shell_exec('curl -kOL ' . COLOR_FIELD_SIMPLE_COLOR_PICKER_DOWNLOAD_URI);
  }

  // If file has been downloaded.
  if (is_file($filename)) {
    // Remove old library.
    if (is_dir($dirname)) {
      drush_shell_exec('rm -rf ' . $dirname);
    }
    // Decompress the zip archive.
    drush_shell_exec('unzip -qq -o ' . $filename);
    drush_shell_exec('mv ' . $unzipfilename . ' ' . $dirname);
    // Remove the zip archive.
    drush_op('unlink', $filename);
  }

  // Set working directory back to the previous working directory.
  chdir($olddir);

  if (is_dir($path . '/' . $dirname)) {
    drush_log(dt('jQuery Simple Color plugin has been downloaded to @path', array('@path' => $path)), 'success');
  }
  else {
    drush_log(dt('Drush was unable to download the jQuery Simple Color  plugin to @path', array('@path' => $path)), 'error');
  }
}

/**
 * Commands to download the EyeCon Color Picker plugin.
 */
function drush_color_field_eyecon($path = 'sites/all/libraries') {
  $filename = 'colorpicker.zip';
  $dirname = 'eyecon-color-picker';

  if (!drush_shell_exec('type unzip')) {
    return drush_set_error(dt('Missing dependency: unzip. Install it before using this command.'));
  }

  // Create the path if it does not exist.
  if (!is_dir($path)) {
    drush_op('mkdir', $path);
    drush_log(dt('Directory @path was created', array('@path' => $path)), 'notice');
  }

  // Set the directory to the download location.
  $olddir = getcwd();
  chdir($path);

  // Remove any existing EyeCon Color Picker plugin directory.
  if (is_dir($dirname)) {
    drush_shell_exec('rm -rf ' . $dirname);
    drush_log(dt('An existing EyeCon Color Picker plugin was overwritten at @path', array('@path' => $path)), 'notice');
  }
  // Remove any existing EyeCon Color Picker plugin zip archive.
  if (is_file($filename)) {
    drush_op('unlink', $filename);
  }

  // Download the zip archive.
  if (!drush_shell_exec('wget ' . COLOR_FIELD_EYECON_COLOR_PICKER_DOWNLOAD_URI)) {
    drush_shell_exec('curl -kOL ' . COLOR_FIELD_EYECON_COLOR_PICKER_DOWNLOAD_URI);
  }

  if (is_file($filename)) {
    drush_shell_exec('mkdir ' . $dirname);
    // Decompress the zip archive.
    drush_shell_exec('unzip -qq -o ' . $filename);
    // Remove old librairy.
    // Remove the zip archive.
    drush_op('unlink', $filename);
    drush_shell_exec('mv css ' . $dirname);
    drush_shell_exec('mv images ' . $dirname);
    drush_shell_exec('mv js ' . $dirname);
    drush_shell_exec('mv index.html ' . $dirname);
  }

  // Set working directory back to the previous working directory.
  chdir($olddir);

  if (is_dir($path . '/' . $dirname)) {
    drush_log(dt('EyeCon Color Picker plugin has been downloaded to @path', array('@path' => $path)), 'success');
  }
  else {
    drush_log(dt('Drush was unable to download the EyeCon Color Picker plugin to @path', array('@path' => $path)), 'error');
  }
}

/**
 * Commands to download the Dematte Color Picker plugin.
 */
function drush_color_field_dematte($path = 'sites/all/libraries') {
  $filename = 'cP_v0.91.zip';
  $unzipfilename = 'cP_v0.91';
  $dirname = 'dematte-color-picker';

  if (!drush_shell_exec('type unzip')) {
    return drush_set_error(dt('Missing dependency: unzip. Install it before using this command.'));
  }

  // Create the path if it does not exist.
  if (!is_dir($path)) {
    drush_op('mkdir', $path);
    drush_log(dt('Directory @path was created', array('@path' => $path)), 'notice');
  }

  // Set the directory to the download location.
  $olddir = getcwd();
  chdir($path);

  // Remove any existing Dematte Color Picker plugin directory.
  if (is_dir($dirname)) {
    drush_log(dt('A existing Dematte Color Picker plugin was overwritten at @path', array('@path' => $path)), 'notice');
  }
  // Remove any existing Dematte Color Picker plugin zip archive.
  if (is_file($filename)) {
    drush_op('unlink', $filename);
  }

  // Download the zip archive.
  if (!drush_shell_exec('wget ' . COLOR_FIELD_DEMATTE_COLOR_PICKER_DOWNLOAD_URI)) {
    drush_shell_exec('curl -kOL ' . COLOR_FIELD_DEMATTE_COLOR_PICKER_DOWNLOAD_URI);
  }

  // If file has been downloaded.
  if (is_file($filename)) {
    // Decompress the zip archive.
    drush_shell_exec('unzip -qq -o ' . $filename);
    // Remove old library.
    if (is_dir($dirname)) {
      drush_shell_exec('rm -rf ' . $dirname);
    }
    drush_shell_exec('mv ' . $unzipfilename . ' ' . $dirname);
    // Remove the zip archive.
    drush_op('unlink', $filename);
  }

  // Set working directory back to the previous working directory.
  chdir($olddir);

  if (is_dir($path . '/' . $dirname)) {
    drush_log(dt('Dematte Color Picker plugin has been downloaded to @path', array('@path' => $path)), 'success');
  }
  else {
    drush_log(dt('Drush was unable to download the Dematte Color Picker plugin to @path', array('@path' => $path)), 'error');
  }
}

/**
 * Commands to download the Dematte Color Picker plugin.
 */
function drush_color_field_spectrum($path = 'sites/all/libraries') {
  $filename = 'master.zip';
  $unzipfilename = 'spectrum-master';
  $dirname = 'bgrins-spectrum';

  if (!drush_shell_exec('type unzip')) {
    return drush_set_error(dt('Missing dependency: unzip. Install it before using this command.'));
  }

  // Create the path if it does not exist.
  if (!is_dir($path)) {
    drush_op('mkdir', $path);
    drush_log(dt('Directory @path was created', array('@path' => $path)), 'notice');
  }

  // Set the directory to the download location.
  $olddir = getcwd();
  chdir($path);

  // Remove any existing Dematte Color Picker plugin directory.
  if (is_dir($dirname)) {
    drush_log(dt('A existing Spectrum Color Picker plugin was overwritten at @path', array('@path' => $path)), 'notice');
  }
  // Remove any existing Dematte Color Picker plugin zip archive.
  if (is_file($filename)) {
    drush_op('unlink', $filename);
  }

  // Download the zip archive.
  if (!drush_shell_exec('wget ' . COLOR_FIELD_SPECTRUM_COLOR_PICKER_DOWNLOAD_URI)) {
    drush_shell_exec('curl -kOL ' . COLOR_FIELD_SPECTRUM_COLOR_PICKER_DOWNLOAD_URI);
  }

  // If file has been downloaded.
  if (is_file($filename)) {
    // Decompress the zip archive.
    drush_shell_exec('unzip -qq -o ' . $filename);
    // Remove old library.
    if (is_dir($dirname)) {
      drush_shell_exec('rm -rf ' . $dirname);
    }
    drush_shell_exec('mv ' . $unzipfilename . ' ' . $dirname);
    // Remove the zip archive.
    drush_op('unlink', $filename);
  }

  // Set working directory back to the previous working directory.
  chdir($olddir);

  if (is_dir($path . '/' . $dirname)) {
    drush_log(dt('Spectrum Color Picker plugin has been downloaded to @path', array('@path' => $path)), 'success');
  }
  else {
    drush_log(dt('Drush was unable to download the Dematte Color Picker plugin to @path', array('@path' => $path)), 'error');
  }
}
