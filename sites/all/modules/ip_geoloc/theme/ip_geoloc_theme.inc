<?php

/**
 * @file
 * ip_geoloc_theme.inc
 *
 * Registers themes and defines template preprocess functions for these themes.
 */

/**
 * Implements hook_theme().
 *
 * Register the theme hooks available in this module, with their arguments.
 * Does not apply to Views-specific themes and template files.
 *
 * Themes may be invoked explicitly like so:
 *
 *  theme(array('<theme>'), $template_variables); // eg. <theme>==ip_geoloc_map
 *
 * where $template_variables is an array indexed by template variable names:
 *
 *  $template_variables = array(
 *    'locations' => $locations,
 *    'div_id' => '...'
 *  );
 *
 * The $template_variables are passed first to the corresponding
 * template_preprocess_<theme> function where more variables may be added.
 * Then it's on to the <theme>.tpl.php file for rendering!
 */
function ip_geoloc_theme($existing_themes, $type, $theme, $path) {
  $theme = array();
  $theme['ip_geoloc_visitor_map'] = array(
    // Located at theme/ip_geoloc_map_multi_visitor.tpl.php.
    'template' => 'ip_geoloc_map_multi_visitor',
    'path' => $path . '/theme',
    'variables' => array(
      'locations' => NULL,
      'div_id' => NULL,
      'map_options' => NULL,
      'map_style' => NULL,
    ),
  );
  $theme['ip_geoloc_map_current_visitor'] = array(
    // Located at theme/ip_geoloc_map_current_visitor.tpl.php.
    'template' => 'ip_geoloc_map_current_visitor',
    'path' => $path . '/theme',
    'variables' => array(
      'div_id' => NULL,
      'map_options' => NULL,
      'map_style' => NULL,
    ),
  );

  // For the differentiator color associations table in the Views UI
  // @see views/ip_geoloc_plugin_style.inc
  $theme['ip_geoloc_plugin_style_differentiator_color_table'] = array(
    'render element' => 'form',
  );

  return $theme;
}

/**
 * Template preprocess function for the ip_geoloc_visitor_map theme.
 *
 * Preprocesses variables for theme/ip_geoloc_map_multi_visitor.tpl.php
 */
function template_preprocess_ip_geoloc_visitor_map(&$variables) {
}

/**
 * Template preprocess function for the ip_geoloc_map_current_visitor theme.
 *
 * Preprocesses variables for theme/ip_geoloc_map_current_visitor.tpl.php
 */
function template_preprocess_ip_geoloc_map_current_visitor(&$variables) {
}
