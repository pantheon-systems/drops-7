<?php

/**
 * @file
 * Native handler plugin.
 */

$plugin = array(
  'label' => 'Native',
  'handler' => array(
    'class' => 'draggableviews_hierarchy_handler_native',
  ),
);

class draggableviews_hierarchy_handler_native extends draggableviews_hierarchy_handler {
  public function get($field, $index) {
    $row = $field->view->result[$index];
    return (isset($row->draggableviews_structure_parent)) ? $row->draggableviews_structure_parent : 0;
  }

  public function get_depth($field, $index) {
    // $index can be NULL for multiple reasons. Common problem
    // is after parent is deleted.
    if (!isset($index)) {
      return;
    }
    $row = $field->view->result[$index];
    // If parent is available, set parent's depth +1.
    return (!empty($row->draggableviews_structure_parent)) ? $this->get_depth($field, $this->get_index($field->view, $row->draggableviews_structure_parent)) + 1 : 0;
  }

  // Don't need to set value here as it is done in "weight" handler
  // draggableviews_handler in order to avoid doing multiple identical queries
  // to draggableviews_structure table.
  function set($form_state) {}
}
