<?php

/**
 * @file
 * Common pages for the Media Bulk Upload module.
 */

/**
 * Menu callback; Edit multiple files on the same page using multiform module.
 *
 * @todo When http://drupal.org/node/1227706 is fixed, filter the $files
 * array using file_access($file, 'edit').
 *
 * @see media_bulk_upload_file_operation_edit_multiple()
 */
function media_bulk_upload_file_page_edit_multiple($files) {
  if (empty($files)) {
    return MENU_ACCESS_DENIED;
  }

  $forms = array();
  foreach ($files as $file) {
    // To maintain unique form_ids, append the file id.
    $forms[] = array('media_edit_' . $file->fid, $file);
  }

  $form = call_user_func_array('multiform_get_form', $forms);
  $form['#attributes']['class'][] = 'media-bulk-upload-multiedit-form';

  // Add the title to each 'subform'.
  foreach (element_children($form['multiform']) as $key) {
    $fid = $form['multiform'][$key]['fid']['#value'];
    $file = $files[$fid];
    $title = t('<em>Edit @type</em> @title', array('@type' => $file->type, '@title' => $file->filename));
    $form['multiform'][$key]['#prefix'] = '<h2>' . $title . '</h2>';
    $form['multiform'][$key]['actions']['#access'] = FALSE;
  }

  if (isset($form['buttons']['Delete'])) {
    $form['buttons']['Delete']['#access'] = FALSE;
  }

  // Add a cancel button at the bottom of the form.
  $form['buttons']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#weight' => 50,
  );
  if (isset($_GET['destination'])) {
    $form['buttons']['cancel']['#href'] = $_GET['destination'];
  }
  else if (user_access('administer files')) {
    $form['buttons']['cancel']['#href'] = 'admin/content/file';
  }
  else {
    $form['buttons']['cancel']['#href'] = '<front>';
  }

  // Override the page title since each file form sets a title.
  drupal_set_title(t('Edit multiple files'));

  // Allow other modules to alter the form.
  drupal_alter('media_bulk_upload_edit_multiple_form', $form);

  return $form;
}
