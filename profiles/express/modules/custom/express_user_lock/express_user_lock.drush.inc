<?php

/**
 * Implements hook_drush_command().
 */
function express_user_lock_drush_command() {

  $commands['lock_user_dev'] = array(
    'description' => 'This logs everyone out and locks out non-developer roles from logging in.',
    'aliases' => array('lud'),
  );

  return $commands;
}

function _logout_everyone() {
  $table = '{sessions}';
  db_truncate($table)->execute();
}

function drush_express_user_lock_lock_user_dev() {
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
 * Change cron.
 */
function take_down_site_update_cron_schedule() {
  $atlas_id = variable_get('atlas_id', NULL);
  $site_array = atlas_api_request('sites', $atlas_id);

  if ($site_array['status'] === 'locked') {
    // Set options
    $default_options = array (
      '*/15 * * * *' => 'Every 15 minutes',
      '*/30 * * * *' => 'Every 30 minutes',
      '0 * * * *' => 'Every hour',
      '0 */6 * * *' => 'Every 6 hours',
      '0 */12 * * *' => 'Every 12 hours',
      '0 2 * * *' => 'Every day at 2am',
      '4 0 * * *' => 'Once a day',
      '4 0 * * 0' => 'Once a week',
      '4 0 1 * *' => 'Once a month',
    );

    variable_set('elysia_cron_default_rules', '0 */12 * * *');

    // Set values. Null uses default.
    variable_set('atlas_statistics_cron', '0 */12 * * *');
    variable_set('ctools_cron', NULL);
    variable_set('cu_redirect_cron', '0 */12 * * *');
    variable_set('cu_users_cron', '0 2 * * *');
    variable_set('field_cron', NULL);
    variable_set('googleanalytics_cron', '0 */12 * * *');
    variable_set('honeypot_cron', '*/15 * * * *');
    variable_set('ldap_servers_cron', NULL);
    variable_set('ldap_user_cron', '0 */12 * * *');
    variable_set('node_cron', NULL);
    variable_set('queue_file_entity_type_determine', '*/15 * * * *');
    variable_set('queue_views_bulk_operations', '*/15 * * * *');
    variable_set('redirect_cron', NULL);
    variable_set('system_cron', NULL);
    variable_set('user_external_invite_cron', '4 0 * * *');
    variable_set('views_bulk_operations_cron', '0 */12 * * *');
  }
}

/**
 * Drush command logic.
 * drush_lud().
 */
function express_user_lock_command($arg1 = 'N/A', $arg2 = 'N/A') {
  $opt1 = drush_get_option('opt1', 'N/A');
  $tokens = array('@arg1' => $arg1, '@opt1' => $opt1);
  drush_print(dt('My custom command. Argument: @arg1 Option: @opt1', $tokens));
}
