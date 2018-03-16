<?php

/**
 * @file
 * Allows site administrators to further delegate the task of managing user's
 * roles.
 */

/**
 * Returns a system settings form to administer RoleAssign.
 *
 * Allows the administrator to select which roles will be available to assign
 * for users with the <code>assign roles</code> permission.
 *
 * @return array|null
 */
function roleassign_admin_form() {
  // To admister roleassign, 'administer permissions' permission is required.
  if (!user_access('administer permissions')) {
    return NULL;
  }

  // Get all available roles except for 'anonymous user'
  // and 'authenticated user'.
  $roles = user_roles(TRUE);
  unset($roles[DRUPAL_AUTHENTICATED_RID]);

  // Show checkboxes with roles that can be delegated if any.
  if ($roles) {
    $form['roleassign_roles'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Roles'),
      '#options' => $roles,
      '#default_value' => variable_get('roleassign_roles',  array()),
      '#description' => t('Select roles that should be available for assignment.'),
    );
  }
  else {
    $form['roleassign_roles'] = array(
      '#type' => 'markup',
      '#value' => '<p>No assignable roles available. You have to ' . l(t('create roles'), 'admin/people/permissions/roles') . ' that can be assigned.</p>',
    );
  }

  // Return system settings form.
  return system_settings_form($form);
}

/**
 * Really implements hook_form_alter().
 *
 * Adds checkboxes for assignable roles to the user edit form.
 *
 * @param array $form
 * @param array $form_state
 * @param string $form_id
 */
function _roleassign_form_alter(array &$form, array &$form_state, $form_id) {
  // Get all roles that are available.
  $roles = user_roles(TRUE);

  // Get roles that are available for assignment.
  $assignable_roles = _roleassign_assignable_roles($roles);

  // Get roles already assigned to the account.
  $account = NULL;
  if ($form_id == 'user_profile_form' && !empty($form_state['user'])) {
    $account = $form_state['user'];
  }
  $assigned_roles = (isset($account->roles) ? $account->roles : array());

  // An account might already have a role that isn't available for assignment
  // through this module. Such a role is called "sticky".
  // Get sticky roles.
  $sticky_roles = array_diff($assigned_roles, $assignable_roles);
  $sticky_roles = array_intersect_key($roles, $sticky_roles);

  // Store sticky roles for later use in roleassign_user_presave().
  _roleassign_sticky_roles($sticky_roles);

  // Make a string of all sticky roles.
  $sticky_roles[DRUPAL_AUTHENTICATED_RID] = $roles[DRUPAL_AUTHENTICATED_RID];
  $sticky_roles_str = implode(', ', $sticky_roles);

  // Build the assign roles checkboxes.
  $roles_field = array(
    '#type' => 'checkboxes',
    '#title' => t('Assignable roles'),
    '#options' => $assignable_roles,
    '#default_value' => array_keys($assigned_roles),
    '#description' => t('The user receives the combined permissions of all roles selected here and the following roles: %roles.', array('%roles' => $sticky_roles_str)),
  );

  // The user form is sometimes within an 'account' fieldset.
  if (isset($form['account'])) {
    $user_form =& $form['account'];
  }
  else {
    $user_form =& $form;
  }

  // Add the assign roles checkboxes to the user form, and make sure
  // that the notify user checkbox comes last.
  if (isset($user_form['notify'])) {
    $notify_field = $user_form['notify'];
    unset($user_form['notify']);
    $user_form['roleassign_roles'] = $roles_field;
    $user_form['notify'] = $notify_field;
  }
  else {
    $user_form['roleassign_roles'] = $roles_field;
  }

  if (user_access('administer permissions', $account) && !user_access('administer permissions')) {
    drupal_set_message(t('Some of the fields on this form are locked for this user.'), 'warning', FALSE);
    $form['account']['name']['#disabled'] = TRUE;
    $form['account']['mail']['#disabled'] = TRUE;
    $form['account']['pass']['#disabled'] = TRUE;
  }
}

/**
 * Really implements hook_user_presave().
 *
 * @param array $edit
 * @param object $account
 * @param string $category
 */
function _roleassign_user_presave(array &$edit, $account, $category) {
  // If this isn't the account category, or there is no roleassign_roles
  // field, there isn't much to do.
  if ($category != 'account' || !isset($edit['roleassign_roles'])) {
    return;
  }

  // If someone is trying to update user's roles, it's a malicious
  // attempt to alter user's roles.
  if (!user_access('assign roles')) {
    watchdog('security', "Detected malicious attempt to alter user's roles.", array(), WATCHDOG_WARNING);
    form_set_error('category', t("Detected malicious attempt to alter user's roles."));
  }

  // On submit, copy sticky and assigned roles from 'roleassign_roles'
  // to 'roles'.
  $edit['roles'] = array_filter(_roleassign_sticky_roles() + $edit['roleassign_roles']);
  unset($edit['roleassign_roles']);
}

/**
 * Really implements hook_user_operations().
 *
 * Add or remove roles to selected users.
 * Thanks to hunmonk for the original code.
 *
 * @return array|null
 */
