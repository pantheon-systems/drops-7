<?php
/**
 * @file
 * Executes the environment_switch capabilities.
 */

/**
 * Implements hook_drush_command().
 */
function environment_drush_command() {
  $items = array();

  $items['environment-show'] = array(
    'description' => 'Show the current environment state.',
    'arguments' => array(
      'workflow' => 'Specify the workflow whose current environment state you wish to check. Leave blank for all workflows.',
    ),
    'drupal dependencies' => array('environment'),
    'aliases' => array('env', 'env-show', 'environment'),
  );

  $items['environment-switch'] = array(
    'description' => 'Switch the environment to specified state.',
    'drupal dependencies' => array('environment'),
    'arguments' => array(
      'target_env' => 'The target environment to which the site will be switched.',
    ),
    'options' => array(
      'force' => 'Whether an environment switch should be forced if the target enviornment is the same as the current environment.',
      'nocacheclear' => 'Switching environments clears the cache by default, this stops that.',
    ),
    'examples' => array(
      'drush environment-switch development' => 'Switch the environment to development.',
      'drush env-switch --force production' => 'Force the environment to switch to production even if the current environment already is production.',
      'drush env-switch --nocacheclear production' => 'Switch environments but do not clear cache.',
    ),
    'aliases' => array('env-switch'),
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 */
function environment_drush_help($section) {
  switch ($section) {
    case 'meta:environment:title':
      return dt('Environment commands');

    case 'meta:environment:summary':
      return dt('Review and control the site environment.');

    case 'drush:environment':
      return dt("Shows the current environment.");

    case 'drush:environment-switch':
      return dt("Switches the environment to the target environment.");
  }
}

/**
 * Implements drush_hook_COMMAND for environment.
 */
function drush_environment_show($workflow = NULL) {
  $func = drush_get_context('DRUSH_PIPE') ? 'drush_print_pipe' : 'drush_print_table';
  $func(environment_drush_render_current($workflow));
}

/**
 * Implements drush_hook_COMMAND_validate() for environment_switch.
 */
function drush_environment_switch_validate($target_env = NULL) {
  $environments = environment_load();
  if (empty($environments)) {
    return drush_set_error('DRUSH_ENVIRONMENT_ERROR', dt('There are no defined environments available.'));
  }
}

/**
 * Implements drush_hook_COMMAND for environment_switch.
 */
function drush_environment_switch($target_env = NULL) {
  if ($target_env && !array_key_exists($target_env, environment_load())) {
    drush_set_error('DRUSH_ENVIRONMENT_ERROR', dt('Invalid environment specified.'));
    $target_env = NULL;
  }
  if (!$target_env) {
    $environments = environment_load();
    $options = array();
    foreach ($environments as $name => $env) {
      $options[$name] = environment_drush_render_environment($env);
    }
    $target_env = drush_choice($options);
  }
  if (!$target_env) {
    return;
  }

  $force = drush_get_option('force');
  $no_cache_clear = drush_get_option('nocacheclear', FALSE);

  drush_print(dt("Switching the environment to !environment", array('!environment' => $target_env)));
  if (!drush_get_context('DRUSH_SIMULATE')) {
    $result = environment_switch($target_env, $force, !$no_cache_clear);
  }
  else {
    $result = TRUE;
  }

  if ($result) {
    drupal_set_message('Done.', 'success');
  }
  else {
    drupal_set_message('Failed.', 'error');
  }
}

/**
 * Render all current environment states with full description.
 *
 * @param string $workflow
 *   Optional; specify a single workflow whose environment to check. Otherwise
 *   show all states.
 *
 * @return array
 *   Array of strings describing the environment(s).
 */
function environment_drush_render_current($workflow = NULL) {
  $environments = (array) variable_get('environment', array());
  foreach ($environments as $environment) {
    $states[] = environment_load($environment);
  }

  $items = array();
  if (empty($states)) {
    drush_log('Your site environment is not set.', 'warning');
  }
  else {
    foreach ($states as $state) {
      if (is_null($workflow) || $state['workflow'] == $workflow) {
        if (!drush_get_context('DRUSH_PIPE')) {
          $items[] = array(
            environment_drush_render_environment($state, is_null($workflow), TRUE),
          );
        }
        else {
          $items[] = $state['name'];
        }
      }
    }
  }

  return $items;
}

/**
 * Render the specified environment definition as a descriptive one-liner.
 *
 * @param array $state
 *   Array defining an environment state.
 * @param bool $show_workflow
 *   Optional; default to TRUE. If there are more than one workflows defined in
 *   the system, will prefix the return with the workflow's label. If set to
 *   FALSE this is skipped. This is separate from $verbose because you could
 *   want a verbose description in a workflow-specific context.
 * @param bool $verbose
 *   Optional; defaults to FALSE. If TRUE, will include the environment
 *   description.
 *
 * @return string
 *   String describing the specified environment.
 */
function environment_drush_render_environment($state, $show_workflow = TRUE, $verbose = FALSE) {
  $description = $verbose && isset($state['description']) ? ': ' . $state['description'] : '';
  $prefix = '';

  if ($show_workflow) {
    $workflows = environment_load_workflow();
    $prefix .= count($workflows) > 1 ? $workflows[$state['workflow']]['label'] . ' : ' : '';
  }

  return $prefix . $state['label'] . ' (' . $state['name'] . ')' . $description;
}
