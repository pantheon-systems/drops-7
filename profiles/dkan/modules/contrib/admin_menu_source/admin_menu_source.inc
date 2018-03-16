<?php
/**
 * @file
 * Menu callbacks for admin_menu_source module
 */

function admin_menu_source_settings() {
  $roles = user_roles(TRUE, 'access administration menu');
  $menus = menu_get_menus();
  $default_values = _admin_menu_source_get_settings();

  $form['#theme'] = 'admin_menu_source_settings_form';

  $form['admin_menu_source_description'] = array(
    '#markup' => t('Configure the source for the administration menu.'),
  );

  $form['admin_menu_source_settings'] = array(
    '#tree' => TRUE,
    '#title' => t('Menu source per role'),
    '#sorted' => FALSE,
  );

  foreach ($default_values as $rid => $setting) {
    if (empty($roles[$rid])) {
      continue;
    }
    $form['admin_menu_source_settings'][$rid]['role'] = array(
      '#markup' => check_plain($roles[$rid]),
    );
    $form['admin_menu_source_settings'][$rid]['rid'] = array(
      '#type' => 'hidden',
      '#value' => $rid,
    );
    $form['admin_menu_source_settings'][$rid]['source'] = array(
      '#type' => 'select',
      '#options' => array('' => t('Default')) + $menus,
      '#default_value' => isset($setting['source']) ? $setting['source'] : '',
    );
    $form['admin_menu_source_settings'][$rid]['weight'] = array(
      '#type' => 'weight',
      '#title' => t('Weight'),
      '#default_value' => isset($setting['weight']) ? $setting['weight'] : 0,
      '#title_display' => 'invisible',
    );
    $form['admin_menu_source_settings'][$rid]['#weight'] =  isset($setting['weight']) ? $setting['weight'] : 0;
  }

  $form = system_settings_form($form);

  // Add a custom submit handler.
  $form['#submit'][] = 'admin_menu_source_settings_submit';

  return $form;
}

function admin_menu_source_settings_submit() {
  $settings = _admin_menu_source_get_settings();

  // Sort the settings based on their weight.
  uasort($settings, '_admin_menu_source_sort_by_roles');

  // normalize weights
  $weight = 0;
  foreach ($settings as $key => $setting) {
    $settings[$key]['weight'] = $weight;
    $weight++;
  }

  // re-save
  variable_set('admin_menu_source_settings', $settings);

  // Flush admin_menu's cache.
  admin_menu_flush_caches();
}

/**
 * Implementation of a uasort compatible sorter function.
 * Helper function to sort roles based on their weight
 */
function _admin_menu_source_sort_by_roles($a, $b) {
  if ($a['weight'] == $b['weight']) {
    return 0;
  }
  return ($a['weight'] < $b['weight']) ? -1 : 1;
}
