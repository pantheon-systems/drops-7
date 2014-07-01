<?php
/**
 * @file
 * container.func.php
 */

/**
 * Overrides theme_container().
 */
function bootstrap_container($variables) {
  $element = $variables['element'];

  // Special handling for form elements.
  if (isset($element['#array_parents'])) {
    // Assign an html ID.
    if (!isset($element['#attributes']['id'])) {
      $element['#attributes']['id'] = $element['#id'];
    }

    // Core's "form-wrapper" class is required for states.js to function.
    $element['#attributes']['class'][] = 'form-wrapper';

    // Add Bootstrap "form-group" class.
    $element['#attributes']['class'][] = 'form-group';
  }

  return '<div' . drupal_attributes($element['#attributes']) . '>' . $element['#children'] . '</div>';
}
