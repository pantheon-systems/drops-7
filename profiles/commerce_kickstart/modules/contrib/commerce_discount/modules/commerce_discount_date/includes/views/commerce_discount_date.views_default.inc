<?php

/**
 * @file
 * Views defaults for Commerce discount date module.
 */

/**
 * Implements hook_views_default_views_alter().
 *
 * Add start and end date to the discount overview.
 */
function commerce_discount_date_views_default_views_alter(&$views) {
  $date_fields = _commerce_discount_date_views_get_view_delta();

  $view = &$views['commerce_discount_overview'];
  $handler = &$view->display['default']->handler;

  // In order to insert the date fields in the middle of the fields array,
  // rebuild it while inserting the date fields after the 'rendered_entity'
  // field.
  if (empty($handler->display->display_options['fields']) || !is_array($handler->display->display_options['fields'])) {
    return;
  }
  foreach ($handler->display->display_options['fields'] as $field_id => $field) {
    $fields[$field_id] = $field;

    if ($field_id == 'rendered_entity') {
      $fields += $date_fields;
    }
  }

  // Overwrite the original fields array.
  $handler->display->display_options['fields'] = $fields;
}

/**
 * Helper function to get the Views import.
 */
function _commerce_discount_date_views_get_view_delta() {
  $handler = new stdClass();

  /* Field: Commerce Discount: Discount dates */
  $handler->display->display_options['fields']['commerce_discount_date']['id'] = 'commerce_discount_date';
  $handler->display->display_options['fields']['commerce_discount_date']['table'] = 'field_data_commerce_discount_date';
  $handler->display->display_options['fields']['commerce_discount_date']['field'] = 'commerce_discount_date';
  $handler->display->display_options['fields']['commerce_discount_date']['label'] = 'Start date';
  $handler->display->display_options['fields']['commerce_discount_date']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date']['alter']['external'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['commerce_discount_date']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['commerce_discount_date']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date']['alter']['html'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['commerce_discount_date']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['commerce_discount_date']['hide_empty'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date']['empty_zero'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['commerce_discount_date']['settings'] = array(
    'format_type' => 'short',
    'fromto' => 'value',
    'multiple_number' => '',
    'multiple_from' => '',
    'multiple_to' => '',
  );
  $handler->display->display_options['fields']['commerce_discount_date']['field_api_classes'] = 0;
  /* Field: Commerce Discount: Discount dates */
  $handler->display->display_options['fields']['commerce_discount_date_1']['id'] = 'commerce_discount_date_1';
  $handler->display->display_options['fields']['commerce_discount_date_1']['table'] = 'field_data_commerce_discount_date';
  $handler->display->display_options['fields']['commerce_discount_date_1']['field'] = 'commerce_discount_date';
  $handler->display->display_options['fields']['commerce_discount_date_1']['label'] = 'End date';
  $handler->display->display_options['fields']['commerce_discount_date_1']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date_1']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date_1']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date_1']['alter']['external'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date_1']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date_1']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date_1']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date_1']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['commerce_discount_date_1']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['commerce_discount_date_1']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date_1']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date_1']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date_1']['alter']['html'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date_1']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['commerce_discount_date_1']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['commerce_discount_date_1']['hide_empty'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date_1']['empty_zero'] = 0;
  $handler->display->display_options['fields']['commerce_discount_date_1']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['commerce_discount_date_1']['settings'] = array(
    'format_type' => 'short',
    'fromto' => 'value2',
    'multiple_number' => '',
    'multiple_from' => '',
    'multiple_to' => '',
  );
  $handler->display->display_options['fields']['commerce_discount_date_1']['field_api_classes'] = 0;

  return $handler->display->display_options['fields'];
}
