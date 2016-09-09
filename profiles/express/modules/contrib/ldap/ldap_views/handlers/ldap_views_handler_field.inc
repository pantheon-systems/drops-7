<?php

/**
 * @file
 * LDAP field handler
 *
 * Defines a new class field handler for a default ldap field
 */
class ldap_views_handler_field extends views_handler_field {

  function render($values) {
    return array_key_exists($this->field_alias, $values) ? check_plain($values[$this->field_alias]) : '';
  }

  function element_type($none_supported = FALSE, $default_empty = FALSE, $inline = FALSE) {
    if (isset($this->definition['element type'])) {
      return $this->definition['element type'];
    }

    return 'div';
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['multivalue']      = array('default' => 'v-all');
    $options['value_separator'] = array('default' => '');
    $options['index_value']     = array('default' => 0);
    return $options;
  }

  /*
   * Add the field for the LDAP Attribute
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['multivalue'] = array(
      '#type' => 'select',     // it should be 'radios', but it makes #dependency not to work
      '#title' => t('Values to show'),
      '#description' => t('What to do whith  multivalue attributes'),
      '#options' => array(
        'v-all' => t('All values'),
        'v-index' => t('Show Nth value'),
        'v-count' => t('Count values'),
      ),
      '#default_value' => $this->options['multivalue'],
      '#required' => TRUE,
    );
    $form['value_separator'] = array(
      '#type' => 'textfield',
      '#title' => t('Value separator'),
      '#description' => t('Separator to use between values in multivalued attributes'),
      '#default_value' => $this->options['value_separator'],
      '#dependency' => array(
        'edit-options-multivalue' => array('v-all'),
      ),
    );
    $form['index_value'] = array(
      '#type' => 'textfield',
      '#title' => t('Index'),
      '#description' => t('Index of the value to show. Use negative numbers to index from last item (0=First, -1=Last)'),
      '#default_value' => $this->options['index_value'],
      '#dependency' => array(
        'edit-options-multivalue' => array('v-index'),
      ),
    );
  }
}
