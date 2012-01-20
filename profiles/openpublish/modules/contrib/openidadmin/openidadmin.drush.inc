<?php
// $Id: openidadmin.drush.inc,v 1.2 2010/02/02 16:27:57 yhahn Exp $ 

/**
 * @file
 * OpenID Admin Drush commands
 */

/**
 * Implementation of hook_drush_command().
 */
function openidadmin_drush_command() {
  $items = array();
  $items['openid-add'] = array(
    'callback' => 'openidadmin_drush_add',
    'description' => "Add OpenID(s) to a user account",
  );
  return $items;
}

/**
 * Implementation of hook_drush_help().
 */
function openidadmin_drush_help($section) {
  switch ($section) {
    case 'drush:openid-add':
      return dt("Add OpenID(s) to a user account. The first argument needs to be a valid UID, additional arguments should be OpenID URLs.");
  }
}

/**
 * Drush callback to add OpenIDs
 */
function openidadmin_drush_add() {
  $commands = func_get_args();
  $uid = array_shift($commands);
  if ($account = user_load($uid)) {
    $add_list = openidadmin_normalize_list(implode("\n", $commands));
    foreach ($add_list as $claimed_id) {
      db_query("INSERT INTO {authmap}(uid, authname, module) VALUES(%d, '%s', 'openid')", $account->uid, $claimed_id);
    }
    drush_print(format_plural(count($add_list), 'One OpenID has been added.', '@count OpenIDs have been added.'));
  }
  else {
    drush_die('Invalid UID');
  }
}
