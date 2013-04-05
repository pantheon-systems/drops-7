<?php

/**
 * @file
 * This file contains no working PHP code; it exists to provide additional
 * documentation for doxygen as well as to document hooks in the standard
 * Drupal manner.
 */

/**
 * Allows modules to alter the list of customer profile labels.
 *
 * During checkout the user selects the "address on file" from a dropdown list
 * of customer profiles. The label used to represent each customer profile
 * is the "thoroughfare" column of the addressfield. By altering the list
 * of labels, a module can use additional data to represent each customer
 * profile. Note that the list should always be keyed by customer profile id.
 *
 * @param $labels
 *   An array of labels, keyed by customer profile id.
 * @param $profiles
 *   An array of customer profile entities.
 */
function hook_commerce_addressbook_labels_alter(&$labels, $profiles) {
  // No example.
}

/**
 * Allows modules to alter the AJAX commands when selecting another customer
 * profile.
 *
 * During checkout the user selects the "address on file" from a dropdown list
 * of customer profiles. The response (prefilled fields) is handled using AJAX.
 * By altering the array of AJAX commands, a module can add additional commands
 * to the response.
 *
 * @param array $commands
 *   An array of AJAX commands.
 * @param type $form
 *   Nested array of form elements that comprise the form.
 * @param type $form_state
 *   A keyed array containing the current state of the form.
 */
function hook_commerce_addressbook_callback_alter(&$commands, $form, $form_state) {
  // Example.
  $commands[] = ajax_command_alert('It works!');
}
