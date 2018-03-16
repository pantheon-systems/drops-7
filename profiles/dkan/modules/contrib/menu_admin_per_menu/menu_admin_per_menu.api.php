<?php

/**
 * @file
 * Hooks that can be implemented by other modules to extend Menu Admin per Menu.
 */

/**
 * Alter the menus for which a user has per menu admin permissions.
 *
 * @param $perm_menus
 *   The $perm_menus array returned by _menu_admin_per_menu_get_perm_menus()
 *   for a user account.
 * @param $account
 *   The user account object.
 *
 * @see _menu_admin_per_menu_get_perm_menus()
 */
function hook_menu_admin_per_menu_perm_menus_alter(&$perm_menus, $account) {
  // Change $perm_menus.
}
