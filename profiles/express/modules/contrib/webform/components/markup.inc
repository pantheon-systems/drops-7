<?php

/**
 * @file
 * Webform module markup component.
 */

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_markup() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'extra' => array(
      'format' => NULL,
      'private' => FALSE,
      'display_on' => 'form',
    ),
  );
}

/**
 * Implements _webform_theme_component().
 */
function _webform_theme_markup() {
  return array(
    'webform_display_markup' => array(
      'render element' => 'element',
      'file' => 'components/markup.inc',
    ),
  );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_markup($component) {
  $form = array();
  $form['value'] = array(
    '#type' => 'text_format',
    '#title' => t('Value'),
    '#default_value' => $component['value'],
    '#description' => t('Markup allows you to enter custom HTML into your form.') . ' ' . theme('webform_token_help', array('groups' => array('node', 'submission'))),
    '#weight' => -1,
    '#format' => $component['extra']['format'],
    '#element_validate' => array('_webform_edit_markup_validate'),
  );

  $form['display']['display_on'] = array(
    '#type' => 'select',
    '#title' => t('Display on'),
    '#default_value' => $component['extra']['display_on'],
    '#options' => array(
      'form' => t('form only'),
      'display' => t('viewed submission only'),
      'both' => t('both form and viewed submission'),
    ),
    '#weight' => 1,
    '#parents' => array('extra', 'display_on'),
  );

  return $form;
}

/**
 * Element validate handler; Set the text format value.
 */
function _webform_edit_markup_validate($form, &$form_state) {
  if (is_array($form_state['values']['value'])) {
    $form_state['values']['extra']['format'] = $form_state['values']['value']['format'];
    $form_state['values']['value'] = $form_state['values']['value']['value'];
  }
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_markup($component, $value = NULL, $filter = TRUE, $submission = NULL) {

  $element = array(
    '#type' => 'markup',
    '#title' => $filter ? NULL : $component['name'],
    '#weight' => $component['weight'],
    '#markup' => $component['value'],
    '#format' => $component['extra']['format'],
    '#theme_wrappers' => array('webform_element'),
    '#translatable' => array('title', 'markup'),
    '#access' => $component['extra']['display_on'] != 'display',
    '#webform_nid' => isset($component['nid']) ? $component['nid'] : NULL,
    '#webform_submission' => $submission,
    '#webform_format' => $component['extra']['format'],
  );

  if ($filter) {
    $element['#after_build'] = array('_webform_render_markup_after_build');
  }

  return $element;
}

/**
 * Helper function to replace tokens in markup component.
 *
 * Required to have access to current form values from $form_state.
 */
function _webform_render_markup_after_build($form_element, &$form_state) {
  global $user;
  $node = node_load($form_element['#webform_nid']);
  $submission = NULL;

  // Convert existing submission data to an in-progress submission.
  $form_state_for_submission = $form_state;
  $form_state_for_submission['values']['submitted'] = $form_state['#conditional_values'];
  module_load_include('inc', 'webform', 'includes/webform.submissions');
  $submission = webform_submission_create($node, $user, $form_state_for_submission, TRUE, $form_element['#webform_submission']);

  // Replace tokens using the current or generated submission.
  $value = webform_replace_tokens($form_element['#markup'], $node, $submission, NULL, $form_element['#webform_format']);

  // If the markup value has been set by a conditional, display that value.
  $component = $form_element['#webform_component'];
  if ($node) {
    $sorter = webform_get_conditional_sorter($node);
    // If the form was retrieved from the form cache, the conditionals may not
    // have been executed yet.
    if (!$sorter->isExecuted()) {
      $sorter->executeConditionals(isset($submission) ? $submission->data : array());
    }
    $conditional_value = $sorter->componentMarkup($component['cid'], $component['page_num']);
    if (isset($conditional_value)) {
      // Provide original value, should conditional logic no longer set the value.
      $form_element['#wrapper_attributes']['data-webform-markup'] = $value;
      if (is_string($conditional_value)) {
        $value = check_markup($conditional_value, $component['extra']['format']);
      }
    }
  }

  $form_element['#markup'] = $value;
  return $form_element;
}

/**
 * Implements _webform_display_component().
 */
function _webform_display_markup($component, $value, $format = 'html', $submission = array()) {
  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;
  $value = webform_replace_tokens($component['value'], $node, $submission, NULL, $component['extra']['format']);

  // If the markup value has been set by a conditional, display that value.
  if ($node && is_string($conditional_value = webform_get_conditional_sorter($node)->componentMarkup($component['cid'], $component['page_num']))) {
    $value = check_markup($conditional_value, $component['extra']['format']);
  }

  return array(
    '#weight' => $component['weight'],
    '#theme' => 'webform_display_markup',
    '#format' => $format,
    '#value' => $value,
    '#translatable' => array('title'),
    '#access' => $component['extra']['display_on'] != 'form',
  );
}

/**
 * Format the output of data for this component.
 */
function theme_webform_display_markup($variables) {
  $element = $variables['element'];
  return $element['#access'] ? $element['#value'] : '';
}
