<?php

class commerce_message_handler_area_add_message extends views_handler_area {

  function option_definition() {
    $options = parent::option_definition();
    $options['valid_types'] = array('default' => array());
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $options = array();
    foreach (message_type_load() as $name => $message_type) {
      $options[$name] = check_plain($message_type->description);
    }
    $form['valid_types'] = array(
      '#type' => 'select',
      '#title' => t('Valid types'),
      '#options' => $options,
      '#default_value' => $this->options['valid_types'],
      '#multiple' => TRUE,
    );
  }

  public function render($empty = FALSE) {
    $order_argument = $this->view->argument['message_commerce_order_target_id'];
    if (!isset($order_argument->value[0])) {
      return;
    }

    $message = entity_create('message', array('type' => 'commerce_order_user_comment'));
    $message->message_commerce_order[LANGUAGE_NONE][0]['target_id'] = $order_argument->value[0];
    $commerce_message_message_form = drupal_get_form('commerce_message_message_form', $message, $this->options['valid_types'], $this->view->current_display);
    return drupal_render($commerce_message_message_form);
  }
}
