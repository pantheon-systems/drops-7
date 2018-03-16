<?php

/**
 * @file
 * Provide special views data and handlers for draggableviews_book module
 */

/**
 * Implements hook_views_data().
 */
function draggableviews_book_views_data() {
  // Book hierarchy and weight data are now in {menu_links}.
  $data['draggableviews_book_structure']['table']['group'] = t('Book');
  $data['draggableviews_book_structure']['table']['join'] = array(
    'node' => array(
      'table' => 'menu_links',
      'left_table' => 'book',
      'left_field' => 'mlid',
      'field' => 'mlid',
    ),
  );

  $data['draggableviews_book_structure']['book'] = array(
    'title' => t('All sub nodes of this book page.'),
    'help' => t('All sub nodes of this book page.'),
    'argument' => array(
      'handler' => 'views_handler_argument_draggableviews_book',
    ),
  );

  return $data;
}
