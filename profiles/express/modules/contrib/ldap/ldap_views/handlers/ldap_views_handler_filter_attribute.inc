<?php

/**
 * @file
 * Basic textfield filter to handle string filtering commands for a generic ldap attribute
 * Includes new criterias
 */
class ldap_views_handler_filter_attribute extends ldap_views_handler_filter {

  /**
   * Determine if a filter can be exposed.
   */
  function can_expose() {
    return TRUE;
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['attribute_name'] = array('default' => '');
    return $options;
  }

  /**
   * This kind of construct makes it relatively easy for a child class
   * to add or remove functionality by overriding this function and
   * adding/removing items from this array.
   */
  function operators() {
    $operators = array(
      'exists' => array(
        'title' => t('Exists'),
        'method' => 'op_exists',
        'short' => t('exists'),
        'values' => 0,
      ),
      'not exists' => array(
        'title' => t('Not exists'),
        'method' => 'op_exists',
        'short' => t('not exists'),
        'values' => 0,
      ),
    );

    return parent::operators() + $operators;
  }
  /**
   * Provide a simple textfield for equality
   */
  function value_form(&$form, &$form_state) {
/*
    $current_display = $this->view->display[$this->view->current_display];
    $qid             = isset($current_display->display_options['query']['options']['qid']) ? $current_display->display_options['query']['options']['qid']
                                                                                           : $current_display->handler->default_display->display->display_options['query']['options']['qid'];
 */
    $ldap_data       = new LdapQuery(ldap_views_get_qid($this->view)); //ldap_data_load($qid);

    if (empty($ldap_data)) {
      $form['attribute_name'] = array(
        '#markup' => 'You must select a valid LDAP search (Advanced::Query settings)',
      );
      return;
    }

    $options         = array();
    foreach ($ldap_data->attributes as $attribute) {
      $options[$attribute] = $attribute;
    }

    if (empty($form_state['exposed'])) {
      $form['attribute_name'] = array(
        '#type' => 'select',
        '#title' => t('Attribute name'),
        '#description' => t('The attribute name from LDAP response'),
        '#options' => $options,
        '#default_value' => $this->options['attribute_name'],
        '#required' => TRUE,
      );
    }

    parent::value_form($form, $form_state);
  }
  function op_equal($field) {
    parent::op_equal($this->options['attribute_name']);
  }

  function op_contains($field) {
    parent::op_contains($this->options['attribute_name']);
  }

  function op_starts($field) {
    parent::op_starts($this->options['attribute_name']);
  }

  function op_not_starts($field) {
    parent::op_not_starts($this->options['attribute_name']);
  }

  function op_ends($field) {
    parent::op_ends($this->options['attribute_name']);
  }

  function op_not_ends($field) {
    parent::op_not_ends($this->options['attribute_name']);
  }

  function op_not($field) {
    parent::op_not($this->options['attribute_name']);
  }

  function op_greater_eq($field) {
    parent::op_greater_eq($this->options['attribute_name']);
  }

  function op_less_eq($field) {
    parent::op_less_eq($this->options['attribute_name']);
  }

  function op_exists($field) {
    parent::op_exists($this->options['attribute_name']);
  }

}
