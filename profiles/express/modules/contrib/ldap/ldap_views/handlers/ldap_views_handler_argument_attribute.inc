<?php

/**
 * @file
 * Basic textfield argument to handle dynamic ldap attributes
 */
class ldap_views_handler_argument_attribute extends views_handler_argument {

  function option_definition() {
    $options = parent::option_definition();
    $options['attribute_name'] = array('default' => '');
    return $options;
  }

  function options_form(&$form, &$form_state) {
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

    parent::options_form($form, $form_state);

    $options         = array();
    foreach ($ldap_data->attributes as $attribute) {
      $options[$attribute] = $attribute;
    }

    $form['attribute_name'] = array(
      '#type' => 'select',
      '#title' => t('Attribute name'),
      '#description' => t('The attribute name from LDAP response'),
      '#options' => $options,
      '#default_value' => $this->options['attribute_name'],
      '#required' => TRUE,
    );

  }

  /**
   * Build the query
   */
  function query() {
    $this->query->add_where(0, $this->options['attribute_name'], $this->argument, '=');
  }
}
