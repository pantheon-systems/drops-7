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

/**
 * Alter the xml content just before sending to Lingotek for translation.
 *
 * @param array $params
 *  An associative array containing the following:
 *  - entity_type: a string naming the entity type
 *  - entity: a reference to the entity object for which content is being sent
 *  - xml: a LingotekXMLElement object containing the content to be translated.
 */
function hook_lingotek_entity_upload_alter(array &$params) {
}

/**
 * Alter the xml content just after receiving a translated entity from Lingotek.
 *
 * @param array $params
 *  An associative array containing the following:
 *  - entity_type: a string naming the entity type
 *  - entity: a reference to the entity object for which content is received.
 *  - xml: a LingotekXMLElement object containing the translated content.
 *  - langcode: the Drupal language code associated with the content.
 */
function hook_lingotek_entity_download_alter(array &$params) {
}

/**
 * Alter the support for entity types within the Lingotek module.
 *
 * @param array $enabled_types
 *  An associative array containing the entities currently Lingotek-enabled.
 * @param bool $include_all
 *  Whether all translatable entity types should be shown, or just the ones
 *  that should appear on the bulk-manage grid.
 */
function hook_lingotek_managed_entity_types_alter(array &$enabled_types, $include_all) {
}

/**
 * Alter the profile options available for a given entity type.
 *
 * @param string $entity_type
 *  The entity type for which profile options should be modified.
 * @param array $options
 *  An associative array containing the options for the given entity type.
 */
function hook_lingotek_entity_profiles_options_alter($entity_type, array &$options) {
}

/**
* Alter the source URL of an entity before upload to TMS.
*
* @param string $source_url
*  The entity's url.
*/
function hook_lingotek_source_URL_alter(&$source_url) {
}
