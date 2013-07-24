<?php
/**
 * @file
 * commerce_kickstart_search.current_search.inc
 */

/**
 * Implements hook_current_search_default_items().
 */
function commerce_kickstart_search_current_search_default_items() {
  $export = array();

  $item = new stdClass();
  $item->disabled = FALSE; /* Edit this to true to make a default item disabled initially */
  $item->api_version = 1;
  $item->name = 'kickstart_search';
  $item->label = 'Search results';
  $item->settings = array(
    'items' => array(
      'results' => array(
        'id' => 'text',
        'label' => 'Results',
        'text' => 'There is <em>[facetapi_results:result-count]</em> search result',
        'plural' => 1,
        'text_plural' => 'There are <em>[facetapi_results:result-count]</em> search results',
        'plural_condition' => 'facetapi_results:result-count',
        'wrapper' => 1,
        'element' => 'h3',
        'css' => 1,
        'classes' => 'pseudo-breadcrumb-total-results',
        'weight' => '-50',
      ),
      'active_items' => array(
        'id' => 'active',
        'label' => 'Active items',
        'pattern' => '[facetapi_active:active-value]',
        'keys' => 1,
        'css' => 1,
        'classes' => 'pseudo-breadcrumb',
        'nofollow' => 1,
        'weight' => '-49',
      ),
    ),
    'advanced' => array(
      'empty_searches' => 3,
    ),
  );
  $export['kickstart_search'] = $item;

  return $export;
}
