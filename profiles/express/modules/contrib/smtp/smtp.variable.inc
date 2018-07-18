<?php
/**
 * @file
 * Custom integration with the Variable module.
 */

/**
 * Implements hook_variable_group_info().
 */
function smtp_variable_group_info() {
  $groups['smtp'] = array(
    'title' => t('SMTP Authentication Support'),
    'access' => 'administer smtp module',
    'description' => t('Configure SMTP server for site emails to be sent from different names.'),
  );

  return $groups;
}

/**
 * Implements hook_variable_info().
 *
 * Allows for the SMTP from name to be translated if/when the Variable module is
 * enabled.
 *
 * @link http://api.drupalhelp.net/api/variable/variable.api.php/function/hook_variable_info/7
 * @param array $options
 */
function smtp_variable_info($options) {
  $variable['smtp_fromname'] = array (
    'title' => t('Email from name (SMTP module)'),
    'type' => 'string',
    'description' => t('Allow for site emails to be sent from a different name.'),
    'group' => 'smtp',
    'multidomain' => TRUE,
    'localize' => TRUE,
  );

  return $variable;
}
