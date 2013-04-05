<?php

/**
 * @file
 * Views defaults for Commerce discount usage module.
 */

/**
 * Implements hook_views_default_views_alter().
 *
 * Add analytics field to discounts overview.
 */
function commerce_discount_usage_views_default_views_alter(&$views) {
  $usage_field = _commerce_discount_usage_views_get_view_delta();

  $view = &$views['commerce_discount_overview'];
  $handler = &$view->display['default']->handler;

  if (!is_array($handler->display->display_options['fields'])) {
    return;
  }
  foreach ($handler->display->display_options['fields'] as $field_id => $field) {
    // Add the usage column after the status column.
    if ($field_id == 'enabled') {
      $fields += $usage_field;
    }
    $fields[$field_id] = $field;
  }

  // Overwrite the original fields array.
  $handler->display->display_options['fields'] = $fields;
}

/**
 * Helper function to get the Views import.
 */
function _commerce_discount_usage_views_get_view_delta() {
  $handler = new stdClass();

  /* Field: Commerce Discount: Analytics */
  $handler->display->display_options['fields']['commerce_discount_usage']['id'] = 'commerce_discount_usage';
  $handler->display->display_options['fields']['commerce_discount_usage']['table'] = 'commerce_discount';
  $handler->display->display_options['fields']['commerce_discount_usage']['field'] = 'commerce_discount_usage';
  $handler->display->display_options['fields']['commerce_discount_usage']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['commerce_discount_usage']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['commerce_discount_usage']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['commerce_discount_usage']['alter']['external'] = 0;
  $handler->display->display_options['fields']['commerce_discount_usage']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['commerce_discount_usage']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['commerce_discount_usage']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['commerce_discount_usage']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['commerce_discount_usage']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['commerce_discount_usage']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['commerce_discount_usage']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['commerce_discount_usage']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['commerce_discount_usage']['alter']['html'] = 0;
  $handler->display->display_options['fields']['commerce_discount_usage']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['commerce_discount_usage']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['commerce_discount_usage']['hide_empty'] = 0;
  $handler->display->display_options['fields']['commerce_discount_usage']['empty_zero'] = 0;
  $handler->display->display_options['fields']['commerce_discount_usage']['hide_alter_empty'] = 1;

  return $handler->display->display_options['fields'];
}
