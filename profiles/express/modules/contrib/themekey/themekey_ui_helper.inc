<?php

/**
 * @file
 * helper functions for ThemeKey UI feature to add a theme option to the
 * 'URL aliases' administration if the "Path" module is enabled.
 *
 * @see themekey_ui.module
 * @see themekey_ui_admin.inc
 *
 * @author Markus Kalkbrenner | Cocomore AG
 *   @see http://drupal.org/user/124705
 *
 * @author profix898
 *   @see http://drupal.org/user/35192
 */

require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'themekey') . '/themekey_build.inc';

/**
 * Returns a theme assigned to a Drupal path alias using
 * a ThemeKey Theme Switching Rule which is not part of a chain
 *
 * @param $path
 *   Drupal path as string
 *
 * @return
 *   array with two elements:
 *   - id of the database entry in table themekey_properties
 *     or '0' if no entry exists
 *   - the theme
 */
function themekey_ui_get_path_theme($path) {
  $serialized_empty_array = serialize(array());
  if ($result = db_select('themekey_properties', 'tp')
      ->fields('tp', array('id', 'theme'))
      ->condition('property', 'drupal:path')
      ->condition('operator', '=')
      ->condition('value', $path)
      ->condition('wildcards', $serialized_empty_array)
      ->condition('parent', 0)
      ->execute()
  ) {
  
    foreach ($result as $row) {
      $query = db_select('themekey_properties', 'tp')
      ->condition('parent', $row->id);
      $query->addExpression('COUNT(*)');
      if (!$query->execute()->fetchField()) {
        return array($row->id, $row->theme);
      }
    }
    
  }

  return array(0, 'default');
}


/**
 * Saves a theme assigned to a path alias as
 * ThemeKey rule
 *
 * @see themekey_rule_set()
 *
 * @param $path
 *   Drupal path alias as string
 *
 * @param $theme
 *   assigned Drupal theme as string
 *
 * @param $id
 *   the id of an existing rule if this
 *   one should be modified
 */
function themekey_ui_set_path_theme($path, $theme = 'default', $id = 0) {
  $item = array(
    'property' => 'drupal:path',
    'value' => $path,
    'theme' => $theme,
    'enabled' => 1,
  );

  if ($id > 0) {
    $item['id'] = $id;
  }

  themekey_rule_set($item);
}


/**
 * Deletes a ThemeKey rule.
 *
 * @see themekey_rule_del()
 *
 * @param $id
 *   the id of an existing rule
 */
function themekey_ui_del_path_theme($id) {
  return themekey_rule_del($id);
}
