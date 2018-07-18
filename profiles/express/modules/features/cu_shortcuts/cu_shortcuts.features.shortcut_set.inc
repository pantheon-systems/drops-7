<?php
/**
 * @file
 * cu_shortcuts.features.shortcut_set.inc
 */

/**
 * Implements hook_shortcut_default_shortcut_set().
 */
function cu_shortcuts_shortcut_default_shortcut_set() {
  $shortcut_sets = array();
  $shortcut_sets['shortcut-set-1'] = array(
    'set_name' => 'shortcut-set-1',
    'title' => 'Default',
  );
  return $shortcut_sets;
}
