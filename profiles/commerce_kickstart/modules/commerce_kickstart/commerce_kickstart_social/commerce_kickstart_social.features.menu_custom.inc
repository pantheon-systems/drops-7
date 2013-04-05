<?php
/**
 * @file
 * commerce_kickstart_social.features.menu_custom.inc
 */

/**
 * Implements hook_menu_default_menu_custom().
 */
function commerce_kickstart_social_menu_default_menu_custom() {
  $menus = array();

  // Exported menu: menu-social-connection.
  $menus['menu-social-connection'] = array(
    'menu_name' => 'menu-social-connection',
    'title' => 'Connect with us',
    'description' => 'The "Connect with us" menu.',
  );
  // Translatables
  // Included for use with string extractors like potx.
  t('Connect with us');
  t('The "Connect with us" menu.');


  return $menus;
}
