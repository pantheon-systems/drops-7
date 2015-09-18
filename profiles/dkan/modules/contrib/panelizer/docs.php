<?php
/**
 * @file
 *
 * Documentation for panelizer hooks.
 */

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
