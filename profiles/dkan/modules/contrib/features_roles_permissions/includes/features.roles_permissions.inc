<?php

/**
 * Implements hook_features_export().
 */
function roles_permissions_features_export($data, &$export, $module_name = '') {
  $export['dependencies']['features'] = 'features';
  $export['dependencies']['features_roles_permissions'] = 'features_roles_permissions';

  $map = features_get_default_map('roles_permissions', 'name');
  foreach ($data as $role) {
    // Role is provided by another module. Add dependency.
    if (isset($map[$role]) && $map[$role] != $module_name) {
      $export['dependencies'][$map[$role]] = $map[$role];
    }
    // Export.
    elseif (user_role_load_by_name($role)) {
      $export['features']['roles_permissions'][$role] = $role;
    }
  }

  return array();
}

/**
 * Implements hook_features_export_options().
 */
function roles_permissions_features_export_options() {
  return drupal_map_assoc(_user_features_get_roles());
}

/**
 * Implements hook_features_export_render().
 */
function roles_permissions_features_export_render($module, $data) {
  $code = array();
  $code[] = '  $roles = array();';
  $code[] = '';

  foreach ($data as $name) {
    if ($role = user_role_load_by_name($name)) {
      $role_lookup = array($role->rid => $role->name);
      $permissions = user_role_permissions($role_lookup);
      $role->permissions = array_shift($permissions);
      unset($role->rid);
      $role_identifier = features_var_export($name);
      $role_export = features_var_export($role, '  ');
      $code[] = "  // Exported role: {$name}";
      $code[] = "  \$roles[{$role_identifier}] = {$role_export};";
      $code[] = "";
    }
  }

  $code[] = '  return $roles;';
  $code = implode("\n", $code);

  return array('default_roles_permissions' => $code);
}

/**
 * Implements hook_features_revert().
 */
function roles_permissions_features_revert($module) {
  roles_permissions_features_rebuild($module);
}

/**
 * Implements hook_features_rebuild().
 */
function roles_permissions_features_rebuild($module) {
  if ($defaults = features_get_default('roles_permissions', $module)) {
    $modules = user_permission_get_modules();

    foreach ($defaults as $role) {
      $role = (object) $role;
      if ($existing = user_role_load_by_name($role->name)) {
        $role->rid = $existing->rid;
      }
      user_role_save($role);

      $role_lookup = array($role->rid => $role->name);
      $existing_permissions = user_role_permissions($role_lookup);
      $existing = array_shift($existing_permissions);
      $permissions = array_merge($existing, $role->permissions);

      foreach ($permissions as $permission => $value) {
        if (array_key_exists($permission, $modules)) {
          if (array_key_exists($permission, $role->permissions)) {
            $permissions[$permission] = TRUE;
          }
          else {
            $permissions[$permission] = FALSE;
          }
        }
        else {
          unset($permissions[$permission]);
        }
      }

      user_role_change_permissions($role->rid, $permissions);
    }
  }
}
