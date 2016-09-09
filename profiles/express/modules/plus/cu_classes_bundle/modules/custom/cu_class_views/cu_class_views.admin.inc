<?php

function cu_class_search_settings($form, $form_state) {
  $form = array();
  $options = cu_class_import_get_terms();
  $form['cu_class_search_term'] = array(
    '#type' => 'checkboxes',
    '#options' => $options,
    '#title' => 'Term',
    '#description' => 'Choose which terms are available in the search.',
    '#default_value' => variable_get('cu_class_search_term', array()),
  );
  $options = array(
    'CUBLD' => 'Boulder',
    'CUSPG' => 'Colorado Springs',
    'CUDEN' => 'Denver',
  );
  $form['cu_class_search_campus'] = array(
    '#type' => 'checkboxes',
    '#options' => $options,
    '#title' => 'Campus',
    '#description' => 'Choose which campuses are available in the search.',
    '#default_value' => variable_get('cu_class_search_campus', array()),
  );
  return system_settings_form($form);
}
