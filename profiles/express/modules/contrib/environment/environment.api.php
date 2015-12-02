<?php
/**
 * @file
 * Hooks provided by Environment.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * React to an environment state change.
 *
 * Use this hook to specify changes to your site configuration depending on
 * what kind of environment the site is operating in. For example, production
 * environments should not have developer/site-builder oriented modules enabled,
 * such as administrative UI modules.
 *
 * When defining your state change actions, be careful to account for a given
 * state always consisting of the same behaviors and configuration, regardless
 * of how it returns to that state (which previous environment it was in.) Be
 * careful that you do not *disable* any modules in one environment that
 * implement a necessary instance of hook_environment_switch().
 *
 * @param string $target_env
 *   The name of the environment being activated.
 * @param string $current_env
 *   The name of the environment being deactivated.
 * @param string $workflow
 *   The name of the environment workflow whose current state is being switched.
 *   A "NULL" workflow is the default/generic/unspecified workflow for the site.
 *
 * @return string
 *   String summarizing changes made for drush user.
 */
function hook_environment_switch($target_env, $current_env, $workflow = NULL) {
  // Declare each optional development-related module.
  $devel_modules = array(
    'devel',
    'devel_generate',
    'devel_node_access',
  );

  switch ($target_env) {
    case 'production':
      module_disable($devel_modules);
      drupal_set_message('Disabled development modules');
      return;

    case 'development':
      module_enable($devel_modules);
      drupal_set_message('Enabled development modules');
      return;
  }
}

/**
 * Declare additional environments.
 *
 * This hook is to facilitate UI building and restricting environment switch to
 * known environments.
 *
 * @return array
 *   Array of environment names in the format:
 *   - label: Human-readable name for the environment.
 *   - description: Description of the environment and it's purpose.
 *   - workflow: Tag the state with the machine name of the environment
 *     workflow.
 *   - allowed: Central definition of permitted operations for the
 *     environment_allowed() function. Default FALSE indicates that something
 *     should not happen, such as show the user a debugging message. Different
 *     categories can be specified for different rulesets.
 *
 * @see environment_allowed
 */
function hook_environment() {
  $environments = array();

  $environments['stage'] = array(
    'label' => t('Staging'),
    'description' => t('Staging sites are for content creation before publication.'),
    'allowed' => array(
      'default' => FALSE,
      'email' => FALSE,
    ),
  );
  $environment['internal'] = array(
    'label' => t('Internal-only site'),
    'description' => t('Internal sites are not available for live access.'),
    'workflow' => 'public',
  );
  $environment['live'] = array(
    'label' => t('Live site'),
    'description' => t('Live sites are in full production and browsable on the web.'),
    'workflow' => 'public',
  );

  return $environments;
}

/**
 * Alter the environments as defined.
 *
 * This is especially useful to modify the defined allowed operations.
 *
 * @param array $environments
 *   Defined environment states.
 */
function hook_environment_alter(&$environments) {
  $environments['production'] = t('Production site');
}

/**
 * Define qualities about a given environment workflow.
 *
 * Environment workflows might also be thought of as environment namespaces.
 * A given site might have a number of different environment contexts. The
 * default workflow is NULL, and represents a straightforward site deployment
 * workflow.
 *
 * In the example for hook_environmnet, a pair of states are created for a
 * 'public' workflow which is intended to be used to indicate whether the site
 * is actually live, as opposed to in a state for internal testing.
 *
 * Other workflows that may be useful could include the current state of
 * functional development vs. front-end design, or administrative review stages
 * of the site as a software project.
 *
 * @return array
 *   Array of workflows indexed on machine name. Supported elements include:
 *   - label: The human-readable name for the workflow.
 *   - description: Extended description of the workflow.
 */
function hook_environment_workflow() {
  $workflows = array();

  $workflows['public'] = array(
    'label' => t('Publicly accessible'),
  );
  $workflows['design'] = array(
    'label' => t('Design status'),
    'description' => t('Set the current status of design/front-end work for the site.'),
  );
  $workflows['review'] = array(
    'label' => t('Administrative review'),
  );

  return $workflows;
}

/**
 * Alter the workflows as defined.
 *
 * @param array $workflows
 *   Array of defined workflows.
 */
function hook_environment_workflow_alter(&$workflows) {
  $workflows['public']['label'] = t('Publicly visible status');
}

/**
 * @} End of "addtogroup hooks".
 */
