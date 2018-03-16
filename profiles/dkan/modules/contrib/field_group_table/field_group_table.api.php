<?php

/**
 * @file
 * Hooks provided by Field Group Table.
 */

/**
 * @defgroup hooks Field Group Table hooks.
 * @{
 * The hooks provided by Field Group Table.
 */

/**
 * Allows modules to alter rows, useful to remove empty rows.
 *
 * When removing rows, you must unset the corresponding key/value pairs in both
 * $element and $children.
 *
 * @param array &$element
 *   An array containing render API code for the field group.
 * @param array &$children
 *   An array with the keys of the render API children of $element as returned
 *   by element_children.
 *
 * @see element_children()
 */
function hook_field_group_table_rows_alter(array &$element, array &$children) {
  // Only operate on "display" mode.
  if ($element['#mode'] != 'display') {
    return;
  }

  $render_api_properties = array('#theme', '#markup', '#prefix', '#suffix');

  foreach ($children as $index => $child) {
    // A row with a multivalue field is empty when the first element contains no
    // render API code.
    if (isset($element[$child][0]) && !array_intersect($render_api_properties, array_keys($element[$child][0]))) {
      unset($children[$index]);
      unset($element[$child]);
    }
  }
}

/**
 * @} End of "defgroup hooks".
 */
