<?php
/**
 * @file
 * Theme setting callbacks for the nuboot theme.
 */

/**
 * Implements hook_form_system_theme_settings_alter().
 */
function nuboot_form_system_theme_settings_alter(&$form, &$form_state) {
  // Hero fieldset.
  $form['hero'] = array(
    '#type' => 'fieldset',
    '#title' => t('Hero'),
    '#group' => 'general',
  );
  // Default path for image.
  $hero_path = theme_get_setting('hero_path');
  if (file_uri_scheme($hero_path) == 'public') {
    $hero_path = file_uri_target($hero_path);
  }
  // Helpful text showing the file name, non-editable.
  $form['hero']['hero_path'] = array(
    '#type' => 'textfield',
    '#title' => 'Path to front page hero region background image',
    '#default_value' => $hero_path,
    //'#disabled' => TRUE,
  );
  // Upload field.
  $form['hero']['hero_upload'] = array(
    '#type' => 'file',
    '#title' => 'Upload a photo for the hero region background image',
    '#description' => 'Upload a new image for the hero region background.',
    '#upload_validators' => array(
      'file_validate_extensions' => array('png jpg jpeg'),
    ),
  );
  // Attach custom submit handler to the form.
  $form['#submit'][] = 'nuboot_settings_submit';
}

/**
 * Implements hook_setings_submit().
 */
function nuboot_settings_submit($form, &$form_state) {
  $settings = array();
  // If the user entered a path relative to the system files directory for
  // for the hero unit, store a public:// URI so the theme system can handle it.
  if (!empty($values['hero_path'])) {
    $values['hero_path'] = _system_theme_settings_validate_path($values['hero_path']);
  }
  // Get the previous value.
  $previous = 'public://' . $form['hero']['hero_path']['#default_value'];
  if ($file = file_save_upload('hero_upload')) {
    $parts = pathinfo($file->filename);
    $destination = 'public://' . $parts['basename'];
    $file->status = FILE_STATUS_PERMANENT;
    if (file_copy($file, $destination, FILE_EXISTS_REPLACE)) {
      $_POST['hero_path'] = $form_state['values']['hero_path'] = $destination;
      // If new file has a different name than the old one, delete the old.
      if ($destination != $previous) {
        drupal_unlink($previous);
      }
    }
  }
  else {
    // Avoid error when the form is submitted without specifying a new image.
    $_POST['hero_path'] = $form_state['values']['hero_path'] = $previous;
  }
}
