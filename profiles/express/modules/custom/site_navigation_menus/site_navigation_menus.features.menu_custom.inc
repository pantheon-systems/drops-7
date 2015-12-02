<?php
/**
 * @file
 * site_navigation_menus.features.menu_custom.inc
 */

/**
 * Implements hook_menu_default_menu_custom().
 */
function site_navigation_menus_menu_default_menu_custom() {
  $menus = array();

  // Exported menu: menu-footer-menu.
  $menus['menu-footer-menu'] = array(
    'menu_name' => 'menu-footer-menu',
    'title' => 'Footer Menu',
    'description' => 'A simple menu for the footer',
  );
  // Exported menu: menu-mobile-menu.
  $menus['menu-mobile-menu'] = array(
    'menu_name' => 'menu-mobile-menu',
    'title' => 'Mobile Menu',
    'description' => 'A menu for mobile navigation',
  );
  // Exported menu: menu-secondary-menu.
  $menus['menu-secondary-menu'] = array(
    'menu_name' => 'menu-secondary-menu',
    'title' => 'Secondary Menu',
    'description' => 'A secondary menu, often used as audience-based navigation',
  );
  // Translatables
  // Included for use with string extractors like potx.
  t('A menu for mobile navigation');
  t('A secondary menu, often used as audience-based navigation');
  t('A simple menu for the footer');
  t('Footer Menu');
  t('Mobile Menu');
  t('Secondary Menu');


  return $menus;
}
