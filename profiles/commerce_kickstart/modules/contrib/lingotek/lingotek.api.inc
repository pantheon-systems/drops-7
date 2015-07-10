<?php

/**
 * @file
 * Describe hooks provided by the Lingotek Translation module.
 */

/**
 * Alter the Lingotek data for a translatable object prior to sending it
 * for translation.  This is called just before new documents are sent to
 * the Lingotek Translation Management System (TMS).
 *
 * @param object $translatable_obj
 *  An instance of the LingotekTranslatableEntity class
 */
function hook_lingotek_entity_pre_upload(LingotekTranslatableEntity $translatable_obj) {
}

/**
 * Alter the regular expression patterns used to find and protect Drupal
 * variables before they are uploaded to Lingotek TMS for translation.
 *
 * @param array $patterns
 *  An array of regular expressions to be modified
 * @param bool $protect_vars
 *  TRUE or FALSE depending on whether !,@,% variables should be protected.
 */
function hook_lingotek_protect_variables_alter(array &$patterns, $protect_vars) {
}

