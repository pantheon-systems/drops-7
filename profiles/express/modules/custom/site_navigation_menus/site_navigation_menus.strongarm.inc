<?php
/**
 * @file
 * site_navigation_menus.strongarm.inc
 */

/**
 * Implements hook_strongarm().
 */
function site_navigation_menus_strongarm() {
  $export = array();

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'menu_block_menu_order';
  $strongarm->value = array(
    'main-menu' => '',
    'menu-secondary-menu' => '',
    'menu-footer-menu' => '',
    'user-menu' => '',
  );
  $export['menu_block_menu_order'] = $strongarm;

  return $export;
}
