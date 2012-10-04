<?php
/**
 * @file
 * openpublish_general.strongarm.inc
 */

/**
 * Implements hook_strongarm().
 */
function openpublish_general_strongarm() {
  $export = array();

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'context_ui_dialog_enabled';
  $strongarm->value = 1;
  $export['context_ui_dialog_enabled'] = $strongarm;

  return $export;
}
