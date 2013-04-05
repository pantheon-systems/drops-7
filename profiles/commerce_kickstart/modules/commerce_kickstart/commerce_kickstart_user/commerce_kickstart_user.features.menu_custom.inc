<?php
/**
 * @file
 * commerce_kickstart_user.features.menu_custom.inc
 */

/**
 * Implements hook_menu_default_menu_custom().
 */
function commerce_kickstart_user_menu_default_menu_custom() {
  $menus = array();

  // Exported menu: menu-user-menu.
  $menus['menu-user-menu'] = array(
    'menu_name' => 'menu-user-menu',
    'title' => 'User menu',
    'description' => '',
  );
  // Translatables
  // Included for use with string extractors like potx.
  t('User menu');


  return $menus;
}
