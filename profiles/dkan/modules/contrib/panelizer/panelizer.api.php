<?php
/**
 * @file
 *
 * Documentation for Panelizer's hooks.
 */

/**
 * Allow panelizer_defaults_override to be customized.
 *
 * Primarily for use by Features Overrides.
 */
function hook_panelizer_defaults_override_alter(&$items) {
}

/**
 * Add operations to Panelizer objects.
 *
 * Operations can be performed on panelizer defaults as well as entities.
 * Panelizer provides the 4 default operations, but modules can add
 * additional operations to add additional functionality.
 *
 * Data can be stored in $panelizer->extra which is a serialized array.
 * Modules should be sure to namespace their keys in this extra to avoid
 * collisions.
 *
 * Each operation supports the following keys:
 * - 'menu title': The title to use in menu tab entries. This will be
 *   translated by the menu system, so do not t() it.
 * - 'link title': The title to use in links. This will not be translated by
 *   the menu system, so t() it. In Drupal, the link title is typically in
 *   lower case when the tab would be in upper case, so this will not quite
 *   match the menu title.
 * - 'entity callback': If not using the normal operation hook on the object,
 *   put this may be a function callback. It will receive the following args:
 *   $handler, $js, $input, $entity, $view_mode.
 * - 'admin callback': The callback to use when editing a panelizer default.
 *   It will receive the following arguments: $handler, $bundle, $name,
 *   $view_mode.
 * - 'file path': A 'file path' entry to be used for hook_menu entries.
 * - 'file': A 'file' entry to be used for hook_menu entries.
 */
function hook_panelizer_operations_alter(&$operations) {
  $operations['example'] = array(
    'menu title' => 'Example',
    'link title' => t('example'),
    'entity callback' => 'mymodule_panelizer_example_entity_page',
    'admin callback' => 'mymodule_panelizer_example_admin_page',
  );
}

/**
 * Allow panelizer_entity_plugin_process to be customized.
 */
function hook_panelizer_entity_plugin_process_alter(&$plugin, $info) {
}

/**
 * Allow the links on the Overview page to be customized.
 */
function hook_panelizer_overview_links_alter(&$links_array, $entity_type, $context) {
}

/**
 * Act on default objects just before they're deleted.
 *
 * @param object $panelizer
 *   The panelizer default object.
 */
function hook_panelizer_delete_default($panelizer) {
  db_delete('example_something')
    ->condition('name', $panelizer->name)
    ->execute();
}

/**
 * Adjust access to the Panelizer administrative interface beyond the standard
 * permissions options.
 *
 * @param string $op
 *   The operation currently being performed.
 * @param string $entity_type
 *   The type of entity to which the operation is related.
 * @param string|object $bundle
 *   Either the entity's bundle name or the entity object itself, will vary
 *   depending upon how it is called.
 * @param string $view_mode
 *   The view mode of the entity related to this operation.
 *
 * @return bool
 *   Whether or not the user has permission to perform this $op.
 */
function hook_panelizer_access($op, $entity_type, $bundle, $view_mode) {
}

/**
 * Allow modules to alter the defined panelizer access definitions.
 *
 * @param array $panelizer_access
 *   An array of panelizer access options. If any are true, this will return
 *   true. Set $panelizer_access equal to an empty array to return false.
 * @param $options
 *   drupal_alter() can only handle so many parameters. In order to pass the
 *   same parameters that are passed in hook_panelizer_access, the params are
 *   placed into an $options array. Expected keys are:
 *     op
 *     entity_type
 *     bundle
 *     view_mode
 */
function hook_panelizer_access_alter(&$panelizer_access, $options) {
}
