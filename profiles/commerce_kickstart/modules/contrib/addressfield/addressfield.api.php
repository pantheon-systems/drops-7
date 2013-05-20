<?php

/**
 * @file
 * API documentation for Addressfield.
 */

/**
 * Format generation callback.
 *
 * @param $format
 *   The address format being generated.
 * @param $address
 *   The address this format is generated for.
 * @param $context
 *   An associative array of context information pertaining to how the address
 *   format should be generated. If no mode is given, it will initialize to the
 *   default value. The remaining context keys should only be present when the
 *   address format is being generated for a field:
 *   - mode: either 'form' or 'render'; defaults to 'render'.
 *   - field: the field info array.
 *   - instance: the field instance array.
 *   - langcode: the langcode of the language the field is being rendered in.
 *   - delta: the delta value of the given address.
 *
 * @ingroup addressfield_format
 */
function CALLBACK_addressfield_format_callback(&$format, $address, $context = array()) {
  // No example.
}

/**
 * Allows modules to add arbitrary AJAX commands to the array returned from the
 * standard address field widget refresh.
 *
 * @param &$commands
 *   The array of AJAX commands used to refresh the address field widget.
 * @param $form
 *   The rebuilt form array.
 * @param $form_state
 *   The form state array from the form.
 *
 * @see addressfield_standard_widget_refresh()
 */
function hook_addressfield_standard_widget_refresh_alter(&$commands, $form, $form_state) {
  // Display an alert message.
  $commands[] = ajax_command_alert(t('The address field widget has been updated.'));
}
