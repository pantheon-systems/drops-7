<?php
/**
 * @file
 * commerce_kickstart_block.features.menu_custom.inc
 */

/**
 * Implements hook_menu_default_menu_custom().
 */
function commerce_kickstart_block_menu_default_menu_custom() {
  $menus = array();

  // Exported menu: menu-footer-navigation.
  $menus['menu-footer-navigation'] = array(
    'menu_name' => 'menu-footer-navigation',
    'title' => 'Footer navigation',
    'description' => '',
  );
  // Exported menu: menu-payment-methods.
  $menus['menu-payment-methods'] = array(
    'menu_name' => 'menu-payment-methods',
    'title' => 'Payment methods',
    'description' => '',
  );
  // Exported menu: secondary-navigation.
  $menus['secondary-navigation'] = array(
    'menu_name' => 'secondary-navigation',
    'title' => 'Secondary navigation',
    'description' => '',
  );
  // Translatables
  // Included for use with string extractors like potx.
  t('Footer navigation');
  t('Payment methods');
  t('Secondary navigation');


  return $menus;
}
