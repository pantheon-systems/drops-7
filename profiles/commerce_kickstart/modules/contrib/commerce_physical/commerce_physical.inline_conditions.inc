<?php

/**
 * @file
 * Provides Inline Conditions integration for the Commerce Physical module.
 */

/**
 * Implements hook_inline_conditions_info().
 */
function commerce_physical_inline_conditions_info() {
  $conditions = array();

  $conditions['commerce_physical_rules_order_weight_comparison'] = array(
    'label' => t('Total order weight'),
    'entity type' => 'commerce_order',
    'callbacks' => array(
      'configure' => 'commerce_physical_order_weight_comparison_configure',
    ),
  );

  return $conditions;
}

 /**
  * Configuration callback for commerce_physical_order_weight_comparison_configure.
  *
  * @param array $settings
  *   An array of rules condition settings.
  *
  * @return array
  *   A form element array.
  */
function commerce_physical_order_weight_comparison_configure($settings) {
  $form = array();

  // Ensure the settings array contains default values.
  $settings += array(
    'operator' => '>=',
    'value' => '',
    'unit' => '',
  );

  $form['operator'] = array(
    '#type' => 'select',
    '#multiple' => FALSE,
    '#options' => commerce_numeric_comparison_operator_options_list(),
    '#title' => t('Operator'),
    '#title_display' => 'invisible',
    '#default_value' => $settings['operator'],
    '#require' => TRUE,
  );

  $form['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Value'),
    '#title_display' => 'invisible',
    '#default_value' => $settings['value'],
    '#size' => 5,
    '#required' => TRUE,
    '#element_validate' => array('element_validate_number'),
  );

  $form['unit'] = array(
    '#type' => 'select',
    '#multiple' => FALSE,
    '#options' => physical_weight_unit_options(),
    '#title' => t('Weight unit'),
    '#title_display' => 'invisible',
    '#default_value' => $settings['unit'],
    '#require' => TRUE,
  );

  return $form;
}
