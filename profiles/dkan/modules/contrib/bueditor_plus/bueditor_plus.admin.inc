<?php

/**
 * @file
 * Provides the administration interface for bueditor_plus.
 */

/**
 * Main administration page. Overrides the default BUEditor admin page.
 * We actually grab the bueditor_admin build array and replace the form
 * with our own to represent the new profiles system. The format has
 * been mimicked from the original bueditor_admin() form for the purposes
 * of usability.
 */
function bueditor_plus_admin() {

  // We are going to load the original page from bueditor_admin and then
  // unset the form in the build array.
  module_load_include('inc', 'bueditor', 'admin/bueditor.admin');
  $output = bueditor_admin();
  unset($output['form']);

  // Add in our profiles table.
  $path = 'admin/config/content/bueditor/profile';
  $profiles = bueditor_plus_profiles();
  $header = array(t('Profile name'), array('data' => t('Operations'), 'colspan' => 2));
  $rows = array();

  // We don't need our special "global" profile to appear in the table,
  // so we go ahead and unset it.
  unset($profiles['global']);

  foreach ($profiles as $pid => $profile) {
    $profile->name = check_plain($profile->name);
    $name = $profile->global ? $profile->name . ' <em>(' . t('Global') . ')</em>' : $profile->name;
    $rows[] = array(
      $name,
      l(t('Edit'), $path . '/edit/' . $pid) . ' | ' .
      l(t('Delete'), $path . '/delete/' . $pid),
    );
  }

  // Even though we add a local action link, we keep the Add new Profile
  // link in the table so that it copies the same usability as the bueditor
  // editor's table from bueditor_admin().
  $rows[] = array(
    '',
    l(t('Add new profile'), $path . '/new'),
  );

  $output['profile_title'] = array(
    '#markup' => '<h2 class="title">' . t('Available profiles') . '</h2>',
  );

  $output['profile_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('id' => 'available-profiles-list'),
  );

  return $output;
}

/**
 * Menu Callback; Delivers the confirmation to delete $profile
 *
 * @param array $form
 *   Drupal form array.
 * @param array $form_state
 *   An associative array containing the current state of the form.
 * @param object $profile
 *   A BUEditor Plus profile object.
 *
 * @return array
 *   Returns a confirm form
 */
function bueditor_plus_profile_delete_form($form, &$form_state, $profile) {
  $form['#profile'] = $profile;
  return confirm_form($form, t('Delete profile %profile', array('%profile' => $profile->name)), 'admin/config/content/bueditor');
}

/**
 * Handles the submission of our profile delete form.
 *
 * @see bueditor_plus_profile_delete_form()
 */
function bueditor_plus_profile_delete_form_submit($form, &$form_state) {
  $profile = $form['#profile'];
  db_delete('bueditor_plus_profiles')
    ->condition('pid', $profile->pid)
    ->execute();
  drupal_set_message(t('The profile %profile has been deleted', array('%profile' => $profile->name)));
  $form_state['redirect'] = 'admin/config/content/bueditor';
}

/**
 * Delivers the profile form. If $profile is specified then that profile will
 * be edited. If not then we are creating a new profile.
 *
 * @param array $form
 *   Drupal form array.
 * @param array $form_state
 *   An associative array containing the current state of the form.
 * @param object $profile
 *   A BUEditor Plus profile object.
 *
 * @return array
 *   Returns Drupal form array.
 */
function bueditor_plus_profile_form($form, &$form_state, $profile = NULL) {
  $formats = filter_formats();
  module_load_include('inc', 'bueditor', 'admin/bueditor.admin');
  $eds = bueditor_editors('all');
  $editors = array(0 => 'None');
  foreach ($eds as $eid => $editor) {
    $editors[$eid] = $editor->name;
  }

  if ($profile) {
    $profile->is_new = FALSE;
  }
  else {
    // Create our default profile.
    $profile = new stdClass();
    $profile->pid = 0;
    $profile->is_new = TRUE;
    $profile->name = '';
    $profile->global = '';
    $profile->data = array();
    foreach ($formats as $format) {
      $profile->data[$format->format]['default'] = 0;
      $profile->data[$format->format]['alternative'] = 0;
    }
  }
  $form['#profile'] = $profile;

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Profile Name'),
    '#description' => t('Give your profile a unique name to easily identify it.'),
    '#required' => TRUE,
    '#default_value' => $profile->name,
  );

  $form['formats'] = array(
    '#tree' => TRUE,
    '#theme' => 'bueditor_plus_profile_table',
  );

  foreach ($formats as $format) {
    $form['formats'][$format->format]['name'] = array('#markup' => check_plain($format->name));
    $form['formats'][$format->format]['default'] = array(
      '#type' => 'select',
      '#options' => $editors,
      '#default_value' => $profile->data[$format->format]['default'],
    );
    $form['formats'][$format->format]['alternative'] = array(
      '#type' => 'select',
      '#options' => $editors,
      '#default_value' => $profile->data[$format->format]['alternative'],
    );
  }

  $form['global'] = array(
    '#type' => 'checkbox',
    '#title' => t('Global profile'),
    '#description' => t("A global profile will be used on fields that don't have a specific profile assigned. Only one global profile can exist per installation"),
    '#default_value' => $profile->global,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => $profile->is_new ? t('Create Profile') : t('Save Profile'),
  );

  return $form;
}

/**
 * Handles the submission of our profile form.
 *
 * @see bueditor_plus_profile_delete_form()
 */
function bueditor_plus_profile_form_submit(&$form, &$form_state) {
  $fields = array(
    'name' => $form_state['values']['name'],
    'data' => serialize($form_state['values']['formats']),
    'global' => $form_state['values']['global'],
  );

  if ($form_state['values']['global']) {
    // If this profile is set to be a global profile then we unset any other
    // profiles that are set as global. There can be only one global profile.
    db_update('bueditor_plus_profiles')
    ->fields(array('global' => 0))
    ->condition('global', 1)
    ->execute();
  }

  // Insert/update our profile data.
  db_merge('bueditor_plus_profiles')
    ->fields($fields)
    ->condition('pid', $form['#profile']->pid)
    ->execute();

  if ($form['#profile']->pid) {
    drupal_set_message(t('Profile has been updated'));
  }
  else {
    drupal_set_message(t('New profile created'));
  }

  $form_state['redirect'] = 'admin/config/content/bueditor';
}

/**
 * Returns HTML for our profile table.
 *
 * @param array $vars
 *   An associative array containing:
 *   - form: An array containing the form object of the table.
 *
 * @ingroup themeable
 */
function theme_bueditor_plus_profile_table(&$vars) {
  $form = $vars['form'];
  foreach (element_children($form) as $key) {
    $rows[] = array(
      drupal_render($form[$key]['name']),
      drupal_render($form[$key]['default']),
      drupal_render($form[$key]['alternative']),
    );
  }

  return theme('table', array('header' => array('Format', 'Default Editor', 'Alternative Editor'), 'rows' => $rows));
}
