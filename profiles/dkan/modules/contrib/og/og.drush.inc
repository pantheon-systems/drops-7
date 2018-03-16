<?php

/**
 * Implements hook_drush_command().
 */
function og_drush_command() {
  $items = array();

  $items['og-add-entity'] = array(
    'description' => dt("Add one or more entity instances to a group."),
    // Describe the arguments for this command. Delete
    // this seciton if command takes no arguments.
    'arguments' => array(
      'group-type' => 'The entity type of the group.',
      'group' => 'The group entity or ID.',
      'entity-type' => 'The entity type to add to the group.',
      'entities' => 'A comma-separated list (no whitespace) of entity IDs to add to the group. If not specified, all IDs matching the entity type (and bundle, if set) will be added.',
    ),

    // List all options used for this command. Only options that
    // are listed here (or that are global Drush options) may
    // be specified on the commnadline; all others raise an error.
    'options' => array(
      'bundle' => array(
        'description' => dt('Restrict to-be-added entity instances to a specific bundle, such as "page" for nodes.'),
        'example-value' => 'bundle-ID',
      ),
      'dry-run' => array(
        'description' => dt("Don't actually add entities to the group. Instead, show what would be done."),
      ),
    ),

    // Give one or more example commandline usages for this command.
    'examples' => array(
      'drush og-add-entity node 25 node 41,42,43' => 'Add nodes with IDs 41, 42 and 43 to node group ID 25.',
      'drush og-add-entity --bundle=page node 25 node 41,42,43' => 'Add nodes with IDs 41, 42 and 43 to node group ID 25. Non-page nodes will not be added.',
      'drush og-add-entity --dry-run node 25 node 41' => 'Show attempt to add node 41 to node group ID 25 without actually doing it.',
    ),
  );

  $items['og-add-user'] = array(
    'description' => dt("Add one or more users to a group."),
    // Describe the arguments for this command. Delete
    // this seciton if command takes no arguments.
    'arguments' => array(
      'group-type' => 'The entity type of the group.',
      'group' => 'The group entity or ID.',
      'role' => 'The group role ID to which users should be added.',
      'users' => 'A comma-separated list (no whitespace) of user IDs to add to the group. If not specified, all users will be added.',
    ),

    // List all options used for this command. Only options that
    // are listed here (or that are global Drush options) may
    // be specified on the commnadline; all others raise an error.
    'options' => array(
      'dry-run' => array(
        'description' => dt("Don't actually add users to the group. Instead, show what would be done."),
      ),
      'core-role' => array(
        'description' => dt("Only add users belonging to the specified Drupal core system role, such as '2' for 'authenticated user'."),
        'example-value' => 'core-role-ID',
      ),
    ),

    // Give one or more example commandline usages for this command.
    'examples' => array(
      'drush og-add-user node 25 2 5,6,7' => 'Give users with IDs 5, 6 and 7 the "member" role within node group ID 25.',
      'drush og-add-user node 30 3 2,3,4' => 'Give users with IDs 2, 3 and 4 the "administrator member" role within node group ID 30.',
      'drush og-add-user --dry-run node 30 3 5' => 'Show what would happen if giving user 5 the "administrator member" role within node group 30 without actually doing it.',
      'drush og-add-user --core-role=2 node 30 3 5' => 'Give user 5 the "member" role within node group 30 if he or she is in core role 2, "authenticated user" on most systems.',
    ),
  );

  // Commandfiles may also add topics. These will appear in
  // the list of topics when `drush topic` is executed.
  $topic_file = dirname(__FILE__) . '/og-topic.txt';
  if (file_exists($topic_file)) {
    $items['docs-og'] = array(
      'description' => dt('Description of command topic.'),
      'hidden' => TRUE,
      'topic' => TRUE,
      'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
      'callback' => 'drush_print_file',
      'callback arguments' => array($topic_file),
    );
  }

  return $items;
}

/**
 * Implements hook_drush_help().
 *
 * @param $section
 *   A string with the help section (prepend with 'drush:')
 *
 * @return
 *   A string with the help text for your command.
 */
