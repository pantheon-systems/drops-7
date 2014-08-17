<?php

/**
 * @file
 * Field related hook implementations for the AddThis-module.
 */

/**
 * Implements hook_field_info().
 */
function addthis_field_info() {
  return array(
    AddThis::FIELD_TYPE => array(
      'label' => t('AddThis'),
      'description' => t('This field stores addthis settings in the database.'),
      'settings' => array('max_length' => 255),
      'default_widget' => AddThis::WIDGET_TYPE,
      'default_formatter' => AddThis::WIDGET_TYPE_DISABLED,
    ),
  );
}

/**
 * Implements hook_field_is_empty().
 */
function addthis_field_is_empty($item, $field) {
  return empty($item['value']) && (string) $item['value'] !== '0';
}

/**
 * Implements hook_field_formatter_info().
 *
 * Creates a formatter element for all the default formatter types.
 */
function addthis_field_formatter_info() {
  $formatters = array();

  foreach (AddThis::getInstance()->getDefaultFormatterTypes() as $key => $label) {
    $formatters[$key] = array(
      'label' => $label,
      'field types' => array(AddThis::FIELD_TYPE),
    );
  }

  return $formatters;
}

/**
 * Implementation to retrieve formatters for a given type of field.
 */
function _addthis_field_info_formatter_field_type($field_type = NULL) {
  $formatters = field_info_formatter_types();
  foreach ($formatters as $key => $formatter) {
    if (!in_array((!isset($field_type) ? AddThis::FIELD_TYPE : $field_type), $formatter['field types'])) {
      unset($formatters[$key]);
    }
  }
  return $formatters;
}

/**
 * Implements hook_field_formatter_view().
 */
function addthis_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  return addthis_render_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display);
}

// There is only one item to display, the share item.
// Get the markup for the display type and add that to the $element[0] to display.
function addthis_render_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {

  $element = array();
  $display_type = $display['type'];

  $options = array(
    '#entity_type' => $entity_type,
    '#entity' => $entity,
    '#display' => $display,
  );

  $markup = AddThis::getInstance()->getDisplayMarkup($display_type, $options);
  if (!isset($element[0])) {
    $element[0] = $markup;
  }
  return $element;
}

/**
 * Implements hook_field_prepare_view().
 *
 * Adds a dummy value in AddThis field for nodes that do not have any value for
 * the field to make rendering possible.
 */
function addthis_field_prepare_view($entity_type, $entities, $field, $instances, $langcode, &$items) {
  foreach ($items as $key => $item) {
    if (!isset($item[0]['value'])) {
      $items[$key][0]['value'] = 'Dummy value';
    }
  }
}

/**
 * Implements hook_field_presave().
 *
 * Adds a dummy value to $items to make rendering possible.
 */
function addthis_field_presave($entity_type, $entity, $field, $instance, $langcode, &$items) {
  $dummy_value = 'Dummy value';

  if (count($items) == 0) {
    $items += array(array('value' => $dummy_value));
  }
  else {
    foreach ($items as $delta => $value) {
      $items[$delta]['value'] = $dummy_value;
    }
  }
}

/**
 * Implements hook_field_widget_info().
 */
function addthis_field_widget_info() {
  return array(
    AddThis::WIDGET_TYPE => array(
      'label' => t('AddThis button'),
      'field types' => array(AddThis::FIELD_TYPE),
    ),
  );
}

/**
 * Implements hook_field_widget_form().
 *
 * The AddThis field is a placeholder field and has no widget settings.
 * To suppress the form, #access is set to FALSE.
 */
function addthis_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  if ($instance['widget']['type'] == AddThis::WIDGET_TYPE) {
    $main_widget = $element + array(
      '#access' => FALSE,
    );
    $element['value'] = $main_widget;
  }

  return $element;
}

/**
 * Implements hook_field_widget_error().
 */
function addthis_field_widget_error($element, $error, $form, &$form_state) {
  switch ($error['error']) {
    default:
      $error_element = $element[$element['#columns'][0]];
      break;
  }

  form_error($error_element, $error['message']);
}
