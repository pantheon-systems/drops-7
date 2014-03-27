<?php
/**
 * @file
 * webform-element.func.php
 */

/**
 * Implements hook_preprocess_webform_element().
 */
function bootstrap_preprocess_webform_element(&$variables) {
  $element = $variables['element'];
  $wrapper_attributes = array();
  if (isset($element['#wrapper_attributes'])) {
    $wrapper_attributes = $element['#wrapper_attributes'];
  }

  // See http://getbootstrap.com/css/#forms-controls.
  if (isset($element['#type'])) {
    if ($element['#type'] == "radio") {
      $wrapper_attributes['class'][] = 'radio';
    }
    elseif ($element['#type'] == "checkbox") {
      $wrapper_attributes['class'][] = 'checkbox';
    }
    else {
      $wrapper_attributes['class'][] = 'form-group';
    }
  }

  $variables['element']['#wrapper_attributes'] = $wrapper_attributes;
}
