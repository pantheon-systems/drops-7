<?php

/**
 * @file
 * Integration with Rules module.
 */

/**
 * Implements hook_rules_event_info().
 * @ingroup rules
 */
function simplesamlphp_auth_rules_event_info() {
  $defaults = array(
    'group' => t('User'),
    'module' => 'simplesamlphp_auth',
  );

  return array(
    'simplesamlphp_auth_rules_event_register' => $defaults + array(
      'label' => t('After registering a new user account via simpleSAMLphp'),
      'group' => t('User'),
      'variables' => array(
        'user' => array('type' => 'user', 'label' => t('Logged in user')),
      ),
    ),
    'simplesamlphp_auth_rules_event_login' => $defaults + array(
      'label' => t('User has logged in via simpleSAMLphp'),
      'group' => t('User'),
      'variables' => array(
        'user' => array('type' => 'user', 'label' => t('User')),
      ),
    ),
  );
}
