<?php
/**
 * @file
 * context_field_context_condition.inc
 */

/**
 * Expose node views/node forms of specific node types as a context condition.
 */
class context_field_context_condition extends context_condition {
  function condition_values() {
    // needs to return array of nid => titles
    $values = array();
    $values = array(
      1 => 'Yes',
      2 => 'Default'
    );
    return $values;
  }

  function get_context_name($node) {
    return "sf-{$node->nid}";
  }

  function options_form($context) {
    $defaults = $this->fetch_from_context($context, 'options');
    $form = array();

    $form['context_field_category'] = array(
      '#type' => 'textfield',
      '#title' => 'Category',
      '#description' => t('The category of contexts for which you\'d like to utilize this context. For example, a category named blog for blog related contexts, which would differ from the contexts that could be used for basic pages.'),
      '#default_value' => isset($defaults['context_field_category']) ? $defaults['context_field_category'] : '',
    );
    return $form;
  }

  function create_context($context_name) {
    $context = (object) array(
      'name' => $context_name,
      'tag' => 'Context Field',
      'reactions' => array(),
      'conditions' => array('context_field' => array('values' => array(1 => 1))),
    );
    context_save($context);
    return $context;
  }

  function execute($context_name) {
    //If the context exist lets met its context_field condition
    $contexts = $this->get_contexts();
    if ($context = isset($contexts[$context_name]) ? $contexts[$context_name] : FALSE) {
      $this->condition_met($context, 'context_field');
    }
  }
}
