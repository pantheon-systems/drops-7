<?php

/**
 * @file
 *   drush integration for paymill.
 */


// The Paymill library download URI.
define('PAYMILL_DOWNLOAD_URI', 'https://github.com/Paymill/Paymill-PHP/archive/master.zip');
// The default path for installing the library.
define('COMMERCE_PAYMILL_LIBRARY_DEFAULT_PATH',
       sprintf('sites%sall%slibraries', DIRECTORY_SEPARATOR, DIRECTORY_SEPARATOR));

/**
 * Implements hook_drush_command().
 *
 * In this hook, you specify which commands your
 * drush module makes available, what it does and
 * description.
 *
 * Notice how this structure closely resembles how
 * you define menu hooks.
 *
 * See `drush topic docs-commands` for a list of recognized keys.
 *
 * @return
 *   An associative array describing your command(s).
 */
function commerce_paymill_drush_command() {
  $items = array();

  // The command requires no level of bootstrap whatsoever.
  $items['paymill-library'] = array(
    'callback' => 'drush_commerce_paymill_library',
    'description' => dt('Downloads the Paymill library.'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'arguments' => array(
      'path' => dt('Optional. A path where to install the Paymill library. If omitted Drush will use the default location: @default_path',
                   array('default_path' => COMMERCE_PAYMILL_LIBRARY_DEFAULT_PATH)),
    ),
    'aliases' => array('paymill'),
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 *
 * This function is called whenever a drush user calls
 * 'drush help <name-of-your-command>'
 *
 * @param string $section
 *   A string with the help section (prepend with 'drush:')
 *
 * @return string
 *   The help text for your command.
 */
function commerce_paymill_drush_help($section) {
  switch ($section) {
    case 'drush:paymill-library':
      return dt('Downloads the Paymill library from github, default location is @default_path.',
                array('@default_path' => COMMERCE_PAYMILL_LIBRARY_DEFAULT_PATH)
      );
  }
}

/**
 * Command to download the Paymill library.
 */
function drush_commerce_paymill_library() {
  if (!drush_shell_exec('type unzip')) {
    return drush_set_error(dt('Missing dependency: unzip. Install it before using this command.'));
  }

  $args = func_get_args();
  if ($args[0]) {
    $path = check_plain($args[0]);
  }
  else {
    $path = COMMERCE_PAYMILL_LIBRARY_DEFAULT_PATH;
  }

  // Create the path if it does not exist.
  if (!is_dir($path)) {
    drush_op('mkdir', $path);
    drush_log(dt('Directory @path was created.', array('@path' => $path)), 'notice');
  }

  // Set the directory to the download location.
  $olddir = getcwd();
  chdir($path);

  $filename = basename(PAYMILL_DOWNLOAD_URI);
  $dirname = 'paymill-php-master';

  // Remove any existing Paymill library zip archive.
  if (is_file($filename)) {
    drush_op('unlink', $filename);
  }

  // Download the zip archive.
  if (!drush_shell_exec('wget ' . PAYMILL_DOWNLOAD_URI)) {
    drush_shell_exec('curl -O ' . PAYMILL_DOWNLOAD_URI);
  }

  // Download the archive.
  if (is_file($filename)) {
    // Decompress the zip archive.
    drush_shell_exec('unzip -qq -o '. $filename);
    // Remove the zip archive.
    drush_op('unlink', $filename);
  }

  // Set working directory back to the previous working directory.
  chdir($olddir);

  // The full dirname.
  $dirname = $path . DIRECTORY_SEPARATOR . $dirname;
  // The destination directory.
  $dest = sprintf('%s%s%s', $path, DIRECTORY_SEPARATOR, 'paymill');

  // Check if the destination library exists. if it does remove it to begin
  // with a clean slate.
  if (is_dir($dest) && !drush_shell_exec(sprintf('rm -fr %s', $dest))) {
    drush_log(dt('Cannot remove existing library at @dest.', array('@dest' => $dest)), 'error');
  }

  // Install the library.
  if (is_dir($dirname)
      &&
      drush_shell_exec(sprintf('mv %s %s', $dirname, $dest))) {
    drush_log(dt('Paymill library has been downloaded to @path.', array('@path' => $path)), 'success');
  }
  else {
    drush_log(dt('Drush was unable to download the Paymill library to @path.', array('@path' => $path)), 'error');
  }
}

