<?php

/**
 * @file
 * Ubercart Catalog implementations of the page title hooks
 */


/**
 * Ubercart Catalog pages are on both:
 * /catalog
 * /catalog/[tid]
 *
 * This means we need to make sure arg(1) in the menu item is a valid term.
 */

/**
 * Implements hook_page_title_alter().
 */
function uc_catalog_page_title_alter(&$title) {
  $menu_item = menu_get_item();

  // If we're looking at a UC Catalog term page, get the term title
  if ( !strncmp($menu_item['path'], 'catalog', 7) &&
       isset($menu_item['page_arguments'][0]) &&
       ($term = taxonomy_term_load($menu_item['page_arguments'][0])) &&
       variable_get('page_title_vocab_' . $term->vocabulary_machine_name . '_showfield', 0) &&
       ($term_title = page_title_load_title($term->tid, 'term')) ) {
    $title = $term_title;
  }
}


/**
 * Implements hook_page_title_pattern_alter().
 */
function uc_catalog_page_title_pattern_alter(&$pattern, &$types) {
  $menu_item = menu_get_item();

  // Ubercart Catalog Term Page
  if ( !strncmp($menu_item['path'], 'catalog', 7) &&
       isset($menu_item['page_arguments'][0]) &&
       ($term = taxonomy_term_load($menu_item['page_arguments'][0])) ) {
    $types['taxonomy'] = $term;
    $pattern = variable_get('page_title_uc_catalog_' . $types['taxonomy']->vid, '');
  }
}


/**
 * Implements hook_page_title_settings().
 */
function uc_catalog_page_title_settings() {
  $settings = array();

  $vocabs = taxonomy_get_vocabularies();
  foreach ($vocabs as $vocab) {
    $settings['page_title_uc_catalog_' . $vocab->vid] = array(
      'label' => 'Ubercart Catalog - %vocab_name',
      'label arguments' => array('%vocab_name' => $vocab->name),
      'scopes' => array('global', 'taxonomy'),
      'show field' => FALSE,
      'description' => 'This pattern will be used for all %vocab_name Ubercart catalog pages.<br />' .
                       'The Show Field setting does not apply here. Use the matching Vocabulary row.',
      'description arguments' => array('%vocab_name' => $vocab->name),
    );
  }

  return $settings;
}
