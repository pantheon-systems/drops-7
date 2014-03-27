<?php
/**
 * @file
 * button.vars.php
 */

/**
 * Implements hook_preprocess_button().
 */
function bootstrap_preprocess_button(&$vars) {
  $element = &$vars['element'];

  // Set the element's attributes.
  element_set_attributes($element, array('id', 'name', 'value', 'type'));

  // Add the base Bootstrap button class.
  $element['#attributes']['class'][] = 'btn';

  // Colorize button.
  _bootstrap_colorize_button($element);

  // Add in the button type class.
  $element['#attributes']['class'][] = 'form-' . $element['#button_type'];

  // Ensure that all classes are unique, no need for duplicates.
  $element['#attributes']['class'] = array_unique($element['#attributes']['class']);
}
