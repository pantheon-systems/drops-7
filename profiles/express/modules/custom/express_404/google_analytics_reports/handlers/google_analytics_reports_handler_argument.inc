<?php

/**
 * @file
 * Definition of google_analytics_reports_handler_argument.
 */

/**
 * Provides base argument functionality for Google Analytics fields.
 *
 * @ingroup views_argument_handlers
 */
class google_analytics_reports_handler_argument extends views_handler_argument {

  /**
   * @var bool
   * Stores whether this is a custom field or not.
   */
  var $is_custom = NULL;

  /**
   * {@inheritdoc}
   */
  function init(&$view, &$options) {
    parent::init($view, $options);

    $this->is_custom = google_analytics_reports_is_custom($this->real_field);
  }

  /**
   * {@inheritdoc}
   */
  function query($group_by = FALSE) {
    if ($this->is_custom) {
      $this->real_field = google_analytics_reports_custom_to_variable_field($this->real_field, $this->options['custom_field_number']);
    }

    $this->operator = '==';
    $this->query->add_where(1, $this->real_field, $this->argument, $this->operator);
  }

  /**
   * {@inheritdoc}
   */
  function option_definition() {
    $options = parent::option_definition();

    if ($this->is_custom) {
      $options['custom_field_number'] = array('default' => 1);
    }

    return $options;
  }

  /**
   * {@inheritdoc}
   */
  function options_form(&$form, &$form_state) {
    if ($this->is_custom) {
      $form['custom_field_number'] = array(
        '#type' => 'textfield',
        '#title' => t('Custom field number'),
        '#default_value' => isset($this->options['custom_field_number']) ? $this->options['custom_field_number'] : 1,
        '#size' => 2,
        '#maxlength' => 2,
        '#required' => TRUE,
        '#element_validate' => array('element_validate_integer_positive'),
      );
    }

    parent::options_form($form, $form_state);
  }

}
