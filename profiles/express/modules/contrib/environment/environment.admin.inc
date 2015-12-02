<?php
/**
 * @file
 * Settings for Environment module.
 */

/**
 * Environment admin form.
 *
 * @return array
 *   admin form.
 */
function environment_admin_settings() {
  $form = array();

  $env_override = variable_get('environment_override', NULL);
  $env_current = environment_current(NULL);

  if ($env_override) {
    $form['environment'] = array(
      '#type'  => 'item',
      '#title' => t('Current Environment'),
      '#value' => $env_current,
    );
    $form['environment_override'] = array(
      '#type'  => 'item',
      '#title' => t('Environment Override'),
      '#value' => t('Active'),
    );
  }
  else {
    $form['environment'] = array(
      '#title' => t('Environment state'),
      '#type' => 'fieldset',
      '#tree' => TRUE,
    );
    foreach (environment_load_workflow() as $name => $workflow) {
      $default = isset($env_current[$name]) ? $env_current[$name] : '';
      $form['environment'][$name] = array(
        '#title' => t('@workflow environment workflow', array('@workflow' => $workflow['label'])),
        '#description'   => $workflow['description'],
        '#type'          => 'select',
        '#options'       => _environment_state_options($name),
        '#default_value' => $default,
      );

      if (!$default) {
        drupal_set_message(t('Not in a valid state for the @workflow workflow. Saving this form will put it into a valid state if one exists.', array('@workflow' => $workflow['label'])), 'warning');
      }
    }
  }
  $form['environment_require_override'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Require environment override'),
    '#description'   => t('Used to require that an environment is set in the settings.php file.'),
    '#default_value' => variable_get('environment_require_override', FALSE),
  );
  $form['#submit'][] = 'environment_switch_environment_submit';
  return system_settings_form($form);
}

/**
 * Submit callback to switch environment if changed.
 */
function environment_switch_environment_submit($form, &$form_state) {
  if (!$form_state['values']['environment_require_override']) {
    foreach (element_children($form['environment']) as $element) {
      if ($form['environment'][$element]['#default_value'] != $form_state['values']['environment'][$element]) {
        $result = environment_switch($form_state['values']['environment'][$element]);
        if ($result) {
          $original = environment_load($form['environment'][$element]['#default_value']);
          $new = environment_load($form_state['values']['environment'][$element]);
          $workflow = environment_load_workflow($element);
          drupal_set_message(t("Successfully switched @workflow workflow from the '@original' to the '@new' environment.",
                               array(
                                 '@workflow' => $workflow['label'],
                                 '@original' => $original['label'],
                                 '@new' => $new['label'],
                               )
                              )
                            );
        }
      }
    }
  }
}

/**
 * Environment switch form callback.
 *
 * @param string $environment
 *   The environment being switched to.
 */
function environment_switch_confirm(&$form_state, $environment) {
  if (empty($environment)) {
    drupal_set_message(t('Invalid environment "%environment". You cannot switch to an undefined environment.', array('%environment' => $environment)), 'warning');
    drupal_goto('admin/settings/environment');
  }
  return confirm_form(array('environment' => array('#type' => 'hidden', '#value' => $environment)), t('Are you sure you want to switch the current environment?'), 'admin/settings/environment',
    t('This action switches the current environment to "%env". This kind of change is as risky as updating your site. This action cannot be undone.', array('%env' => $environment)),
    t('Switch environment'), t('Cancel')
  );
}

/**
 * Handler for switch environment confirmation.
 */
function environment_switch_confirm_submit($form, &$form_state) {
  if (isset($form['environment'])) {
    foreach ($form['environment'] as $workflow => $environment) {
      if ($form_state['values']['environment'][$workflow] != $form['environment'][$workflow]) {
        environment_switch($form_state['values']['environment'][$workflow], $workflow);
      }
    }
  }
  $form_state['redirect'] = 'admin/settings/environment';
}
