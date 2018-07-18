<?php

/**
 * @file
 * Definition of google_analytics_reports_handler_filter.
 */

/**
 * Provides base filter functionality for Google Analytics fields.
 *
 * @ingroup views_filter_handlers
 */
class google_analytics_reports_handler_filter extends views_handler_filter {

  /**
   * {@inheritdoc}
   */
  var $always_multiple = TRUE;

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
    parent::options_form($form, $form_state);

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
  }

  /**
   * {@inheritdoc}
   */
  function operator_options($which = 'title') {
    $options = array();
    foreach ($this->operators() as $id => $info) {
      $options[$id] = $info[$which];
    }

    return $options;
  }

  /**
   * {@inheritdoc}
   */
  function query() {
    if ($this->is_custom) {
      $this->real_field = google_analytics_reports_custom_to_variable_field($this->real_field, $this->options['custom_field_number']);
    }

    $info = $this->operators();
    if (!empty($info[$this->operator]['method'])) {
      $this->{$info[$this->operator]['method']}($this->real_field);
    }
  }

}
