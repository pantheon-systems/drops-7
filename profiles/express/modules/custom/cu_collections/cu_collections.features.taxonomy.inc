<?php
/**
 * @file
 * cu_collections.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function cu_collections_taxonomy_default_vocabularies() {
  return array(
    'collection_categories' => array(
      'name' => 'Collection Categories',
      'machine_name' => 'collection_categories',
      'description' => '',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
    'collection_type' => array(
      'name' => 'Collection Type',
      'machine_name' => 'collection_type',
      'description' => 'A type of collection, such as program, resource, scholarship, etc.',
      'hierarchy' => 1,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
  );
}
