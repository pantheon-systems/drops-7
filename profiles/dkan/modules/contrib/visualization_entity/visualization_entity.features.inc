<?php
/**
 * @file
 * visualization_entity.features.inc
 */

/**
 * Implements hook_eck_entity_type_info().
 */
function visualization_entity_eck_entity_type_info() {
  $items = array(
    'visualization' => array(
      'name' => 'visualization',
      'label' => 'Visualization',
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
