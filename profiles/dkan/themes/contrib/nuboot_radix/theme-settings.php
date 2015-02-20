<?php
/**
 * @file
 * Theme settings.
 */

/**
 * Implements theme_settings().
 */
function nuboot_radix_form_system_theme_settings_alter(&$form, &$form_state) {
  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('theme', 'default') . '/theme-settings.php';

  // Add theme settings here.
  $form['nuboot_radix_theme_settings'] = array(
    '#title' => t('Theme Settings'),
    '#type' => 'fieldset',
  );

  // Copyright.
  $copyright = theme_get_setting('copyright');
  $form['nuboot_radix_theme_settings']['copyright'] = array(
    '#title' => t('Copyright'),
    '#type' => 'text_format',
    '#format' => 'html',
    '#default_value' => isset($copyright['value']) ? $copyright['value'] : t('Powered by <a href="http://nucivic.com/dkan">DKAN</a>, a project of <a href="http://nucivic.com">NuCivic</a>'),
  );

  // Hero fieldset.
  $form['hero'] = array(
    '#type' => 'fieldset',
    '#title' => t('Hero Unit'),
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
    '#title' => 'Path to front page background image',
    '#default_value' => $hero_path,
    '#disabled' => TRUE,
  );
  // Upload field.
  $form['hero']['hero_upload'] = array(
    '#type' => 'file',
    '#title' => 'Upload a new photo for the hero unit',
    '#description' => t('<p>The hero unit is the large featured area located on the front page. 
      This theme supplies a default background image for this area. You may upload a different 
      photo here and it will replace the default background image.</p><p>Max. file size: 2 MB
      <br>Recommended pixel size: 1920 x 400<br>Allowed extensions: .png .jpg .jpeg</p>'),
    '#upload_validators' => array(
      'file_validate_extensions' => array('png jpg jpeg'),
    ),
  );
  // Attach custom submit handler to the form.
  $form['#submit'][] = 'nuboot_radix_settings_submit';

  // Return the additional form widgets.
  return $form;
}

/**
 * Implements hook_setings_submit().
 */
function nuboot_radix_settings_submit($form, &$form_state) {
  $settings = array();
  // If the user entered a path relative to the system files directory for
  // for the hero unit, store a public:// URI so the theme system can handle it.
  if (!empty($values['hero_path'])) {
    $values['hero_path'] = _system_theme_settings_validate_path($values['hero_path']);
  }
  // Get the previous value.
  $previous = $form['hero']['hero_path']['#default_value'];
  if ($previous !== 'profiles/dkan/themes/contrib/nuboot_radix/assets/images/hero.jpg') {
    $previous = 'public://' . $previous;
  }
  else {
    $previous = FALSE;
  }
  if ($file = file_save_upload('hero_upload')) {
    $parts = pathinfo($file->filename);
    $destination = 'public://' . $parts['basename'];
    $file->status = FILE_STATUS_PERMANENT;
    if (file_copy($file, $destination, FILE_EXISTS_REPLACE)) {
      $_POST['hero_path'] = $form_state['values']['hero_path'] = $destination;
      // If new file has a different name than the old one, delete the old.
      if ($previous && $destination != $previous) {
        drupal_unlink($previous);
      }
    }
  }
  else {
    // Avoid error when the form is submitted without specifying a new image.
    $_POST['hero_path'] = $form_state['values']['hero_path'] = $previous;
  }
}
