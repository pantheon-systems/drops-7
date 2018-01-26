<?php

/**
 * @file
 * Drush command "lock_user_dev" for express_content_edit_lock module.
 */

/**
 * Implements hook_drush_command().
 */
function express_content_edit_lock_drush_command() {

  $commands['lock_user_dev'] = array(
    'description' => 'This logs everyone out and locks out non-developer roles from logging in.',
    'aliases' => array('lud'),
  );

  return $commands;
}

/**
 * Implements sessions table truncation on lock.
 */
function _logout_everyone() {
  $table = '{sessions}';
  db_truncate($table)->execute();
}

/**
 * Function displaying lock info.
 */
function drush_express_content_edit_lock_lock_user_dev() {
  $lud = variable_get('lock_user_dev');
  if ($lud === FALSE) {
    _logout_everyone();
    variable_set('lock_user_dev', TRUE);
    drush_print(dt('Users have been logged out and site is locked.'));
  } else {
    variable_set('lock_user_dev', FALSE);
    drush_print(dt('Site is unlocked.'));
  }
}

/**
 * Drush command logic.
 *
 * drush_lud().
 */
function express_content_edit_lock_command($arg1 = 'N/A', $arg2 = 'N/A') {
  $opt1 = drush_get_option('opt1', 'N/A');
  $tokens = array('@arg1' => $arg1, '@opt1' => $opt1);
  drush_print(dt('My custom command. Argument: @arg1 Option: @opt1', $tokens));
}