function og_drush_help($section) {
  switch ($section) {
    case 'drush:og':
      return dt("Brief help for Drush command og.");
    // The 'title' meta item is used to name a group of
    // commands in `drush help`. If a title is not defined,
    // the default is "All commands in ___", with the
    // specific name of the commandfile (e.g. og).
    // Command files with less than four commands will
    // be placed in the "Other commands" section, _unless_
    // they define a title. It is therefore preferable
    // to not define a title unless the file defines a lot
    // of commands.
    case 'meta:og:title':
      return dt("Organic Groups commands");
    // The 'summary' meta item is displayed in `drush help --filter`,
    // and is used to give a general idea what the commands in this
    // command file do, and what they have in common.
    case 'meta:og:summary':
      return dt("Summary of all commands in this command group.");
  }
}

/**
 * Implementation of drush_hook_COMMAND().
 */
function drush_og_add_entity() {

  // Fetch the argument list.
  $arguments = func_get_args();

  // Assign each argument to a corresponding variable.
  $group_type = $arguments[0];
  $group_id = $arguments[1];
  $entity_type = $arguments[2];
  if (isset($arguments[3])) {
    $entity_ids = _convert_csv_to_array($arguments[3]);
  }
  else {
    $entity_ids = FALSE;
  }

  // Load the list of entities to add to the group.
  $entities = entity_load($entity_type, $entity_ids);

  // Get information on the group.
  $group = entity_load_single($group_type, $group_id);
  $group_wrapper = entity_metadata_wrapper($group_type, $group);

  // Get the bundle if it was specified.
  $bundle = drush_get_option('bundle');

  // Iterate through each one.
  $entity_count = 0;
  foreach ($entities as $entity) {

    // Get some information on the entity instance.
    $entity_wrapper = entity_metadata_wrapper($entity_type, $entity);

    // Don't add groups to the group or things could get messy.
    if (($group_type == $entity_type) && ($group_wrapper->getBundle() == $entity_wrapper->getBundle())) {
      continue;
    }

    // Narrow the list to the bundle, if specified.
    if (isset($bundle) && ($bundle != $entity_wrapper->getBundle())) {
      continue;
    }

    // Set its values.
    $entity_values = array(
      'entity_type' => $entity_type,
      'entity' => $entity,
      'state' => OG_STATE_ACTIVE,
    );

    // Add the entity to the group if this isn't a dry run.
    $dry_run = drush_get_option('dry-run');
    if (empty($dry_run)) {
      og_group($group_type, $group_id, $entity_values);
    }

    // Report results.
    $message = dt("@entity_type @entity_name has been added to group @group_type @group_name.", array(
      '@entity_type' => $entity_type,
      '@entity_name' => $entity_wrapper->label(),
      '@group_type' => $group_type,
      '@group_name' => $group_wrapper->label(),
    ));
    $entity_count++;
    drush_print($message);
    $result .= $message;
  }

  // Report the total number added.
  $message = dt("Entities added: @entity_count", array('@entity_count' => $entity_count));
  drush_print($message);
  $result .= $message;

  // Return the result to make it available to code that calls
  // this command using drush_invoke_process.
  return $result;
}

/**
 * Implementation of drush_hook_COMMAND_validate().
 */
function drush_og_add_entity_validate() {

  // Get all of the arguments.
  $arguments = func_get_args();

  // Get a list of valid entity types.
  $entity_types = entity_get_info();

  // Check for a valid group type.
  if (!isset($entity_types[$arguments[0]])) {
    return drush_set_error('DRUSH_OG_GROUP_TYPE_INVALID_ERROR', dt('The group entity type is invalid.'));
  }

  // Check for a group ID.
  $group = entity_load($arguments[0], array($arguments[1]));
  if (empty($group)) {
    return drush_set_error('DRUSH_OG_GROUP_ID_INVALID_ERROR', dt('The group ID is invalid.'));
  }

  // Check for a valid entity type to add.
  if (!isset($entity_types[$arguments[2]])) {
    return drush_set_error('DRUSH_OG_ENTITY_TYPE_INVALID_ERROR', dt('The entity type to add is invalid.'));
  }

  // Check for valid entity instances to add.
  if (isset($arguments[3])) {
    $entity_ids = _convert_csv_to_array($arguments[3]);
    foreach ($entity_ids as $entity_id) {
      $entity = entity_load($arguments[2], array($entity_id));
      if (empty($entity)) {
        return drush_set_error(
          'DRUSH_OG_ENTITY_ID_INVALID_ERROR',
          dt('The entity ID to add, @id, is invalid.', array('@id' => $entity_id))
        );
      }
    }
  }

  // If we made it this far, the command validates.
  return TRUE;
}

