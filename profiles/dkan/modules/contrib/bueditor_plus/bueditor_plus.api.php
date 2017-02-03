<?php
/**
 * @file
 * API documentation for BUEditor Plus.
 */

/**
 * Alter the Drupal fields allowed to have BUEditor applied.
 *
 * @param array $fields
 *   An array containing the system name of Drupal fields that are allowed
 *   to have BUEditor Plus render BUEditor on. This array is checked against
 *   the field widget type when rendering the field_ui_field_edit_form to
 *   determine rather the BUEditor Plus settings should appear on that form,
 *   as well as when fields are processed through
 *   bueditor_plus_process_format().
 */
function hook_bueditor_plus_allowed_fields_alter(&$fields) {

  // Add our new fields to the array of allowed bueditor plus fields.
  $fields[] = 'text_textarea';
}
