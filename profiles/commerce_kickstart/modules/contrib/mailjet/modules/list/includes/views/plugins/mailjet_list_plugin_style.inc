<?php

/**
 * @file
 *
 * Plugin handler for MailJet List.
 */
class views_plugin_mailjet_list extends views_plugin_style {

  function option_definition() {
    $options = array();
    $options['mailjet_list_values'] = array('default' => array(), 'translatable' => FALSE);

    return $options;
  }

  function options_form(&$form, &$form_state) {

    foreach ($this->display->handler->get_handlers('field') as $field => $handler) {
      if ($label = $handler->label()) {
        $fields[$field] = $label;
      }
      else {
        $fields[$field] = $handler->ui_name();
      }
    }

    // Get all the available fields in the current display.
    $field_names = $this->display->handler->get_field_labels();

    $form['email'] = array(
      '#type' => 'select',
      '#title' => t('E-mail'),
      '#description' => t('The e-mail address used for syncronisation.'),
      '#options' => $field_names,
      '#required' => TRUE,
      '#default_value' => $this->options['email'],
    );
  }

}
