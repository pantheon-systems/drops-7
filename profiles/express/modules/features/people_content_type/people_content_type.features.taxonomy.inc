<?php
/**
 * @file
 * people_content_type.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function people_content_type_taxonomy_default_vocabularies() {
  return array(
    'department' => array(
      'name' => 'Department',
      'machine_name' => 'department',
      'description' => '',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
    'people_filter_1' => array(
      'name' => 'People Filter 1',
      'machine_name' => 'people_filter_1',
      'description' => 'A vocabulary to filter person nodes',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
    'people_filter_2' => array(
      'name' => 'People Filter 2',
      'machine_name' => 'people_filter_2',
      'description' => 'A vocabulary to filter person nodes',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
    'people_filter_3' => array(
      'name' => 'People Filter 3',
      'machine_name' => 'people_filter_3',
      'description' => 'A vocabulary to filter person nodes',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
    'person_type' => array(
      'name' => 'Job Type',
      'machine_name' => 'person_type',
      'description' => 'Used to group people in to categories such as administration, faculty, staff, etc.',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
  );
}
