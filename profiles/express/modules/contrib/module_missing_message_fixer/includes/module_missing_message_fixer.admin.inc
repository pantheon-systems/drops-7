<?php

/**
 * @file
 * The Missing Module Message Fixer Admin Settings file.
 */

include_once 'module_missing_message_fixer.helpers.inc';

/**
 * Missing Module Message Fixer Form.
 */
function module_missing_message_fixer_form() {
  $form = array();

  // Set the header.
  $header = array(
    'name' => 'Name',
    'type' => 'Type',
    'filename' => 'Filename',
  );

  $rows = _module_missing_message_fixer_check_modules();

  // Fancy title string.
  $title = t('This list comes from the system table and is checked against the drupal_get_filename() function. See <a href="@link" target="_blank">this issue</a> for more information.', array(
    '@link' => 'https://www.drupal.org/node/2487215',
  ));

  // Title.
  $form['title'] = array(
    '#type' => 'item',
    '#markup' => '<h2><center>' . $title . '</h2></center>',
  );

  // Fancy submit buttons to win this.
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Remove These Errors!'),
    '#submit' => array('module_missing_message_fixer_form_submit'),
    '#prefix' => '<center>',
    '#suffix' => '</center>',
  );

  // Set the tables select to make this more granular.
  $form['table'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#empty' => t('No Missing Modules Found!!!'),
  );

  return $form;
}

/**
 * Submit handler for Missing Module Message Fixer Form.
 */
function module_missing_message_fixer_form_submit($form, &$form_state) {
  $module = array();
  // Go through each record and add it to the array to win.
  foreach ($form_state['values']['table'] as $module) {
    $modules[] = $module;
  }

  // Delete if there is no modules.
  if (count($modules) > 0) {
    db_delete('system')
      ->condition('name', $modules, 'IN')
      ->execute();
  }
}
