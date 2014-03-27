<?php
/**
 * @file
 * form-element.func.php
 */

/**
 * Overrides theme_form_element().
 */
function bootstrap_form_element(&$variables) {
  $element = &$variables['element'];
  $is_checkbox = FALSE;
  $is_radio = FALSE;

  // This function is invoked as theme wrapper, but the rendered form element
  // may not necessarily have been processed by form_builder().
  $element += array(
    '#title_display' => 'before',
  );

  if (empty($element['#wrapper_attributes'])) {
    $element['#wrapper_attributes'] = array();
  }
  $wrapper_attributes = &$element['#wrapper_attributes'];

  // Add element #id for #type 'item'.
  if (isset($element['#markup']) && !empty($element['#id'])) {
    $wrapper_attributes['id'] = $element['#id'];
  }

  // Check for errors and set correct error class.
  if (isset($element['#parents']) && form_get_error($element)) {
    $wrapper_attributes['class'][] = 'has-error';
  }

  if (!empty($element['#type'])) {
    $wrapper_attributes['class'][] = 'form-type-' . strtr($element['#type'], '_', '-');
  }
  if (!empty($element['#name'])) {
    $wrapper_attributes['class'][] = 'form-item-' . strtr($element['#name'], array(
        ' ' => '-',
        '_' => '-',
        '[' => '-',
        ']' => '',
      ));
  }
  // Add a class for disabled elements to facilitate cross-browser styling.
  if (!empty($element['#attributes']['disabled'])) {
    $wrapper_attributes['class'][] = 'form-disabled';
  }
  if (!empty($element['#autocomplete_path']) && drupal_valid_path($element['#autocomplete_path'])) {
    $wrapper_attributes['class'][] = 'form-autocomplete';
  }
  $wrapper_attributes['class'][] = 'form-item';

  // See http://getbootstrap.com/css/#forms-controls.
  if (isset($element['#type'])) {
    if ($element['#type'] == "radio") {
      $wrapper_attributes['class'][] = 'radio';
      $is_radio = TRUE;
    }
    elseif ($element['#type'] == "checkbox") {
      $wrapper_attributes['class'][] = 'checkbox';
      $is_checkbox = TRUE;
    }
    else {
      $wrapper_attributes['class'][] = 'form-group';
    }
  }

  $output = '<div' . drupal_attributes($wrapper_attributes) . '>' . "\n";

  // If #title is not set, we don't display any label or required marker.
  if (!isset($element['#title'])) {
    $element['#title_display'] = 'none';
  }

  $prefix = '';
  $suffix = '';
  if (isset($element['#field_prefix']) || isset($element['#field_suffix'])) {
    // Determine if "#input_group" was specified.
    if (!empty($element['#input_group'])) {
      $prefix .= '<div class="input-group">';
      $prefix .= isset($element['#field_prefix']) ? '<span class="input-group-addon">' . $element['#field_prefix'] . '</span>' : '';
      $suffix .= isset($element['#field_suffix']) ? '<span class="input-group-addon">' . $element['#field_suffix'] . '</span>' : '';
      $suffix .= '</div>';
    }
    // Determine if "#input_group_button" was specified.
    elseif (!empty($element['#input_group_button'])) {
      $prefix .= '<div class="input-group">';
      $prefix .= isset($element['#field_prefix']) ? '<span class="input-group-btn">' . $element['#field_prefix'] . '</span>' : '';
      $suffix .= isset($element['#field_suffix']) ? '<span class="input-group-btn">' . $element['#field_suffix'] . '</span>' : '';
      $suffix .= '</div>';
    }
    else {
      $prefix .= isset($element['#field_prefix']) ? $element['#field_prefix'] : '';
      $suffix .= isset($element['#field_suffix']) ? $element['#field_suffix'] : '';
    }
  }

  switch ($element['#title_display']) {
    case 'before':
    case 'invisible':
      $output .= ' ' . theme('form_element_label', $variables);
      $output .= ' ' . $prefix . $element['#children'] . $suffix . "\n";
      break;

    case 'after':
      if ($is_radio || $is_checkbox) {
        $output .= ' ' . $prefix . $element['#children'] . $suffix;
      }
      else {
        $variables['#children'] = ' ' . $prefix . $element['#children'] . $suffix;
      }
      $output .= ' ' . theme('form_element_label', $variables) . "\n";
      break;

    case 'none':
    case 'attribute':
      // Output no label and no required marker, only the children.
      $output .= ' ' . $prefix . $element['#children'] . $suffix . "\n";
      break;
  }

  if (!empty($element['#description']) && empty($element['#attributes']['title']) && !_bootstrap_tooltip_description($element['#description'])) {
    $output .= '<p class="help-block">' . $element['#description'] . "</p>\n";
  }

  $output .= "</div>\n";

  return $output;
}
