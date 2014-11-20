<?php

/**
 * Build an admin form to enable/disable entity edit path overrides.
 */
function views_megarow_admin_settings_form($form, &$form_state) {
  $form['views_megarow_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Megarow title'),
    '#description' => t('Title of the megarow displayed on top of the content.'),
    '#size' => 30,
    '#default_value' => variable_get('views_megarow_title', t('Megarow content')),
  );
  $form['views_megarow_override'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#title' => t('Override settings'),
  );
  $form['views_megarow_override']['description'] = array(
    '#markup' => t('<p>Disabling these overrides will allow other modules to override the menu paths. <br /> This is useful if you want some paths to be overriden by panels for instance.</p>')
  );
  $form['views_megarow_override']['views_megarow_override_node_edit'] = array(
    '#type' => 'radios',
    '#title' => t('Override the node edit form with Views Megarow'),
    '#default_value' => variable_get('views_megarow_override_node_edit', 1),
    '#options' => array(t('Disable'), t('Enable')),
    '#description' => t('Allows Views Megarow to override the path node/%node/edit.'),
  );
  $form['views_megarow_override']['views_megarow_override_user_edit'] = array(
    '#type' => 'radios',
    '#title' => t('Override the user edit form with Views Megarow'),
    '#default_value' => variable_get('views_megarow_override_user_edit', 1),
    '#options' => array(t('Disable'), t('Enable')),
    '#description' => t('Allows Views Megarow to override the path user/%user/edit.'),
  );
  $form['#submit'][] = 'views_megarow_settings_submit';

  return system_settings_form($form);
}

/**
 * Submit callback for views_megarow_settings().
 */
function views_megarow_settings_submit() {
 // Rebuild the menu cache in order to take into account the new configuration.
  variable_set('menu_rebuild_needed', TRUE);
}
