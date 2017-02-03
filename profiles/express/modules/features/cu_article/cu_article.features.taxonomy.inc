<?php
/**
 * @file
 * cu_article.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function cu_article_taxonomy_default_vocabularies() {
  return array(
    'byline' => array(
      'name' => 'Byline',
      'machine_name' => 'byline',
      'description' => 'Byline/person reference for articles',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
    'category' => array(
      'name' => 'Category',
      'machine_name' => 'category',
      'description' => 'Categories are general topics for broad grouping of articles.',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
    'tags' => array(
      'name' => 'Tags',
      'machine_name' => 'tags',
      'description' => 'Tags describe the specific details of your articles.',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
  );
}
