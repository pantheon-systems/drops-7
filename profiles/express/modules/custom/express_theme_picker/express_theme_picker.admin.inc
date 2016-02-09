<?php

/**
 * Page callback for admin/config/development/module-manager/settings
 */
function express_theme_picker_admin_settings() {
  $form = array();

  $themes = list_themes();
  $active_theme = variable_get('theme_default', NULL);
  $options = array();
  foreach ($themes as $key => $theme) {
    $name = ($theme->status) ? $theme->info['name'] . ' (enabled)' : $theme->info['name'];
    $name = ($key == $active_theme) ? $theme->info['name'] . ' (active)' : $name;
    $options[$key] = $name;
  }
  $form['express_theme_picker_theme_ignore'] = array(
    '#title' => t('Hidden Themes'),
    '#type' => 'checkboxes',
    '#description' => t('You can select which themes you want to be hidden from the end user on the bundles list page. This can be useful for soft launches or other use cases where you don\'t want certain users to be able to enable the theme.'),
    '#default_value' => variable_get('express_theme_picker_theme_ignore', NULL),
    '#options' => $options,
    $active_theme => array('#disabled' => TRUE),
  );

  return system_settings_form($form);

}
