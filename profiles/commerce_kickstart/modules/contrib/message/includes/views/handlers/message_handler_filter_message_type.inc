<?php
/**
 * Filter by node type
 */
class message_handler_filter_message_type extends views_handler_filter_in_operator {

  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Message type');
      $options = array();
      foreach (message_type_load() as $name => $message_type) {
        $options[$name] = check_plain($message_type->description);
      }
      asort($options);
      $this->value_options = $options;
    }
  }
}
