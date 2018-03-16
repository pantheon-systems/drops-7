<?php

/**
 * @file
 * Hooks provided by the File Resumable Upload module.
 */

/**
 * Declare the field widget types supported by the module.
 *
 * @return
 *   A single field widget type or an array of field widget types.
 */
function hook_file_resup_supported_field_widget_types() {
  return array('file_generic', 'image_image');
}

/**
 * Alter the supported field widget types.
 *
 * @param $types
 *   The types returned by hook_file_resup_supported_field_widget_types(), keyed
 *   by type.
 *
 * @see hook_file_resup_supported_field_widget_types()
 */
function hook_file_resup_supported_field_widget_types_alter(&$types) {
  // Remove support for the image_image field widget type.
  unset($types['image_image']);
}
