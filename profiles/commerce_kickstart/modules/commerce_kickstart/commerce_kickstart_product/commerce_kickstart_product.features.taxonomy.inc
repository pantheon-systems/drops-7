<?php
/**
 * @file
 * commerce_kickstart_product.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function commerce_kickstart_product_taxonomy_default_vocabularies() {
  return array(
    'bag_size' => array(
      'name' => 'Bag size',
      'machine_name' => 'bag_size',
      'description' => '',
      'hierarchy' => '0',
      'module' => 'taxonomy',
      'weight' => '0',
    ),
    'brand' => array(
      'name' => 'Brand',
      'machine_name' => 'brand',
      'description' => '',
      'hierarchy' => '0',
      'module' => 'taxonomy',
      'weight' => '0',
    ),
    'category' => array(
      'name' => 'Category',
      'machine_name' => 'category',
      'description' => '',
      'hierarchy' => '0',
      'module' => 'taxonomy',
      'weight' => '0',
    ),
    'collection' => array(
      'name' => 'Collection',
      'machine_name' => 'collection',
      'description' => '',
      'hierarchy' => '0',
      'module' => 'taxonomy',
      'weight' => '0',
    ),
    'color' => array(
      'name' => 'Color',
      'machine_name' => 'color',
      'description' => '',
      'hierarchy' => '0',
      'module' => 'taxonomy',
      'weight' => '0',
    ),
    'gender' => array(
      'name' => 'Gender',
      'machine_name' => 'gender',
      'description' => '',
      'hierarchy' => '0',
      'module' => 'taxonomy',
      'weight' => '0',
    ),
    'hat_size' => array(
      'name' => 'Hat size',
      'machine_name' => 'hat_size',
      'description' => '',
      'hierarchy' => '0',
      'module' => 'taxonomy',
      'weight' => '0',
    ),
    'shoe_size' => array(
      'name' => 'Shoe size',
      'machine_name' => 'shoe_size',
      'description' => '',
      'hierarchy' => '0',
      'module' => 'taxonomy',
      'weight' => '0',
    ),
    'storage_capacity' => array(
      'name' => 'Storage capacity',
      'machine_name' => 'storage_capacity',
      'description' => '',
      'hierarchy' => '0',
      'module' => 'taxonomy',
      'weight' => '0',
    ),
    'top_size' => array(
      'name' => 'Top size',
      'machine_name' => 'top_size',
      'description' => '',
      'hierarchy' => '0',
      'module' => 'taxonomy',
      'weight' => '0',
    ),
  );
}