function _roleassign_user_operations() {
  // Get roles that are available for assignment.
  $assignable_roles = _roleassign_assignable_roles(user_roles(TRUE));

  // Build an array of available operations.
  if (count($assignable_roles)) {
    $add_roles = $remove_roles = array();
    foreach ($assignable_roles as $key => $value) {
      $add_roles['roleassign_add_role-' . $key] = $value;
      $remove_roles['roleassign_remove_role-' . $key] = $value;
    }
    $operations = array(
      t('Add a role to the selected users') => array('label' => $add_roles),
      t('Remove a role from the selected users') => array('label' => $remove_roles),
    );
  }
  else {
    $operations = array();
  }

  // The global variable $form_values is not available anymore;
  // the $_POST values are "sanitized" below.

  // The required 'callback' key and optional 'callback arguments' key are
  // actually only needed when someone has posted. We therefore postpone
  // the attachement of these until $form_values is set.
  if (isset($_POST['operation']) && $operation = $_POST['operation']) {
    // Get operation and role id.
    $op = explode('-', $operation);
    $rid = (isset($op[1]) ? intval($op[1]) : NULL);
    $op = $op[0];

    // If not a RoleAssign operation, there is not much to do.
    if ($op != 'roleassign_add_role' && $op != 'roleassign_remove_role') {
      return NULL;
    }

    // If someone is trying to update user's roles, it's a malicious
    // attempt to alter user's roles.
    if (!user_access('assign roles')) {
      watchdog('security', 'Detected malicious attempt to alter user\'s roles.', array(), WATCHDOG_WARNING);
      form_set_error('category', t('Detected malicious attempt to alter user\'s roles.'));
    }

    // Form the name of the core callback functions for adding and
    // removing roles by choping off the 'roleassign_' part of the
    // operation string.
    $operations[$operation] = array(
      'callback'           => 'user_multiple_role_edit',
      'callback arguments' => array(substr($op, 11), $rid),
      'label'              => '(DUMMY)',
    );
  }

  return $operations;
}

/**
 * Returns assignable roles.
 *
 * @param array $roles
 *
 * @return array
 */
function _roleassign_assignable_roles(array $roles) {
  return array_intersect_key($roles, array_filter(variable_get('roleassign_roles', array())));
}

/**
 * Store and retrive sticky roles.
 *
 * @param array|null $new_sticky_roles
 *
 * @return array
 */
function _roleassign_sticky_roles($new_sticky_roles = NULL) {
  static $sticky_roles = array();
  if (isset($new_sticky_roles)) {
    $sticky_roles = $new_sticky_roles;
  }
  return $sticky_roles;
}

/**
 * Really implements hook_help().
 *
 * Returns various help texts.
 *
 * @param string $path
 * @param $arg
 *
 * @return string|null
 */
function _roleassign_help($path = "admin/help#roleassign", $arg) {
  $perms = user_permission();
  $variables['%Administer_users'] = $perms['administer users']['title'];
  $variables['%Administer_permissions'] = $perms['administer permissions']['title'];
  $perms = roleassign_permission();
  $variables['%Assign_roles'] = $perms['assign roles']['title'];
  $variables['!help'] = l(t('help page'), 'admin/help/roleassign');

  switch ($path) {
    case 'admin/people/permissions/roleassign':
      return t('Users with both %Administer_users and %Assign_roles permissions are allowed to assign the roles selected below. For more information, see the !help.', $variables);
    case 'admin/help#roleassign':
      return t('
<p>RoleAssign specifically allows site administrators to further delegate the task of managing user\'s roles.</p>
<p>RoleAssign introduces a new permission called %Assign_roles. Users with this permission are able to assign selected roles to still other users. Only users with the %Administer_permissions permission may select which roles are available for assignment through this module.</p>
<!--break-->
<h2>Background</h2>
<p>It is possible for site administrators to delegate the user administration through the %Administer_users permission. But that doesn\'t include the right to assign roles to users. That is necessary if the delegatee should be able to administrate user accounts without intervention from a site administrator.</p>
<p>To delegate the assignment of roles, site administrators have had until now no other choice than also grant the %Administer_permissions permission. But that is not advisable, since it gives right to access all roles, and worse, to grant any rights to any role. That can be abused by the delegatee, who can assign himself all rights and thereby take control over the site.</p>
<p>This module solves this dilemma by introducing the %Assign_roles permission. While editing a user\'s account information, a user with this permission will be able to select roles for the user from a set of available roles. Roles available are configured by users with the %Administer_permissions permission.</p>
<h2>Install</h2>
<ol>
  <li>Copy the entire !roleassign directory, containing the !roleassign_module and other files, to your Drupal modules directory.</li>
  <li>Log in as site administrator.</li>
  <li>Go to the administration page for modules and enable the module.</li>
</ol>
<h2>Configuration</h2>
<ol>
  <li>Log in as site administrator.</li>
  <li>Go to the administration page for access control and grant %Assign_roles permission to those roles that should be able to assign roles to other users. Notice that besides the %Assign_roles permission, these roles also must have the %Administer_users permission.</li>
  <li>Go to the administration page for role assign and select those roles that should be available for assignment by users with %Assign_roles permission.</li>
  <li>For each user that should be able to assign roles, go to the user\'s account and select a role with both the %Assign_roles and the %Administer_users permissions.</li>
</ol>
<p><strong>Beware:</strong> granting %Administer_users permission to users will allow them to modify admin passwords or email addresses or even delete the site administrator account. The !User_protect module can prevent this.</p>
<h2>Usage</h2>
<ol>
  <li>Log in as a user with both the %Assign_roles and the %Administer_users permissions.</li>
  <li>To change the roles of a user, go to the user\'s account and review the assignable roles and change them as necessary.</li>
</ol>', $variables + array(
        '!roleassign' => '<code>roleassign</code>',
        '!roleassign_module' => '<code>roleassign.module</code>',
        '!User_protect' => '<a href="http://drupal.org/project/userprotect">User protect</a>',
      ));
  }
  return NULL;
}
