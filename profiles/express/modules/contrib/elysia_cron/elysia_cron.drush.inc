<?php

/**
 * @file
 * Drush integration for Elysia cron module.
 */

/**
 * Detect, is it drush or not.
 *
 * @return bool
 *   TRUE if code executed inside drush, FALSE otherwise.
 */
function elysia_cron_drush_detect() {
  return (!isset($_SERVER['SERVER_SOFTWARE']) && (php_sapi_name() == 'cli' || (is_numeric($_SERVER['argc']) && $_SERVER['argc'] > 0))) && function_exists('drush_main');
}

/**
 * Exit from drush execution.
 */
function elysia_cron_drush_die() {
  drush_set_context('DRUSH_EXECUTION_COMPLETED', TRUE);
  drupal_exit();
}

/**
 * Wrapper for drush_invoke().
 */
function elysia_cron_drush_invoke() {
  $args = drush_get_arguments();
  array_shift($args);

  // If drush command has no arguments or the first argument is not in the
  // list of allowed operations then we assume the cron execution.
  if (empty($args) || !in_array($args[0], array('list', 'run', 'enable', 'disable'))) {
    $args = array('run');
  }

  call_user_func_array('drush_elysia_cron_run_wrapper', $args);
  elysia_cron_drush_die();
}

/**
 * Implements hook_drush_command().
 */
function elysia_cron_drush_command() {
  $items['elysia-cron'] = array(
    'description' => dt('Run all cron tasks in all active modules for specified site using elysia cron system. This replaces the standard "core-cron" drush handler.'),
    'callback' => 'drush_elysia_cron_run_wrapper',
    'arguments' => array(
      'op' => 'Operation: list, run, disable, enable',
      'target' => 'Target of operation (optional): usually a task name or a channel name starting with "@"',
    ),
    'examples' => array(
      'elysia-cron run' => 'Run all cron tasks in all active modules (as the standard "core-cron")',
      'elysia-cron run --verbose' => 'Run all cron tasks in verbose mode',
      'elysia-cron run @channel' => 'Run all cron tasks in specified channel',
      'elysia-cron run search_cron --ignore-time' => 'Run only search index build task (force execution)',
      'elysia-cron list --elysia-cron-verbose' => 'List all channels/tasks in verbose mode',
      'elysia-cron disable search_cron' => 'Disable search index build task',
    ),
    'options' => array(
      'quiet' => 'suppress all output',
      'verbose' => 'enable extended output',
      'elysia-cron-verbose' => 'enable extended output (the same as --verbose, but without enabling drush verbose mode)',
      'force' => 'run channels/tasks even if it disabled/not ready for execution/already running',
      'ignore-disable' => 'run channels/tasks even if disabled',
      'ignore-time' => 'run channels/tasks even if not ready for execution',
      'ignore-running' => 'run channels/tasks even if already running',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  return $items;
}

/**
 * Custom callback for 'elysia-cron' drush command.
 */
function drush_elysia_cron_run_wrapper($op = FALSE, $target = FALSE) {
  global $_elysia_cron_drush;

  if (variable_get('maintenance_mode', FALSE)) {
    if (!variable_get('elysia_cron_run_maintenance', FALSE)) {
      drush_set_error('Cron run is not allowed in maintenance mode');
      return;
    }
  }

  $quiet = drush_get_option('quiet', FALSE);
  $verbose = drush_get_option('verbose', FALSE);
  if (!$verbose) {
    $verbose = drush_get_option('elysia-cron-verbose', FALSE);
  }

  if ($quiet) {
    $_elysia_cron_drush = 1;
  }
  elseif ($verbose) {
    $_elysia_cron_drush = 3;
  }
  else {
    $_elysia_cron_drush = 2;
  }

  switch ($op) {
    case 'list':
      global $_elysia_cron_settings_by_channel;
      elysia_cron_initialize();

      foreach ($_elysia_cron_settings_by_channel as $channel => $jobs) {
        $lines = array();
        if (!$verbose) {
          $line = array('@' . $channel);
        }
        else {
          $line = array('Channel: @' . $channel);
          if ($running = elysia_cron_is_channel_running($channel)) {
            $line[] = dt('Running, since !time', array('!time' => elysia_cron_date($running)));
          }
          if (!empty($jobs['#data']['disabled'])) {
            $line[] = dt('Disabled');
          }
          if (!$running) {
            $line[] = dt('Last run: !time', array('!time' => elysia_cron_date(_ec_variable_get('elysia_cron_last_run', 0))));
          }
        }
        drush_print(implode($line, ', '));

        foreach ($jobs as $job => $conf) {
          $line = array();
          if (strpos($job, '#') !== 0) {
            if (!$verbose) {
              drush_print($job);
            }
            else {
              $line['name'] = $job;
              $line['status'] = empty($conf['disabled']) ? dt('Enabled') : dt('Disabled');
              $line['run_status'] = empty($conf['running']) && elysia_cron_should_run($conf) ? dt('Ready to run') : dt('Waiting');
              $line['run_status'] = !empty($conf['running']) ? dt('Running, since !time', array('!time' => elysia_cron_date($conf['running']))) : $line['run_status'];
              $line['last_run'] = !empty($conf['last_run']) ? dt('Last run: !time', array('!time' => elysia_cron_date($conf['last_run']))) : '';
              $lines[] = $line;
            }
          }
        }
        if ($lines) {
          drush_print_table($lines);
        }
      }
      break;

    case 'run':
      // Collect options.
      $force = drush_get_option('force', FALSE);

      $ignore_disable = $force || drush_get_option('ignore-disable', FALSE);
      $ignore_time = $force || drush_get_option('ignore-time', FALSE);
      $ignore_running = $force || drush_get_option('ignore-running', FALSE);

      if (empty($target)) {
        elysia_cron_run(FALSE, $ignore_disable, $ignore_time, $ignore_running);
      }
      elseif (strpos($target, '@') === 0) {
        elysia_cron_initialize();
        if (elysia_cron_channel_exists(substr($target, 1))) {
          elysia_cron_run_channel(substr($target, 1), $ignore_disable, $ignore_time, $ignore_running);
        }
        else {
          drush_set_error('Channel ' . substr($target, 1) . ' does not exists');
        }
      }
      else {
        elysia_cron_initialize();
        if (elysia_cron_job_exists($target)) {
          elysia_cron_run_job($target, $ignore_disable, $ignore_time, $ignore_running);
        }
        else {
          drush_set_error('Job ' . $target . ' does not exists');
        }
      }
      break;

    case 'disable':
    case 'enable':
      if (!empty($target)) {
        if (strpos($target, '@') === 0) {
          elysia_cron_set_channel_disabled(substr($target, 1), $op == 'disable');
        }
        else {
          elysia_cron_set_job_disabled($target, $op == 'disable');
        }
        drush_log('Done', 'ok');
      }
      else {
        drush_set_error('Target not specified');
      }
      break;

    default:
      drush_print_help(drush_get_command());
      break;
  }
}
