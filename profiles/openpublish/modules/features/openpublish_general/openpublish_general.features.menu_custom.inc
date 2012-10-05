<?php
/**
 * @file
 * openpublish_general.features.menu_custom.inc
 */

/**
 * Implements hook_menu_default_menu_custom().
 */
function openpublish_general_menu_default_menu_custom() {
  $menus = array();

  // Exported menu: menu-footer-links.
  $menus['menu-footer-links'] = array(
    'menu_name' => 'menu-footer-links',
    'title' => 'Footer Links',
    'description' => 'Links to display in the footer.',
  );
  // Exported menu: menu-utility-nav.
  $menus['menu-utility-nav'] = array(
    'menu_name' => 'menu-utility-nav',
    'title' => 'Utility Nav',
    'description' => 'The Utility Navigation, used for things like \'Your Profile\', Login etc.',
  );
  // Translatables
  // Included for use with string extractors like potx.
  t('Footer Links');
  t('Links to display in the footer.');
  t('The Utility Navigation, used for things like \'Your Profile\', Login etc.');
  t('Utility Nav');


  return $menus;
}
