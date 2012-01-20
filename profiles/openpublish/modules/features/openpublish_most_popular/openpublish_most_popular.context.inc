<?php
/**
 * @file
 * openpublish_most_popular.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function openpublish_most_popular_context_default_contexts() {
  $export = array();

  $context = new stdClass;
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'search_results';
  $context->description = 'Provides sidebar blocks for the search page';
  $context->tag = 'Search';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        'search/*' => 'search/*',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'boxes-box-4cc2a9f2' => array(
          'module' => 'boxes',
          'delta' => 'box-4cc2a9f2',
          'region' => 'sidebar_first',
          'weight' => '-10',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Provides sidebar blocks for the search page');
  t('Search');
  $export['search_results'] = $context;

  return $export;
}
