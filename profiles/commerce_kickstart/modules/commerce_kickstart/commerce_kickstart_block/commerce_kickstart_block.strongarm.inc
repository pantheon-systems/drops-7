<?php
/**
 * @file
 * commerce_kickstart_block.strongarm.inc
 */

/**
 * Implements hook_strongarm().
 */
function commerce_kickstart_block_strongarm() {
  $export = array();

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'menu_secondary_links_source';
  $strongarm->value = 'secondary-navigation';
  $export['menu_secondary_links_source'] = $strongarm;

  return $export;
}
