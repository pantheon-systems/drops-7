<?php
/**
 * @file
 * Drush command file.
 */

include_once 'module_missing_message_fixer.helpers.inc';

/**
 * Implements hook_drush_command().
 */
function module_missing_message_fixer_drush_command() {
  $items = array();

  $items['module-missing-message-fixer-list'] = array(
    'description' => dt('Returns a list of modules that have missing messages.'),
    'aliases' => array(
      'mmmfl',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'engines' => array(
      'outputformat' => array(
        'default' => 'table',
        'pipe-format' => 'json',
        'field-labels' => array(
          'name' => 'Name',
          'type' => 'Type',
          'filename' => 'Filename',
        ),
        'require-engine-capability' => array('format-table'),
      ),
    ),
  );

  $items['module-missing-message-fixer-fix'] = array(
    'description' => dt('Fixes modules that have missing messages.'),
    'aliases' => array(
      'mmmff',
    ),
    'arguments' => array(
      'name' => 'The name of the module to fix.',
    ),
    'options' => array(
      'all' => dt('Fixes all module missing messages'),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'engines' => array(
      'outputformat' => array(
        'default' => 'table',
        'pipe-format' => 'json',
        'field-labels' => array(
          'name' => 'Name',
          'type' => 'Type',
          'filename' => 'Filename',
        ),
        'require-engine-capability' => array('format-table'),
      ),
    ),
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 */
function module_missing_message_fixer_drush_help($section) {
  switch ($section) {
    case 'module-missing-message-fixer-list':
      return dt("Returns a list of modules that have missing messages.");

  }
}

/**
 * Drush command.
 *
 * Displays a list of modules that have missing messages.
 */
function drush_module_missing_message_fixer_list() {
  $rows = _module_missing_message_fixer_check_modules();

  return $rows;
}

/**
 * Drush command.
 *
 * @param string $name
 *        The name of the module to fix messages for.
 */
function drush_module_missing_message_fixer_fix($name = NULL) {
  $module = array();

  if (drush_get_option('all') != NULL) {
    $rows = _module_missing_message_fixer_check_modules();

    foreach ($rows as $row) {
      $modules[] = $row['name'];
    }
  }
  else {
    $modules[] = $name;
  }

  // Delete if there is no modules.
  if (count($modules) > 0) {
    db_delete('system')
      ->condition('name', $modules, 'IN')
      ->execute();
  }
}
