<?php

/**
 * @file
 * Provides some comment attributes as ThemeKey properties.
 *
 * @author Markus Kalkbrenner | Cocomore AG
 *   @see http://drupal.org/user/124705
 *
 * @author profix898
 *   @see http://drupal.org/user/35192
 */


/**
 * Implements hook_themekey_properties().
 *
 * Provides additional properties for the Themekey module:
 * - book:bid
 * - book:has_children
 *
 * @return
 *   array of themekey properties and mapping functions
 */
function themekey_book_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['book:bid'] = array(
    'description' => t('Book: ID - The id of the book (bid). This is the node id (nid) of the top book page. See !link for your books', array('!link' => l(t('!path', array('!path' => 'admin/content/book')), 'admin/content/book'))),
    'validator' => 'themekey_validator_ctype_digit',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );
  $attributes['book:has_children'] = array(
    'description' => t('Book: Has Children - Whether the book has child pages or not. Possible values are "0" for false and "1" for true.'),
    'validator' => 'themekey_validator_nummeric_boolean',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );

  $maps = array();
  $maps[] = array(
    'src' => 'node:nid',
    'dst' => 'book:bid',
    'callback' => 'themekey_book_nid2bid',
  );
  $maps[] = array(
    'src' => 'node:nid',
    'dst' => 'book:has_children',
    'callback' => 'themekey_book_nid2has_children',
  );

  return array('attributes' => $attributes, 'maps' => $maps);
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: node_nid
 * dst: book:bid
 *
 * @param $nid
 *   a node id
 *
 * @return
 *   a book id
 *   or NULL if no value could be mapped
 */
function themekey_book_nid2bid($nid) {
  return themekey_book_get_simple_book_property($nid, 'bid');
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: node_nid
 * dst: book:has_children
 *
 * @param $nid
 *   a node id
 *
 * @return
 *   boolean
 *   or NULL if no value could be mapped
 */
function themekey_book_nid2has_children($nid) {
  return themekey_book_get_simple_book_property($nid, 'has_children');
}


/**
 * Helper function that loads a book and returns the
 * value of a book's property.
 *
 * @param $nid
 *   a node id
 *
 * @param $property
 *   name of a nodes attribute as string
 *
 * @return
 *   the value of the property or NULL
 */
function themekey_book_get_simple_book_property($nid, $property) {
  static $books = array();

  if (!isset($books[$nid])) {
    $nodes = array($nid => new stdClass());

    // node_load() must not be called from hook_init().
    // Therefore we have to execute SQL here using book's hook_node_load().
    book_node_load($nodes, NULL);

    if (isset($nodes[$nid]->book)) {
      $books[$nid] = $nodes[$nid]->book;
    }
    else {
      $books[$nid] = array();
    }
  }

  if (isset($books[$nid][$property])) {
    return $books[$nid][$property];
  }
  else {
    return NULL;
  }
}
