<?php

/**
 * @file
 * This contains documentation only.
 */

/**
 * Using the Autocomplete Deluxe element.
 *
 * When you want to use the Autocomplete Deluxe element, you have to choose
 * between two types sources for the suggestion data: Ajax Callbacks or Lists.
 * You can set the source type by using the appropriate options:
 * - #autocomplete_deluxe_path expects a string with an url, that points to the
 *   ajax callback. The response should be encoded as json (like for the core
 *   autocomplete).
 * - #autocomplete_options needs an array in the form of an array (similar to
 *   #options in core for selects or checkboxes): array('a', 'b', 'c') or
 *   array(1 => 'a', 2 => 'b', 3 => 'c').
 *
 * Besides these two, there are four other options which autocomplete deluxe
 * accepts:
 * - #multiple Indicates whether the user may select more than one item. Expects
 *   TRUE or FALSE, by default it is set to FALSE.
 * - #delimiter If #multiple is TRUE, then you can use this option to set a
 *   seperator for multiple values. By default a string with the following
 *   content will be used: ', '.
 * - #min_length Indicates how many characters must be entered
 *   until, the suggesion list can be opened. Especially helpful, when your
 *   ajax callback returns only valid suggestion for a minimum characters.
 *   The default is 0.
 * - #not_found_message A message text which will be displayed, if the entered
 *   term was not found.
 */
function somefunction() {
  switch ($type) {
    case 'list':
      $element = array(
        '#type' => 'autocomplete_deluxe',
        '#autocomplete_options' => $options,
        '#multiple' => FALSE,
        '#min_length' => 0,
      );
      break;

    case 'ajax':
      $element = array(
        '#type' => 'autocomplete_deluxe',
        '#autocomplete_deluxe_path' => url('some_uri', array('absolute' => TRUE)),
        '#multiple' => TRUE,
        '#min_length' => 1,
        '#delimiter' => '|',
        '#not_found_message' => "The term '@term' will be added.",
      );
      break;
  }
}
