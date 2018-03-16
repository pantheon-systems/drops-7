<?php
/**
 * @file
 * Provide views handler arguments for fieldable panel panes
 */

/**
 * Argument handler to accept a node type.
 */
class fieldable_panels_panes_handler_argument_bundle extends views_handler_argument {
  function summary_name($data) {
    return fieldable_panels_panes_get_bundle_label($data->{$this->name_alias});
  }

  function title() {
    return fieldable_panels_panes_get_bundle_label($this->argument);
  }

  function set_argument($arg) {
    // Replace all - in the URL with _ since - is not valid.
    $this->argument = str_replace('-', '_', $arg);
    return $this->validate_arg($arg);
  }

  function summary_argument($data) {
    // Transform all _ into - so that URLs are proper.
    return str_replace('_', '-', $data->{$this->base_alias});
  }
}
