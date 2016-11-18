<?php

/**
 * @file
 * Admin page callbacks for the commerce_search_api module.
 */

/**
 * Form constructor for the Commerce Search API form.
 *
 * @see commerce_search_api_admin_settings_form_submit()
 *
 * @ingroup forms
 */
function commerce_search_api_admin_settings_form($form, &$form_state) {
  $form['commerce_search_api_provide_default_index'] = array(
    '#title' => t('Provide a default Search Index'),
    '#description' => t('If checked, a default Search API Product display index will be generated with sensible defaults configuration.'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('commerce_search_api_provide_default_index', TRUE),
  );
  $form['commerce_search_api_generate_facets'] = array(
    '#title' => t('Generate default facets'),
    '#description' => t('If checked, defaults Facet API facets will be generated for the Product display index.'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('commerce_search_api_generate_facets', TRUE),
    '#states' => array(
      'visible' => array(
        ':input[name="commerce_search_api_provide_default_index"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form = system_settings_form($form);
  $form['#submit'][] = 'commerce_search_api_admin_settings_form_submit';
  return $form;
}

/**
 * Form submission handler for commerce_search_api_admin_settings_form().
 *
 * @see commerce_search_api_admin_settings_form().
 */
function commerce_search_api_admin_settings_form_submit($form, &$form_state) {
  entity_defaults_rebuild(array('search_api_index', 'search_api_server'));
}
