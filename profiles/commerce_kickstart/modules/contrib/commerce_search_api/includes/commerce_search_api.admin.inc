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
  if ($index = search_api_index_load('product_display')) {
    $form['#entity'] = $index;
    $form['description'] = array(
      '#type' => 'item',
      '#title' =>  t('Commerce Search API'),
      '#markup' => t('This page allows you to change the behavior of the generated search index.'),
    );
    // Try to find taxonomy terms on
    foreach ($index->getFields(FALSE) as $field_name => $field) {
      if (!isset($field['entity_type']) || $field['entity_type'] != 'taxonomy_term' || strpos($field_name, ':') !== FALSE) {
        continue;
      }
      $form['commerce_search_api_' . $field_name . '_fulltext'] = array(
        '#title' => t('Allow users to search products by @field name', array('@field' => $field['name'])),
        '#type' => 'checkbox',
        '#default_value' => variable_get('commerce_search_api_' . $field_name . '_fulltext', 1),
      );
    }
    $form = system_settings_form($form);
    $form['#submit'][] = 'commerce_search_api_admin_settings_form_submit';
    return $form;
  }
}

/**
 * Submit callback for the commerce_search_api_admin_settings_form.
 */
function commerce_search_api_admin_settings_form_submit($form, &$form_state) {
  entity_defaults_rebuild(array('search_api_index', 'search_api_server'));
  drupal_set_message(t('You need to reindex your items because we added/remove fields, Click on "Index now".'));
  $form_state['redirect'] = 'admin/config/search/search_api/index/product_display/status';
}
