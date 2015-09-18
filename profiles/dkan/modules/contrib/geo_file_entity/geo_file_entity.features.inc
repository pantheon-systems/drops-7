<?php
/**
 * @file
 * geo_file_entity.features.inc
 */

/**
 * Implements hook_eck_entity_type_info().
 */
function geo_file_entity_eck_entity_type_info() {
  $items = array(
    'geo_file' => array(
      'name' => 'geo_file',
      'label' => 'Geo File',
      'properties' => array(
        'title' => array(
          'label' => 'Title',
          'type' => 'text',
          'behavior' => 'title',
        ),
        'uid' => array(
          'label' => 'Author',
          'type' => 'integer',
          'behavior' => 'author',
        ),
        'created' => array(
          'label' => 'Created',
          'type' => 'integer',
          'behavior' => 'created',
        ),
        'changed' => array(
          'label' => 'Changed',
          'type' => 'integer',
          'behavior' => 'changed',
        ),
      ),
    ),
  );
  return $items;
}
