<?php

/**
 * @file
 * Drush commands for Secure Permissions.
 */

/**
 * Implements hook_drush_command().
 */
function secure_permissions_drush_command() {
  $items['secure-permissions-rebuild'] = array(
    'callback' => 'secure_permissions_drush_rebuild',
    'description' => dt('Rebuild the site\'s roles and permissions with Secure Permissions.'),
    'arguments' => array(),
    'drupal dependencies' => array('secure_permissions'),
    'aliases' => array('spr'),
  );
  $items['secure-permissions'] = array(
    'callback' => 'secure_permissions_drush_toggle_active',
    'description' => dt('Control the settings of the Secure Permissions module from the command line.'),
    'examples' => array(
      'drush secure-permissions on' => 'Activates the Secure Permissions module from the command line, note this is distinct and different from enabling the module.',
      'drush secure-permissions off' => 'Deactivates the Secure Permissions module from the command line, note this is distinct and different from disabling the module.',
    ),
    'arguments' => array(
      'action' => 'Required. Currently has two states - set to "on" to activate Secure Permissions and "off" to deactivate.',
    ),
    'drupal dependencies' => array('secure_permissions'),
    'aliases' => array('sp'),
  );
  return $items;
}

/**
 * Implements hook_drush_help().
 */
function secure_permissions_drush_help($section) {
  switch ($section) {
    case 'drush:secure-permissions-rebuild':
      return dt('Rebuild the site\'s roles and permissions with Secure Permissions.');
    case 'drush:secure-permissions':
      return dt('Turn Secure Permissions module on/off.');
  }
}

/**
 * Callback function for drush secure-permissions-rebuild.
 *
 * Calls the default rebuild function and logs the event.
 */
function secure_permissions_drush_rebuild() {
  $active = variable_get('secure_permissions_active', 0);
  if ($active) {
    secure_permissions_rebuild();
    _secure_permissions_message_drush('Roles and permissions rebuilt.', array(), 'success');
  }
  else {
    _secure_permissions_message_drush('Could not rebuild roles and permissions because Secure Permissions is not active. Use "drush secure-permissions on" to activate the module from the command line.', array(), 'error');
  }
}

/**
 * Callback function for drush secure-permissions.
 *
 * Currently activates/deactivates the Secure Permissions module.
 *
 * @param $action
 *   (string) 'on' to activate, 'off' to deactivate.
 */
function secure_permissions_drush_toggle_active($action) {
  if ($action == 'on') {
    variable_set('secure_permissions_active', 1);
    _secure_permissions_message_drush('Secure Permissions module has been activated.', array(), 'success');
  }
  elseif ($action == 'off') {
    variable_set('secure_permissions_active', 0);
    _secure_permissions_message_drush('Secure Permissions module has been deactivated.', array(), 'success');
  }
  else {
    _secure_permissions_message_drush('Could not understand the response !active. Doing nothing.', array('!active' => $active), 'error');
  }
}

/**
 * Send a message to the drush log.
 * Taken from the Backup And Migrate module.
 */
function _secure_permissions_message_drush($message, $replace, $type) {
  // Use drush_log to display to the user.
  drush_log($message, str_replace('status', 'notice', $type));
  // Watchdog log the message as well for admins.
  watchdog('secure_permissions', $message, $replace, $type == 'error' ? WATCHDOG_ERROR : WATCHDOG_NOTICE);
}
