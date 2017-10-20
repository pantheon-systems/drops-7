<?php

function cu_search_settings_form($form, &$form_state) {
  $form['description'] = array(
    '#type' => 'markup',
    '#markup' => 'Select the options for the site search box.',
  );

  $form['cu_search_options'] = array(
    '#type' => 'checkboxes',
    '#options' => array(
      'all' => 'Colorado.edu',
    ),
    '#default_value' => variable_get('cu_search_options', array('all')),
  );
  $site_search = variable_get('google_cse_cx', NULL);
  if ($site_search) {
    $form['cu_search_options']['#options']['this'] = t('this site');
    $form['this_site_label'] = array(
      '#title' => 'Custom label for site search.',
      '#type' => 'textfield',
      '#default_value' => variable_get('this_site_label', 'this site'),
    );
  }
  if ($searches = variable_get('cu_search_affiliation_search_options', NULL)) {
    $searches = $searches = explode("\n", $searches);
    foreach ($searches as $search) {
      if (!empty($search)) {
        $search_values = explode('|', $search);
        $label = $search_values[0];
        $form['cu_search_options']['#options'][$label] = $label;
      }
    }
  }

  $form['#validation'][] = 'cu_search_settings_form_validate';
  $form = system_settings_form($form);
  return $form;
}

/**
 * Implements hook_form_validate().
 */
function cu_search_settings_form_validate($form, &$form_state) {
  // only do validation stuff here (and only here)

  $options = count($form_state['values']['cu_search_options']);
  $all = $form_state['values']['cu_search_options']['all'];
  if ($options > 1) {
    $thisone = $form_state['values']['cu_search_options']['this'];

    // Check to see if you are searching 'this' site.
    if ($form_state['values']['this_site_label'] == '' && $thisone === 'this') {
      form_set_error('this_site_label', 'You must enter a value for the custom search label.');
    }

    // Next we want to make sure at least one item is checked.
    if ($all === 0 && $thisone === 0) {
      form_set_error('cu_search_options', 'You must select at least one search option');
    }
  } else {
    // If there is no CSE ID, then we're only checking to make sure search colorado.edu goes.
    if ($all === 0) {
      form_set_error('cu_search_options', 'You must select at least one search option');
    }
  }
}

function cu_search_settings_admin_form($form, &$form_state) {
  $form['google_cse_cx'] = array(
    '#title' => 'Google CSE ID',
    '#type' => 'textfield',
    '#default_value' => variable_get('google_cse_cx', ''),
  );
  $form['cu_search_additional_cse'] = array(
    '#type' => 'textarea',
    '#title' => 'Additional Google CSEs',
    '#description' => 'Add CSE IDs for additional searches. Enter as Name|CSE ID',
    '#default_value' => variable_get('cu_search_additional_cse', ''),
    '#access' => FALSE,
  );
  $form['affiliation_search_config'] = array(
    '#type' => 'fieldset',
    '#title' => 'Affiliation Search Configurations',
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
  );
  global $base_url;
  $site_name = trim(variable_get('site_name', ''));
  $form['affiliation_search_config'][$site_name] = array(
    '#type' => 'markup',
    '#markup' => '<div class="additional-cse-config">' .  $site_name . '|' . $base_url . '</div>',
  );
  if ($searches = variable_get('cu_search_additional_cse', NULL)) {

    $searches = explode("\n", $searches);

    foreach ($searches as $key => $search) {
      $search_config = explode('|', $search);
      $form_key = $search_config[0];
      $form['affiliation_search_config'][$form_key] = array(
        '#type' => 'markup',
        '#markup' => '<div class="additional-cse-config">' .  $form_key . '|' . $base_url . '|' . $key . '</div>',
      );
    }
  }

  $form['cu_search_affiliation_search_options'] = array(
    '#type' => 'textarea',
    '#title' => 'Affiliation Search Options',
    '#description' => 'Copy a configuration from the affiliated site.',
    '#default_value' => variable_get('cu_search_affiliation_search_options', ''),
  );
  $form = system_settings_form($form);
  return $form;
}
