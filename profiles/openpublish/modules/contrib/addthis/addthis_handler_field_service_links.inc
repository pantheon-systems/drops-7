<?php
// $Id: addthis_handler_field_service_links.inc,v 1.2 2009/11/13 22:25:54 wesku Exp $

/**
 * A handler to provide an AddThis field for a particular node.
 *
 * @ingroup views_field_handlers
 */
class addthis_handler_field_service_links extends views_handler_field {
  function construct() {
    parent::construct();
    $this->additional_fields['nid'] = 'nid';
    $this->additional_fields['title'] = 'title';
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function option_definition() {
    $options = parent::option_definition();

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // Remove the alteration stuff
    unset($form['alter']);
  }

  function render($values) {
    if (user_access('view addthis')) {
      $node = new stdClass();
      $node->nid = $values->{$this->aliases['nid']};
      $node->title = $values->{$this->aliases['title']};
      return _addthis_create_button($node, TRUE);
    }
  }
}