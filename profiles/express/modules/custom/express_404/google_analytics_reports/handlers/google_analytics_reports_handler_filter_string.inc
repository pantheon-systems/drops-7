<?php

/**
 * @file
 * Definition of google_analytics_reports_handler_filter_string.
 */

/**
 * Basic textfield filter to handle string filtering commands.
 *
 * @ingroup views_filter_handlers
 */
class google_analytics_reports_handler_filter_string extends google_analytics_reports_handler_filter {

  function option_definition() {
    $options = parent::option_definition();

    $options['expose']['contains']['required'] = array('default' => FALSE);

    return $options;
  }

  /**
   * This kind of construct makes it relatively easy for a child class
   * to add or remove functionality by overriding this function and
   * adding/removing items from this array.
   */
  function operators() {
    $operators = array(
      '=' => array(
        'title' => t('Is equal to'),
        'short' => t('='),
        'method' => 'op_equal',
        'values' => 1,
      ),
      '!=' => array(
        'title' => t('Is not equal to'),
        'short' => t('!='),
        'method' => 'op_inequal',
        'values' => 1,
      ),
      'contains' => array(
        'title' => t('Contains'),
        'short' => t('contains'),
        'method' => 'op_contains',
        'values' => 1,
      ),
      'not' => array(
        'title' => t('Does not contain'),
        'short' => t('!has'),
        'method' => 'op_not',
        'values' => 1,
      ),
      'regular_expression' => array(
        'title' => t('Contains a match for the regular expression'),
        'short' => t('regex'),
        'method' => 'op_regex',
        'values' => 1,
      ),
      'not_regular_expression' => array(
        'title' => t('Does not match regular expression'),
        'short' => t('!regex'),
        'method' => 'op_not_regex',
        'values' => 1,
      ),
    );

    return $operators;
  }

  function admin_summary() {
    if (!empty($this->options['exposed'])) {
      return t('exposed');
    }

    $options = $this->operator_options('short');
    $output = '';
    if (!empty($options[$this->operator])) {
      $output = check_plain($options[$this->operator]);
    }
    if (in_array($this->operator, $this->operator_values(1))) {
      $output .= ' ' . check_plain($this->value);
    }
    return $output;
  }

  function operator_values($values = 1) {
    $options = array();
    foreach ($this->operators() as $id => $info) {
      if (isset($info['values']) && $info['values'] == $values) {
        $options[] = $id;
      }
    }

    return $options;
  }

  /**
   * Provide a simple textfield for equality.
   */
  function value_form(&$form, &$form_state) {
    // We have to make some choices when creating this as an exposed
    // filter form. For example, if the operator is locked and thus
    // not rendered, we can't render dependencies; instead we only
    // render the form items we need.
    $which = 'all';
    if (!empty($form['operator'])) {
      $source = ($form['operator']['#type'] == 'radios') ? 'radio:options[operator]' : 'edit-options-operator';
    }
    if (!empty($form_state['exposed'])) {
      $identifier = $this->options['expose']['identifier'];

      if (empty($this->options['expose']['use_operator']) || empty($this->options['expose']['operator_id'])) {
        // Exposed and locked.
        $which = in_array($this->operator, $this->operator_values(1)) ? 'value' : 'none';
      }
      else {
        $source = 'edit-' . drupal_html_id($this->options['expose']['operator_id']);
      }
    }

    if ($which == 'all' || $which == 'value') {
      $form['value'] = array(
        '#type' => 'textfield',
        '#title' => t('Value'),
        '#size' => 30,
        '#default_value' => $this->value,
      );
      if (!empty($form_state['exposed']) && !isset($form_state['input'][$identifier])) {
        $form_state['input'][$identifier] = $this->value;
      }

      if ($which == 'all') {
        $form['value'] += array(
          '#dependency' => array($source => $this->operator_values(1)),
        );
      }
    }

    if (!isset($form['value'])) {
      // Ensure there is something in the 'value'.
      $form['value'] = array(
        '#type' => 'value',
        '#value' => NULL,
      );
    }
  }

  function op_equal($field) {
    $this->query->add_where($this->options['group'], $field, $this->value, '==');
  }

  function op_inequal($field) {
    $this->query->add_where($this->options['group'], $field, $this->value, '!=');
  }

  function op_contains($field) {
    $this->query->add_where($this->options['group'], $field, $this->value, '=@');
  }

  function op_not($field) {
    $this->query->add_where($this->options['group'], $field, $this->value, '!@');
  }

  function op_regex($field) {
    $this->query->add_where($this->options['group'], $field, $this->value, '=~');
  }

  function op_not_regex($field) {
    $this->query->add_where($this->options['group'], $field, $this->value, '!~');
  }

}
