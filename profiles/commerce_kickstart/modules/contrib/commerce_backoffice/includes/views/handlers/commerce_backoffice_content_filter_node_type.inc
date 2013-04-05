<?php

/**
 * @file
 * Definition of commerce_backoffice_content_filter_node_type.
 */

/**
 * Filter by non product display node type.
 *
 * @ingroup views_filter_handlers
 */
class commerce_backoffice_content_filter_node_type extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Content types');
      $types = node_type_get_types();
      $types = array_diff_key($types, commerce_product_reference_node_types());
      $options = array();
      foreach ($types as $type => $info) {
        $options[$type] = t($info->name);
      }
      asort($options);
      $this->value_options = $options;
    }
  }
}
