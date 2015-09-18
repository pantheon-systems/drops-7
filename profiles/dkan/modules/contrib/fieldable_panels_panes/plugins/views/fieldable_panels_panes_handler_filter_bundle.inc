<?php
/**
 * @file
 * Provide views handlers for fieldable panel panes
 */

/**
 * Filter by node type
 */
class fieldable_panels_panes_handler_filter_bundle extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Bundles');
      $this->value_options = fieldable_panels_panes_get_bundle_labels();
    }
  }
}
