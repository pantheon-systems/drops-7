<?php
/**
 * @file
 * commerce_kickstart_search.facetapi_defaults.inc
 */

/**
 * Implements hook_facetapi_default_facet_settings().
 */
function commerce_search_api_facetapi_default_facet_settings() {
  return commerce_search_api_generate_facets('product_display');
}
