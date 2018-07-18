<?php

/**
 * @file
 * Contains hooks provided by the user_external_invite module.
 */

/**
 * Exclude roles used in invite process.
 *
 * @return array $roles
 *   List of roles to exclude.
 *     ['rid'] - Role ID of excluded role
 */
function hook_user_external_invite_excluded_roles($roles) {
  $roles[] = 5;
  return $roles;
}

/**
 * Perform actions or checks before an invite is granted.
 *
 * In this hook, you can perform any checks you would want to prevent the role
 * from being granted. The only reason to return anything is if you want the
 * invite process to be halted. Any message you return will be displayed to the
 * user along with any other messages from subscribers to this hook.
 *
 * @param object $account
 *   The user account that will be granted a role.
 * @param int $grant_rid
 *   The role id that the user account will be granted.
 * @return string
 *   The error message shown to the user for why no role was granted.
 *
 */
function hook_user_external_invite_pre_grant_invite($account, $grant_rid) {
  // If email is in a list of emails we don't want to grant roles to, prevent granting.

  // This is a fictional function you would have to implement in your module.
  $blocked_emails = your_module_check_blocked_emails();

  if (in_array($account->mail, $blocked_emails)) {
    return 'The email associated with this account cannot be granted user roles.';
  }
}
