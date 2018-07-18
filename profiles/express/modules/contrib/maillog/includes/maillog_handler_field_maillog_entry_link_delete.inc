<?php 

/**
 * @file
 * Handler for providing an 'Delete link' in views
 */

class maillog_handler_field_maillog_entry_link_delete extends views_handler_field {
  function construct() {
    parent::construct();
    $this->additional_fields['maillog_id'] = 'maillog_id';
  }

  /**
   * Called to add the field to a query.
   */
  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    // ensure user has access to edit this node.
    if (!user_access('delete maillog')) {
      return;
    }
    $text = !empty($this->options['text']) ? $this->options['text'] : t('delete');
    return l($text, "maillog/delete/$values->maillog_id", array('query' => drupal_get_destination()));
  }
}

