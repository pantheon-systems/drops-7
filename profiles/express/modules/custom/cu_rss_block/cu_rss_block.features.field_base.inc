<?php
/**
 * @file
 * cu_rss_block.features.field_base.inc
 */

/**
 * Implements hook_field_default_field_bases().
 */
function cu_rss_block_field_default_field_bases() {
  $field_bases = array();

  // Exported field_base: 'field_rss_bean_feed_url'.
  $field_bases['field_rss_bean_feed_url'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_rss_bean_feed_url',
    'indexes' => array(),
    'locked' => 0,
    'module' => 'link',
    'settings' => array(
      'attributes' => array(
        'class' => '',
        'rel' => '',
        'target' => 'default',
      ),
      'display' => array(
        'url_cutoff' => 80,
      ),
      'enable_tokens' => 1,
      'title' => 'optional',
      'title_maxlength' => 128,
      'title_value' => '',
      'url' => 0,
    ),
    'translatable' => 0,
    'type' => 'link_field',
  );

  // Exported field_base: 'field_rss_bean_items'.
  $field_bases['field_rss_bean_items'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_rss_bean_items',
    'indexes' => array(
      'value' => array(
        0 => 'value',
      ),
    ),
    'locked' => 0,
    'module' => 'list',
    'settings' => array(
      'allowed_values' => array(
        1 => 1,
        2 => 2,
        3 => 3,
        4 => 4,
        5 => 5,
        6 => 6,
        7 => 7,
        8 => 8,
        9 => 9,
        10 => 10,
      ),
      'allowed_values_function' => '',
      'allowed_values_php' => '',
    ),
    'translatable' => 0,
    'type' => 'list_integer',
  );

  return $field_bases;
}
