<?php

/**
 * @file
 * Hooks provided by the site_verify module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Define supported search engines.
 */
function hook_site_verify_engine_info() {
  $engines['myengine'] = array(
    // Note that the key is limited to 32 characters.
    'name' => t('My search engine'),
    'page' => TRUE,
    'page_example' => 'myengine.html',
    'page_validate' => 'myengine_validate_page',
    'page_contents' => FALSE,
    'page_contents_validate' => FALSE,
    'meta' => TRUE,
    'meta_validate' => 'myengine_validate_meta',
  );
  return $engines;
}

/**
 * Alter the list of supported search engines.
 *
 * @param $engines
 *   An array (passed by reference) of the list of engines, keyed by engine
 *   type.
 */
function hook_site_verify_engine_alter(&$engines) {
  $engines['myengine']['meta'] = FALSE;
}

/**
 * @} End of "addtogroup hooks".
 */
