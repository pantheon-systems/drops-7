<?php
/**
 * @file
 * cu_hero_unit.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function cu_hero_unit_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "bean_admin_ui" && $api == "bean") {
    return array("version" => "5");
  }
  if ($module == "field_group" && $api == "field_group") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_image_default_styles().
 */
function cu_hero_unit_image_default_styles() {
  $styles = array();

  // Exported image style: hero.
  $styles['hero'] = array(
    'label' => 'Hero',
    'effects' => array(
      2 => array(
        'name' => 'image_scale',
        'data' => array(
          'width' => 1500,
          'height' => '',
          'upscale' => 1,
        ),
        'weight' => 1,
      ),
    ),
  );

  return $styles;
}
