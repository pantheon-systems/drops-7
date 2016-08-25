<?php

/**
 * @file
 * Handler to identify if node is locked or not.
 */

/**
 * Class ContentLockViewsHandlerSortLocked.
 */
class ContentLockViewsHandlerSortLocked extends views_handler_sort {
  /**
   * Query.
   */
  public function query() {
    $this->ensure_my_table();
    $this->query->add_orderby($this->table_alias, 'timestamp', $this->options['order']);
  }

}
