<?php

/**
 * @file
 * Configuration pages for Fivestar module.
 */

/**
 * Callback function for admin/settings/fivestar. Display the settings form.
 */
function fivestar_settings($form, $form_state) {

  $form['tags'] = array(
    '#tree' => FALSE,
    '#type' => 'fieldset',
    '#title' => t('Voting tags'),
    '#description' => t('Choose the voting tags that will be available for node rating. A tag is simply a category of vote. If you only need to rate one thing per node, leave this as the default "vote".'),
    '#weight' => 3,
   );

  $form['tags']['tags'] = array(
    '#type' => 'textfield',
    '#title' => t('Tags'),
    '#default_value' => variable_get('fivestar_tags', 'vote'),
    '#required' => TRUE,
    '#description' => t('Separate multiple tags with commas.'),
   );

  $form['#submit'][] ='fivestar_settings_submit';

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
    '#weight' => 45,
  );

  return $form;
}

function fivestar_settings_submit($form, &$form_state) {
  drupal_set_message(t('Your settings have been saved.'));
  // TODO We could delete all variables for removed tags
  variable_set('fivestar_tags', $form_state['values']['tags']);
}
