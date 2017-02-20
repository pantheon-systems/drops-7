<?php

/**
 * @file
 * Hooks provided by the Search API sorts module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Alter the Search API sorts default sort.
 *
 * Modules may implement this hook to alter the default sort used by Search API
 * sorts module.
 *
 * @param object $default_sort
 *   The Search API sort object used as default sort.
 * @param array $search_sorts
 *   An array of all enabled Search API sort objects.
 * @param $keys
 *   The searched terms
 *
 * @see _search_api_sorts_get_default_sort()
 */
function hook_search_api_sorts_default_sort_alter(&$default_sort, array $search_sorts, $keys) {
  // Example which alters the default sort to use title instead. This is
  // example does not make a difference between default and a search page with
  // search queries provided.
  foreach ($search_sorts as $search_sort) {
    if ($search_sort->field == 'title') {
      $default_sort = $search_sort;
    }
  }
}
