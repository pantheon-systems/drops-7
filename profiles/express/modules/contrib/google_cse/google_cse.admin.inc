<?php

/**
 * @file
 * Admin settings for Google Custom Search Engine.
 */

/**
 * Admin settings page for the CSE.
 */
function google_cse_admin_settings() {
  $form = array();

  $form['google_cse_cx'] = array(
    '#title' => t('Google Custom Search Engine ID'),
    '#type' => 'textfield',
    '#default_value' => variable_get('google_cse_cx', ''),
    '#description' => t('Enter your <a target="_blank" href="http://www.google.com/cse/manage/all">Google CSE unique ID</a> (click on control panel).'),
  );

  $form['google_cse_results_tab'] = array(
    '#title' => t('Search results tab name'),
    '#type' => 'textfield',
    '#maxlength' => 50,
    '#size' => 60,
    '#description' => t('Enter a custom name of the tab where search results are displayed (defaults to %google).', array('%google' => t('Google'))),
    '#default_value' => variable_get('google_cse_results_tab', ''),
  );

  $form['google_cse_results_searchbox_width'] = array(
    '#title' => t('Google CSE block searchbox width'),
    '#type' => 'textfield',
    '#maxlength' => 4,
    '#size' => 6,
    '#description' => t('Enter the desired width, in characters, of the searchbox on the Google CSE block.'),
    '#default_value' => variable_get('google_cse_results_searchbox_width', 40),
  );

  $form['google_cse_results_width'] = array(
    '#title' => t('Search results frame width'),
    '#type' => 'textfield',
    '#maxlength' => 4,
    '#size' => 6,
    '#description' => t('Enter the desired width, in pixels, of the search frame.'),
    '#default_value' => variable_get('google_cse_results_width', 600),
  );

  $form['google_cse_cof_here'] = array(
    '#title' => t('Ad format on this site'),
    '#type' => 'radios',
    '#default_value' => variable_get('google_cse_cof_here', 'FORID:11'),
    '#options' => array(
      'FORID:9' => t('Right'),
      'FORID:10' => t('Top and right'),
      'FORID:11' => t('Top and bottom'),
    ),
    '#description' => t('Ads on the right increase the width of the iframe. Non-profit organizations can disable ads in the Google CSE control panel.'),
  );

  $form['google_cse_cof_google'] = array(
    '#title' => t('Ad format on Google'),
    '#type' => 'radios',
    '#default_value' => variable_get('google_cse_cof_google', 'FORID:0'),
    '#options' => array(
      'FORID:0' => t('Right'),
      'FORID:1' => t('Top and bottom'),
    ),
    '#description' => t('AdSense ads are also displayed when the CSE links or redirects to Google.'),
  );

  $form['google_cse_results_prefix'] = array(
    '#title' => t('Search results prefix text'),
    '#type' => 'textarea',
    '#cols' => 50,
    '#rows' => 4,
    '#description' => t('Enter text to appear on the search page before the search form.'),
    '#default_value' => variable_get('google_cse_results_prefix', ''),
  );

  $form['google_cse_results_suffix'] = array(
    '#title' => t('Search results suffix text'),
    '#type' => 'textarea',
    '#cols' => 50,
    '#rows' => 4,
    '#description' => t('Enter text to appear on the search page after the search form and results.'),
    '#default_value' => variable_get('google_cse_results_suffix', ''),
  );

  $form['google_cse_results_display'] = array(
    '#title' => t('Display search results'),
    '#type' => 'radios',
    '#default_value' => variable_get('google_cse_results_display', 'here'),
    '#options' => array(
      'here' => t('On this site (requires JavaScript)'),
      'google' => t('On Google'),
    ),
    '#description' => t('Search results for the Google CSE block can be displayed on this site, using JavaScript, or on Google, which does not require JavaScript.'),
  );

  $form['google_cse_results_display_images'] = array(
    '#title' => t('Display thumbnail images in the search results'),
    '#type' => 'checkbox',
    '#description' => t('If set, search result snippets will contain a thumbnail image'),
    '#default_value' => variable_get('google_cse_results_display_images', TRUE),
  );

  $form['google_cse_show_watermark'] = array(
    '#title' => t('Show watermark in search field'),
    '#type' => 'checkbox',
    '#description' => t('If set, search result snippets will contain a thumbnail image'),
    '#default_value' => variable_get('google_cse_results_display_images', TRUE),
  );

  $form['google_cse_tag_attributes'] = array(
    '#title' => t('Google CSE Tag Attributes'),
    '#type' => 'textfield',
    '#description' => t('Enter any additional tag attributes you want in the GCSE tag. Defaults to gname="google_cse". A list of attributes can be found here: <a href="https://developers.google.com/custom-search/docs/element#supported_attributes">Google Custom Search supported attributes</a>'),
    '#default_value' => variable_get('google_cse_tag_attributes', 'gname="google_cse"'),
  );

  $form['advanced']['google_cse_custom_css'] = array(
    '#title' => t('Stylesheet Override'),
    '#type' => 'textfield',
    '#default_value' => variable_get('google_cse_custom_css', ''),
    '#description' => t('Set a custom stylesheet to override or add any styles not allowed in the CSE settings (such as "background-color: none;"). Include <span style="color:red; font-weight:bold;">!important</span> for overrides.<br/>Example: <em>//replacewithrealsite.com/sites/all/modules/google_cse/default.css</em>'),
  );

  $form['advanced']['google_cse_custom_results_display'] = array(
    '#title' => t('Layout of Search Engine'),
    '#type' => 'radios',
    '#default_value' => variable_get('google_cse_custom_results_display', 'results-only'),
    '#options' => array(
      'overlay' => t('Overlay'),
      'two-page' => t('Two page'),
      'full-width' => t('Full width'),
      'two-column' => t('Two column'),
      'compact' => t('Compact'),
      'results-only' => t('Results only'),
      'google-hosted' => t('Google hosted'),
    ),
    '#description' => t('Set the search engine layout, as found in the Layout tab of <a target="_blank" href="@url">Custom Search settings</a>.', array(
      '@url' => 'https://www.google.com/cse/lookandfeel/layout?cx=' . variable_get('google_cse_cx', ''),
    )),
  );

  if (!module_exists('search')) {
    return system_settings_form($form);
  }

  // @TODO: Put the values back into a fieldset when integrated with core search
  $form['google_cse'] = array(
    '#title' => t('Google CSE'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  return $form;

}
