<?php

/**
 * @file
 * LDAP field handler
 *
 * Defines a new class field handler for a default ldap field
 */
class ldap_views_handler_field_attribute extends ldap_views_handler_field {


  function option_definition() {
    $options = parent::option_definition();
    $options['attribute_name'] = array('default' => '');
    return $options;
  }

  /*
   * Add the field for the LDAP Attribute
   */
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
   * Called to add the field to a query.
   */
  function query() {
    $this->real_field = $this->options['attribute_name'];
    parent::query();
  }
}
