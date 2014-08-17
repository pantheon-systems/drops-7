<?php

/**
 * @file
 * Defines field implementations.
 */

/**
 * Implements hook_field_formatter_info().
 */
function addthis_displays_field_formatter_info() {
  $formatters = array();

  $formatters['addthis_basic_toolbox'] = array(
    'label' => t('Basic Toolbox'),
    'field types' => array(AddThis::FIELD_TYPE),
    'settings' => array(
      'share_services' => 'facebook,twitter',
      'buttons_size' => 'addthis_16x16_style',
      'counter_orientation' => 'horizontal',
      'extra_css' => '',
    ),
  );
  $formatters['addthis_basic_button'] = array(
    'label' => t('Basic Button'),
    'field types' => array(AddThis::FIELD_TYPE),
    'settings' => array(
      'button_size' => 'small',
      'extra_css' => '',
    ),
  );
  return $formatters;
}

/**
 * Implementss hook_field_formatter_settings_form().
 */
function addthis_displays_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $element = array();

  switch ($display['type']) {
    case 'addthis_basic_toolbox':
      $element['share_services'] = array(
        '#title' => t('Services'),
        '#type' => 'textfield',
        '#size' => 80,
        '#default_value' => $settings['share_services'],
        '#required' => TRUE,
        '#element_validate' => array('_addthis_display_element_validate_services'),
        '#description' => t('Specify the names of the sharing services and seperate them with a , (comma). <a href="http://www.addthis.com/services/list">The names on this list are valid.</a>'),
      );
      $element['buttons_size'] = array(
        '#title' => t('Buttons size'),
        '#type' => 'select',
        '#default_value' => $settings['buttons_size'],
        '#options' => array(
          AddThis::CSS_16x16 => t('Small (16x16)'),
          AddThis::CSS_32x32 => t('Big (32x32)'),
        ),
      );
      $element['counter_orientation'] = array(
        '#title' => t('Counter orientation'),
        '#description' => t('Specify the way service counters are oriented.'),
        '#type' => 'select',
        '#default_value' => $settings['counter_orientation'],
        '#options' => array(
          'horizontal' => t('Horizontal'),
          'vertical' => t('Vertical'),
        )
      );
      $element['extra_css'] = array(
        '#title' => t('Extra CSS declaration'),
        '#type' => 'textfield',
        '#size' => 40,
        '#default_value' => $settings['extra_css'],
        '#description' => t('Specify extra CSS classes to apply to the toolbox'),
      );
      break;
    case 'addthis_basic_button':
      $element['button_size'] = array(
        '#title' => t('Image'),
        '#type' => 'select',
        '#default_value' => $settings['button_size'],
        '#options' => array(
          'small' => t('Small'),
          'big' => t('Big'),
        ),
      );
      $element['extra_css'] = array(
        '#title' => t('Extra CSS declaration'),
        '#type' => 'textfield',
        '#size' => 40,
        '#default_value' => $settings['extra_css'],
        '#description' => t('Specify extra CSS classes to apply to the button'),
      );
      break;
  }

  return $element;
}

/**
 * Validate the services text field to validate if services are correct.
 */
function _addthis_display_element_validate_services($element, &$form_state, $form) {
  $bad = FALSE;

  $services = trim($element['#value']);
  $services = str_replace(' ', '', $services);

  if (!preg_match('/^[a-z\_\,]+$/', $services)) {
    $bad = TRUE;
  }
  // @todo Validate the service names against AddThis.com. Give a notice when there are bad names.

  // Return error.
  if ($bad) {
    form_error($element, t('The declared services are incorrect or nonexistent.'));
  }
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function addthis_displays_field_formatter_settings_summary($field, $instance, $view_mode) {
  $view = $instance['display'][$view_mode];
  $info = '';
  switch ($view['type']) {
    case 'addthis_basic_toolbox':
      $settings = $view['settings'];

      $services = trim($settings['share_services']);
      $services = str_replace(' ', '', $services);
      $services = '<b>' . implode(', ', explode(',', $services)) . '</b>';

      $info =  t('Toolbox with the following services: !services', array('!services' => $services));
      break;

    case 'addthis_basic_button':
      $settings = $view['settings'];

      $size = '<b>' . $settings['button_size'] . '</b>';
      $info = t('Basic button size: !size', array('!size' => $size));
      break;
  }

  return $info;
}

/**
 * Implements hook_field_formatter_view().
 */
function addthis_displays_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  // We use the helper function addthis_render_formatter_view with literally all the arguments.
  return addthis_render_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display);
}
