<?php

/**
 * @file
 * Draggableviews views book handler.
 */

$plugin = array(
  'label' => 'Book',
  'handler' => array(
    'class' => 'draggableviews_handler_book',
  ),
);

class draggableviews_handler_book extends draggableviews_handler {

  /**
   * Retrieve the weight.
   */
  function get($field, $index) {
    $row = $field->view->result[$index];
    return isset($row->draggableviews_book_weight) ? $row->draggableviews_book_weight : 0;
  }

  /**
   *  Set both parent and weight values.
   */
  function set($form_state) {
    $fv = $form_state['values'];

    foreach ($fv['draggableviews'] as $item) {
      $node = node_load($item['id']);

      $keys = array('menu_name', 'mlid', 'router_path', 'has_children', 'options', 'module',
//          'original_bid', 'parent_depth_limit',
          'bid');
      $book = array();
      foreach ($keys as $key) {
        $book[$key] = $node->book[$key];
      }

      $book['weight'] = $item['weight'];
      $book['plid'] = db_query('SELECT mlid FROM {menu_links} WHERE link_path = :link_path AND menu_name = :menu_name', array(':link_path' => 'node/' . $item['parent'], ':menu_name' => $book['menu_name']))->fetchField();

      $node->book = $book;
      _book_update_outline($node);

      drupal_static_reset('book_get_books');
    }
  }
}

