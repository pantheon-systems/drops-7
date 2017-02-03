<?php

/**
 * @file
 *   Draggableviews book views handler argument.
 */


/**
 * Argument that refers to a certain book page.
 */
class views_handler_argument_draggableviews_book extends views_handler_argument {
  /**
   * Add condition to select only part of the tree that is under argument's id.
   */
  function query($group_by = FALSE) {
    $this->ensure_my_table();
    $mlid = db_query("SELECT mlid FROM {book} WHERE nid = :nid", array(':nid' => $this->argument))->fetchField();
    // Do not show argument menu item.
    $this->query->add_where(0, $this->table . '.mlid', $mlid, '<>');

    // Select all items that have argument in one of parents.
    $group = $this->query->set_where_group('OR');
    for ($i = 1; $i < 10; $i++) {
      $this->query->add_where($group, $this->table . '.p' . $i, $mlid);
    }

    // We sort items in hook_views_post_execute().
    $tbl = $this->table;

    // Add weight, depth and parent fields.
    $this->query->add_field($tbl, 'weight', 'draggableviews_book_weight');
    $this->query->add_field($tbl, 'depth', 'draggableviews_book_depth');
    $this->query->add_field($tbl, 'plid', 'draggableviews_book_plid');
    $this->query->add_field($tbl, 'mlid', 'draggableviews_book_mlid');
  }
}
