<?php
/**
 * @file
 * cu_hero_unit.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function cu_hero_unit_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'hero_unit';
  $bean_type->label = 'Hero Unit';
  $bean_type->options = '';
  $bean_type->description = 'A lightweight, flexible component to showcase key content on your site';
  $export['hero_unit'] = $bean_type;

  return $export;
}
