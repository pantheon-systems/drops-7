<?php

/**
 * @file
 * Base sort handler for a ldap attributes
 */
class ldap_views_handler_sort_attribute extends ldap_views_handler_sort {
  function option_definition() {
    $options = parent::option_definition();
    $options['attribute_name'] = array('default' => '');
    return $options;
  }

  function options_form(&$form, &$form_state) {
/*
    $current_display = $this->view->display[$this->view->current_display];
    $did             = isset($current_display->display_options['query']['options']['did']) ? $current_display->display_options['query']['options']['did']
                                                                                           : $current_display->handler->default_display->display->display_options['query']['options']['did'];
 */
    $ldap_data       = new LdapQuery(ldap_views_get_qid($this->view)); //ldap_data_load($did);
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
   * Called to add the sort to a query.
   */
  function query() {
    $this->query->add_orderby($this->table, $this->options['attribute_name'], $this->options['order']);
  }
}