/**
 * Implementation of drush_hook_COMMAND().
 */
function drush_og_add_user() {

  // Fetch the argument list.
  $arguments = func_get_args();

  // Assign each argument to a corresponding variable.
  $group_type = $arguments[0];
  $group_id   = $arguments[1];
  $role_id    = $arguments[2];
  if (isset($arguments[3])) {
    $user_ids = _convert_csv_to_array($arguments[3]);
  }
  else {
    $user_ids = FALSE;
  }

  // Get the core-role option if it's set.
  $role_core_id = drush_get_option('core-role');
  if (isset($role_core_id)) {
    $user_roles = user_roles();
    $role_core_name = isset($user_roles[$role_core_id]) ? $user_roles[$role_core_id] : '';
  }

  // Load the list of users to add to the group.
  $users = entity_load('user', $user_ids);

  // Get the name of the role.
  $role_names = og_get_user_roles_name(array($role_id));

  // Get information on the group.
  $group = entity_load_single($group_type, $group_id);
  $group_wrapper = entity_metadata_wrapper($group_type, $group);

  // Iterate through each one.
  $user_count = 0;
  foreach ($users as $user) {

    // Skip users that aren't in the specified system role if specified.
    if (isset($role_core_id) && (!in_array($role_core_name, $user->roles))) {
      continue;
    }

    // If this isn't a dry run, add the user to the group with the role.
    $dry_run = drush_get_option('dry-run');
    if (empty($dry_run)) {

      // Add the user to the group.
      og_group($group_type, $group_id, array('entity' => $user));

      // Assign the group role to the user.
      og_role_grant($group_type, $group_id, $user->uid, $role_id);
    }

    // Report results.
    $message = dt(
      "User @user_name has been added to group @group_type @group_name with role '@role_name'.",
      array(
        '@user_name' => $user->name,
        '@group_type' => $group_type,
        '@group_name' => $group_wrapper->label(),
        '@role_name' => $role_names[$role_id],
      )
    );
    $user_count++;
    drush_print($message);
    $result .= $message;
  }

  // Report the total number added.
  $message = dt("Users added: @user_count", array('@user_count' => $user_count));
  drush_print($message);
  $result .= $message;

  // Return the result to make it available to code that calls
  // this command using drush_invoke_process.
  return $result;
}

/**
 * Implementation of drush_hook_COMMAND_validate().
 */
function drush_og_add_user_validate() {

  // Get all of the arguments.
  $arguments = func_get_args();

  // Get a list of valid entity types.
  $entity_types = entity_get_info();

  // Check for a valid group type.
  if (!isset($entity_types[$arguments[0]])) {
    return drush_set_error('DRUSH_OG_GROUP_TYPE_INVALID_ERROR', dt('The group entity type is invalid.'));
  }

  // Check for a group ID.
  $group = entity_load($arguments[0], array($arguments[1]));
  if (empty($group)) {
    return drush_set_error('DRUSH_OG_GROUP_ID_INVALID_ERROR', dt('The group ID is invalid.'));
  }

  // Make sure that the role is valid.
  $role_names = og_get_user_roles_name(array($arguments[2]));
  if ((!isset($role_names)) || (!isset($role_names[$arguments[2]]))) {
    return drush_set_error('DRUSH_OG_ROLE_ID_INVALID_ERROR', dt('The role ID is invalid.'));
  }

  // Check for valid entity instances to add.
  if (isset($arguments[3])) {
    $user_ids = _convert_csv_to_array($arguments[3]);
    foreach ($user_ids as $user_id) {
      $user = entity_load('user', array($user_id));
      if (empty($user)) {
        return drush_set_error(
          'DRUSH_OG_USER_ID_INVALID_ERROR',
          dt('The user ID to add, @id, is invalid.', array('@id' => $user_id))
        );
      }
    }
  }

  // If we made it this far, the command validates.
  return TRUE;
}

