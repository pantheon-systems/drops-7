<?php
/**
 * @file
 * cu_collections.bean.inc
 */

/**
 * Implements hook_bean_admin_ui_types().
 */
function cu_collections_bean_admin_ui_types() {
  $export = array();

  $bean_type = new stdClass();
  $bean_type->disabled = FALSE; /* Edit this to true to make a default bean_type disabled initially */
  $bean_type->api_version = 5;
  $bean_type->name = 'collection_grid';
  $bean_type->label = 'Collection Grid';
  $bean_type->options = '';
  $bean_type->description = 'A filterable grid of collection items.';
  $export['collection_grid'] = $bean_type;

  return $export;
}
