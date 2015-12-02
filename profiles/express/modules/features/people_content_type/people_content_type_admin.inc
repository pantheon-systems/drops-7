<?php

/**
 * @file
 * People admin.
 */

/**
 * Implements hook_form().
 */
function people_content_type_admin_settings_form($form, &$form_state) {
  $form = array();
  $labels = variable_get('people_content_type_label_settings');
  
  $form['people_content_type_settings']['labels']['type_label'] = array(
    '#type' => 'textfield',
    '#title' => 'Type Label',
    '#default_value' => $labels['person_type'] ? $labels['person_type'] : 'type',
  );
    
  $form['people_content_type_settings']['labels']['filter_one_label'] = array(
    '#type' => 'textfield',
    '#title' => 'Filter One Label',
    '#default_value' => $labels['person_filter_one'] ? $labels['person_filter_one'] : ' ',
  );
  $form['people_content_type_settings']['labels']['filter_two_label'] = array(
    '#type' => 'textfield',
    '#title' => 'Filter Two Label',
    '#default_value' => $labels['person_filter_two'] ? $labels['person_filter_two'] : ' ',
  );
  $form['people_content_type_settings']['labels']['filter_three_label'] = array(
    '#type' => 'textfield',
    '#title' => 'Filter Three Label',
    '#default_value' => $labels['person_filter_three'] ? $labels['person_filter_three'] : ' ',
  );

  $form['#submit'][] = 'people_content_type_settings_submit';

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  return $form;
}

/**
 * TODO: Write function doco.
 */
function people_content_type_settings_submit($form_id, &$form_state) {
  $labels['person_type'] = $form_state['values']['type_label'];
  $labels['person_filter_one'] = $form_state['values']['filter_one_label'];
  $labels['person_filter_two'] = $form_state['values']['filter_two_label'];
  $labels['person_filter_three'] = $form_state['values']['filter_three_label'];  
  variable_set('people_content_type_label_settings', $labels);
  drupal_set_message('Settings have been saved.');
}