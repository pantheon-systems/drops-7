<?php

/**
 * @file
 * Settings for Environment Read Only module
 */

/**
 * Admin settings form.
 *
 * @return array
 *   Form.
 */
function environment_readonly_admin_settings() {
  $form = array();

  // Build the role selection section.
  $roles = user_roles();
  $role_default_value = _environment_readonly_get_configured_roles();
  $form['roles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Configure Roles'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['roles']['environment_readonly_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Configure Read Only Roles'),
    '#description' => t('Choose the roles that should not have the permissions you select below when in a Read Only environment state.'),
    '#options' => $roles,
    '#default_value' => $role_default_value,
  );

  // Build the permission selection section.
  $perms = _environment_readonly_get_perms();
  $perms_default_value = _environment_readonly_get_action_perms();
  $form['perms'] = array(
    '#type' => 'fieldset',
    '#title' => t('Configure Permissions'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['perms']['environment_readonly_permissions'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Configure Read Only Permissions'),
    '#description' => t('Choose all the permissions that should be revoked when in a Read Only environment state.'),
    '#options' => $perms,
    '#default_value' => $perms_default_value,
  );

  return system_settings_form($form);
}
