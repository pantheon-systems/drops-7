<?php

/**
 * @file
 * Documents hooks provided by this module.
 *
 * @author Jim Berry ("solotandem", http://drupal.org/user/240748)
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Alter the state of snippet insertion on the current page response.
 *
 * This hook allows other modules to alter the state of snippet insertion based
 * on custom conditions that cannot be defined by the status, path, and role
 * conditions provided by this module.
 *
 * @param bool $satisfied
 *   The snippet insertion state.
 */
function hook_google_tag_insert_alter(&$satisfied) {
  // Do something to the state.
  $state = !$state;
}

/**
 * Alter the realm values for the current page response.
 *
 * This hook allows other modules to alter the realm values that affect the
 * snippets to be inserted.
 *
 * @param array $realm
 *   Associative array of realm values keyed by name and key.
 */
function hook_google_tag_realm_alter(&$realm) {
  // Do something to the realm values.
  $realm['name'] = 'my_realm';
  $realm['key'] = 'my_key';
}

/**
 * Alter the snippets to be inserted on a page response.
 *
 * This hook allows other modules to alter the snippets to be inserted based on
 * custom settings not defined by this module.
 *
 * @param array $snippets
 *   Associative array of snippets keyed by type: script, noscript and
 *   data_layer.
 */
function hook_google_tag_snippets_alter(&$snippets) {
  // Do something to the script snippet.
  $snippets['script'] = str_replace('insertBefore', 'insertAfter', $snippets['script']);
}
