<?php
/**
 * @file
 * cu_block_theme.strongarm.inc
 */

/**
 * Implements hook_strongarm().
 */
function cu_block_theme_strongarm() {
  $export = array();

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'blocktheme_show_custom_block_theme';
  $strongarm->value = 0;
  $export['blocktheme_show_custom_block_theme'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'blocktheme_themes';
  $strongarm->value = 'blockgray|Gray
blockdarkgray|Dark Gray
tan|Tan
lightblue|Light Blue
blockoutline|Outline
blockunderline|Underline';
  $export['blocktheme_themes'] = $strongarm;

  return $export;
}
