<?php

/**
 * @file
 * Themeable functions for Google Custom Search Engine.
 */

function google_cse_get_cse_tag() {
  $display = variable_get('google_cse_custom_results_display', 'results-only');
  $attributes = filter_xss(variable_get('google_cse_tag_attributes', 'gname="google_cse"'));

  switch ($display) {
    case 'overlay':
    case 'compact':
    case 'full-width':
      return '<gcse:search ' . $attributes . '></gcse:search>';

    case 'two-page':
      return '<gcse:searchbox-only ' . $attributes . '></gcse:searchbox-only><gcse:searchresults-only ' . $attributes . '></gcse:searchresults-only>';


    case 'two-column':
      return '<gcse:searchbox ' . $attributes . '></gcse:searchbox><gcse:searchresults ' . $attributes . '></gcse:searchresults>';


    case 'results-only':
      return '<gcse:searchresults-only ' . $attributes . '></gcse:searchresults-only>';

    case 'google-hosted':
      return '<gcse:searchbox-only ' . $attributes . '></gcse:searchbox-only>';

    default:
      watchdog('google_cse', 'Invalid custom result display %display',
               array('%display' => $display), WATCHDOG_CRITICAL);
  }
}

/**
 * The search results page can be themed/customized.
 */
function template_preprocess_google_cse_results(&$variables) {
  $variables['results_searchbox_form'] = $variables['form'] ? drupal_get_form('google_cse_results_searchbox_form') : '';
  $variables['cse_tag'] = google_cse_get_cse_tag();
  $variables['noscript'] = t('You must enable JavaScript to view the search results.');
  $variables['prefix'] = filter_xss_admin(variable_get('google_cse_results_prefix', ''));
  $variables['suffix'] = filter_xss_admin(variable_get('google_cse_results_suffix', ''));

  if (google_cse_validate_request()) {
    drupal_add_js(drupal_get_path('module', 'google_cse') . '/google_cse_results.js',
                  array('scope' => 'footer'));
    drupal_add_css('' . variable_get('google_cse_custom_css', '') . '', 'external');
  }
}

/**
 * Validate GET parameters to avoid displaying inappropriate search results.
 */
function google_cse_validate_request() {
  return (empty($_GET['cx']) || $_GET['cx'] == variable_get('google_cse_cx', ''));
}

/**
 * Form builder for the searchbox forms.
 */
function google_cse_results_searchbox_form($form, &$form_state) {
  $form = array();
  if (variable_get('google_cse_results_display', 'here') == 'here') {
    $cof = variable_get('google_cse_cof_here', 'FORID:11');
  }
  else {
    $form['#action'] = '//' . variable_get('google_cse_domain', 'www.google.com') . '/cse';
    $cof = variable_get('google_cse_cof_google', 'FORID:0');
  }
  $form['#method'] = 'get';
  $form['cx'] = array(
    '#type' => 'hidden',
    '#value' => variable_get('google_cse_cx', ''),
  );
  $form['cof'] = array(
    '#type' => 'hidden',
    '#value' => $cof,
  );
  $form['query'] = array(
    '#type' => 'textfield',
    '#default_value' => isset($_GET['query']) ? $_GET['query'] : '',
  );
  $form['sa'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
  );
  $form['query']['#size'] = intval(variable_get('google_cse_results_searchbox_width', 40));
  $form['query']['#title'] = t('Enter your keywords');

  return $form;
}
