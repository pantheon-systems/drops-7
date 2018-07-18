<?php
/**
 * @file
 * Rule implementations for the LDAP Authentication module.
 */

/**
 * Implements hook_rules_event_info().
 */
function ldap_authentication_rules_event_info() {
  return array(
    'ldap_user_created' => array(
      'label' => t('User created from LDAP entry'),
      'module' => 'ldap_authentication',
      'group' => t('User'),
      'variables' => array(
        'user' => array(
          'type' => 'user',
          'label' => t('The user created.'),
        ),
        'email_template_used' => array(
          'type' => 'boolean',
          'label' => t('Whether or not the email template was used to create the user.'),
        ),
      ),
    ),
  );
}
