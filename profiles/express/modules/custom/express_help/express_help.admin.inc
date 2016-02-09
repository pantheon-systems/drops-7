<?php

/**
 * Page callback for admin/config/development/module-manager
 */
function express_help_admin_settings() {
  $form = array();

  $form['express_help_find'] = array(
    '#type' => 'textfield',
    '#size' => 100,
    '#default_value' => variable_get('express_help_find'),
    '#title' => t('Find this pattern'),
    '#description' => t('This allow you to rewrite urls to point a dev or test instance at the corresponding help if it exists'),
  );


  $form['express_help_replace'] = array(
    '#type' => 'textfield',
    '#size' => 100,
    '#default_value' => variable_get('express_help_replace'),
    '#title' => t('Replace with'),
    '#description' => t('If you found http://www, you could replace it with https://www-dev'),
  );

  return system_settings_form($form);

}
