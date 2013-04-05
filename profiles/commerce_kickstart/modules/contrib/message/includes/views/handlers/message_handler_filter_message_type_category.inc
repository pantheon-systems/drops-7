<?php

class message_handler_filter_message_type_category extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Message type category');
      $options = array();
      $options['message_type'] = t('Default message type category');
      foreach (message_type_category_load() as $message_category) {
        $options[$message_category->category] = !empty($message_category->description) ? $message_category->description : $message_type->category;
      }
      $this->value_options = $options;
    }
  }

  function query() {
    // Post-process the value before the query.
    if (empty($this->value)) {
      return;
    }

    $values = array();
    foreach (message_type_load() as $message_type) {
      if (isset($this->value[$message_type->category])) {
        $values[] = $message_type->name;
      }
    }
    $this->value = $values;
    return parent::query();
  }
}
