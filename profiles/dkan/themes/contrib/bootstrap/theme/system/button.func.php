<?php
/**
 * @file
 * button.func.php
 */

/**
 * Overrides theme_button().
 */
function bootstrap_button($variables) {
  $element = $variables['element'];

  // Add icons before or after the value.
  // @see https://drupal.org/node/2219965
  $value = $element['#value'];
  if (!empty($element['#icon'])) {
    if ($element['#icon_position'] === 'before') {
      $value = $element['#icon'] . ' ' . $value;
    }
    elseif ($element['#icon_position'] === 'after') {
      $value .= ' ' . $element['#icon'];
    }
  }

  // This line break adds inherent margin between multiple buttons.
  return '<button' . drupal_attributes($element['#attributes']) . '>' . $value . "</button>\n";
}
