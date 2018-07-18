<?php

/**
 * @file
 * Handler to filter based on locked nodes.
 */

/**
 * Class ContentLockViewsHandlerFilterLocked.
 */
class ContentLockViewsHandlerFilterLocked extends views_handler_filter_boolean_operator {
  /**
   * Query.
   */
  public function query() {
    $this->ensure_my_table();
    if (empty($this->value)) {
      $this->query->add_where($this->options['group'], $this->table_alias . ".timestamp", "NULL", "=");
    }
    else {
      $this->query->add_where($this->options['group'], $this->table_alias . ".timestamp", "NULL", "<>");
    }
  }

}
