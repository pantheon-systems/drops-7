<?php
/**
 * @file
 * textfield.func.php
 */

/**
 * Overrides theme_textfield().
 */
function bootstrap_textfield($variables) {
  $element = $variables['element'];
  $element['#attributes']['type'] = 'text';
  element_set_attributes($element, array(
    'id',
    'name',
    'value',
    'size',
    'maxlength',
  ));
  _form_set_class($element, array('form-text'));

  $output = '<input' . drupal_attributes($element['#attributes']) . ' />';

  $extra = '';
  if ($element['#autocomplete_path'] && drupal_valid_path($element['#autocomplete_path'])) {
    drupal_add_library('system', 'drupal.autocomplete');
    $element['#attributes']['class'][] = 'form-autocomplete';

    $attributes = array();
    $attributes['type'] = 'hidden';
    $attributes['id'] = $element['#attributes']['id'] . '-autocomplete';
    $attributes['value'] = url($element['#autocomplete_path'], array('absolute' => TRUE));
    $attributes['disabled'] = 'disabled';
    $attributes['class'][] = 'autocomplete';
    // Uses icon for autocomplete "throbber".
    if ($icon = _bootstrap_icon('refresh')) {
      $output = '<div class="input-group">' . $output . '<span class="input-group-addon">' . $icon . '</span></div>';
    }
    // Fallback to using core's throbber.
    else {
      $output = '<div class="input-group">' . $output . '<span class="input-group-addon">';
      // The throbber's background image must be set here because sites may not
      // be at the root of the domain (ie: /) and this value cannot be set via
      // CSS.
      $output .= '<span class="autocomplete-throbber" style="background-image:url(' . url('misc/throbber.gif') . ')"></span>';
      $output .= '</span></div>';
    }
    $extra = '<input' . drupal_attributes($attributes) . ' />';
  }

  return $output . $extra;
}
