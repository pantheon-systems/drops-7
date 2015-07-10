<?php

/**
 * @file
 * Order relationship to a representative line item.
 */

class commerce_shipping_handler_relationship_shipping_line_item_representative extends views_handler_relationship {
  function label() {
    if (!isset($this->options['label'])) {
      return $this->ui_name();
    }
    return $this->options['label'];
  }

  function query() {
    // Get the JOIN type from the relationship settings.
    $join_type = empty($this->options['required']) ? 'LEFT' : 'INNER';

    // This function populates $this->table_alias with the proper query alias to
    // the order table.
    $this->ensure_my_table();

    // Note: while the left_query was originally constructed using a db_select()
    // object, it turned out to be simpler to just encode the query directly.
    $definition = array(
      'table' => 'commerce_line_item',
      'field' => 'line_item_id',
      'left_table' => $this->table_alias,
      'left_field' => 'order_id',
      'left_query' => "SELECT MIN(subcli.line_item_id) as min_line_item_id FROM field_data_commerce_line_items subfdcli LEFT JOIN commerce_line_item subcli ON subfdcli.commerce_line_items_line_item_id = subcli.line_item_id WHERE subfdcli.entity_id = " . $this->table_alias . ".order_id AND subcli.type = 'shipping' ORDER BY subcli.line_item_id",
      'type' => $join_type,
    );

    $join = new views_join_subquery();
    $join->definition = $definition;
    $join->construct();
    $join->adjusted = TRUE;

    $alias = 'commerce_order_shipping_line_item_representiative';
    $this->alias = $this->query->add_relationship($alias, $join, 'commerce_line_item', $this->relationship);
  }
}
