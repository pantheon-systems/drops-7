<?php
/**
 * @file
 * process.inc
 *
 * Contains various implementations for #process callbacks on elements.
 */

/**
 * Process all elements.
 */
function _bootstrap_process_element(&$element, &$form_state) {
  if (!empty($element['#attributes']['class']) && is_array($element['#attributes']['class'])) {
    $key = array_search('container-inline', $element['#attributes']['class']);
    if ($key !== FALSE) {
      $element['#attributes']['class'][$key] = 'form-inline';
    }
    if (in_array('form-wrapper', $element['#attributes']['class'])) {
      $element['#attributes']['class'][] = 'form-group';
    }
  }
  // Process form actions.
  if (!empty($element['#type']) && $element['#type'] === 'actions') {
    foreach (element_children($element) as $child) {
      _bootstrap_iconize_button($element[$child]);
    }
  }
  // Automatically inject the nearest button found after this element if
  // #input_group_button exists.
  if (!empty($element['#input_group_button'])) {
    // Obtain the parent array to limit search.
    $array_parents = array();
    if (!empty($element['#array_parents'])) {
      $array_parents += $element['#array_parents'];
      // Remove the current element from the array.
      array_pop($array_parents);
    }
    // If element is nested, return the referenced parent from the form.
    if (!empty($array_parents)) {
      $parent = &drupal_array_get_nested_value($form_state['complete form'], $array_parents);
    }
    // Otherwise return the complete form.
    else {
      $parent = &$form_state['complete form'];
    }
    // Ignore buttons before we find the element in the form.
    $found_current_element = FALSE;
    foreach (element_children($parent) as $child) {
      if ($parent[$child] === $element) {
        $found_current_element = TRUE;
        continue;
      }
      if ($found_current_element && _bootstrap_is_button($parent[$child])) {
        _bootstrap_iconize_button($parent[$child]);
        $element['#field_suffix'] = drupal_render($parent[$child]);
        break;
      }
    }
  }
  return $element;
}

/**
 * Process input elements.
 */
function _bootstrap_process_input(&$element, &$form_state) {
  // Only add the "form-control" class for specific element input types.
  $types = array(
    // Core.
    'password',
    'password_confirm',
    'select',
    'textarea',
    'textfield',
    // HTML5.
    'email',
    // Webform module.
    'webform_email',
    'webform_number',
    // Elements module.
    'emailfield',
    'numberfield',
    'rangefield',
    'searchfield',
    'telfield',
    'urlfield',
  );
  if (!empty($element['#type']) && (in_array($element['#type'], $types) || ($element['#type'] === 'file' && empty($element['#managed_file'])))) {
    $element['#attributes']['class'][] = 'form-control';
  }
  if (!empty($element['#description']) && empty($element['#attributes']['title']) && _bootstrap_tooltip_description($element['#description'])) {
    $element['#attributes']['title'] = $element['#description'];
  }
  if (!empty($element['#attributes']['title'])) {
    $element['#attributes']['data-toggle'] = 'tooltip';
  }
  return $element;
}

/**
 * Process fieldset element.
 */
function _bootstrap_process_fieldset(&$element, &$form_state) {
  $parents = implode('][', $element['#parents']);

  // Each fieldset forms a new group. The #type 'vertical_tabs' basically only
  // injects a new fieldset.
  $form_state['groups'][$parents]['#group_exists'] = TRUE;
  $element['#groups'] = &$form_state['groups'];

  // Process vertical tabs group member fieldsets.
  if (isset($element['#group'])) {
    // Add this fieldset to the defined group (by reference).
    $element['#theme_wrappers'] = array('bootstrap_panel');
    $group = $element['#group'];
    $form_state['groups'][$group][] = &$element;
  }

  // Contains form element summary functionality.
  $element['#attached']['library'][] = array('system', 'drupal.form');

  // Collapsible fieldsets.
  if (!empty($element['#collapsible'])) {
    $element['#attached']['library'][] = array('system', 'drupal.collapse');
    $element['#attributes']['class'][] = 'collapsible';
    if (!empty($element['#collapsed'])) {
      $element['#attributes']['class'][] = 'collapsed';
    }
  }

  // The .form-wrapper class is required for #states to treat fieldsets like
  // containers.
  if (!isset($element['#attributes']['class'])) {
    $element['#attributes']['class'] = array();
  }

  return $element;
}
