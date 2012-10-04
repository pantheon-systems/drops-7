<?php
/**
 * @file
 * openpublish_roles.features.user_role.inc
 */

/**
 * Implements hook_user_default_roles().
 */
function openpublish_roles_user_default_roles() {
  $roles = array();

  // Exported role: author.
  $roles['author'] = array(
    'name' => 'author',
    'weight' => '3',
  );

  // Exported role: editor.
  $roles['editor'] = array(
    'name' => 'editor',
    'weight' => '4',
  );

  return $roles;
}
