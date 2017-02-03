<?php

/**
 * @file
 * Draggableviews book views hierarchy handler.
 */

$plugin = array(
  'label' => 'Book',
  'handler' => array(
    'class' => 'draggableviews_hierarchy_handler_book',
  ),
);

class draggableviews_hierarchy_handler_book extends draggableviews_hierarchy_handler {
  public function get($field, $index) {
    $row = $field->view->result[$index];
    $parent_mlid = $row->draggableviews_book_plid;
    
    $parent_link_path = db_query('SELECT link_path FROM {menu_links} WHERE mlid = :mlid', array(':mlid' => $parent_mlid))->fetchField();

    return !empty($parent_link_path) ? substr($parent_link_path, 5) : 0;
  }

  public function get_depth($field, $index) {
    $row = $field->view->result[$index];

    // Cache depth of the top parent so we do not recalculate it.
    static $parent_depth;
    if (is_null($parent_depth)) {
      $parent_mlid = $row->draggableviews_book_plid;
      $parent_depth = db_query('SELECT depth FROM {menu_links} WHERE mlid = :mlid', array(':mlid' => $parent_mlid))->fetchField() + 1;
    }

    return isset($row->draggableviews_book_depth) ? $row->draggableviews_book_depth - $parent_depth : 0;
  }

  // Don't need to set value here as it is done in "weight" handler
  // draggableviews_handler in order to avoid doing multiple identical queries
  // to draggableviews_structure table.
  function set($form_state) {}
}

