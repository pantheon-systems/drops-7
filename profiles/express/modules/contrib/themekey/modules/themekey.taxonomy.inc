<?php

/**
 * @file
 * Provides some taxonomy stuff as ThemeKey properties.
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
 * - taxonomy:vid
 * - taxonomy:tid
 * - taxonomy:tid_and_childs
 *
 * @return
 *   array of themekey properties and mapping functions
 */
function themekey_taxonomy_themekey_properties() {
  // Attributes for properties ;)
  $attributes = array();
  $attributes['taxonomy:vid'] = array(
    'description' => t('Taxonomy: Vocabulary - The vocabulary id (vid) of a taxonomy vocabulary. See !link for your vocabularies.',
      array('!link' => l(t('!path', array('!path' => 'admin/structure/taxonomy')), 'admin/structure/taxonomy'))),
    'validator' => 'themekey_validator_ctype_digit',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );
  $attributes['taxonomy:tid'] = array(
    'description' => t('Taxonomy: Term - The term id (tid) of a taxonomy term.'),
    'validator' => 'themekey_validator_ctype_digit',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );
  $attributes['taxonomy:tid_and_childs'] = array(
    'description' => t('Taxonomy: Term and its childs - The term id (tid) of a taxonomy term. If set, all child terms of this term will be used too.'),
    'validator' => 'themekey_validator_ctype_digit',
    'page cache' => THEMEKEY_PAGECACHE_UNSUPPORTED,
  );

  // Mapping functions
  $maps = array();
  $maps[] = array(
    'src' => 'taxonomy:tid',
    'dst' => 'taxonomy:vid',
    'callback' => 'themekey_taxonomy_tid2vid',
  );
  $maps[] = array(
    'src' => 'taxonomy:tid',
    'dst' => 'taxonomy:tid_and_childs',
    'callback' => 'themekey_taxonomy_tid2tid_and_parents',
  );
  $maps[] = array(
    'src' => 'node:nid',
    'dst' => 'taxonomy:tid',
    'callback' => 'themekey_taxonomy_nid2tid',
  );

  return array('attributes' => $attributes, 'maps' => $maps);
}


/**
 * Implements hook_themekey_paths().
 */
function themekey_taxonomy_themekey_paths() {
  $paths = array();
  $paths[] = array('path' => 'taxonomy/term/#taxonomy:tid');

  // Add support for 'forum' paths
  if (module_exists('forum') && variable_get('themekey_module_forum_triggers_taxonomy_vid', 0)) {
    $paths[] = array('path' => 'forum/#taxonomy:vid');
  }

  return $paths;
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: taxonomy:tid
 * dst: taxonomy:vid
 *
 * @param $tids
 *   array of taxonomy term ids or a single term id
 *
 * @return
 *   array of taxonomy vocabulary ids
 *   or NULL if no value could be mapped
 */
function themekey_taxonomy_tid2vid($tids) {
  $vid = array();
  // Use SQL instead taxonomy API because this code runs during hook_init() stage.
  // Using taxonomy API will require to load the node using node_load() which is not allowed in this stage.
  $tids = is_array($tids) ? $tids : array($tids);
  foreach ($tids as $tid) {
    $vid[] = db_select('taxonomy_term_data', 't')
    ->fields('t', array('vid'))
    ->condition('tid', $tid)
    ->execute()
    ->fetchField();
  }

  return count($vid) ? $vid : NULL;
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: node:nid
 * dst: taxonomy:tid
 *
 * @param $nid
 *   node id
 *
 * @return
 *   array of taxonomy term ids
 *   or NULL if no value could be mapped
 */
function themekey_taxonomy_nid2tid($nid) {
  $tids = array();

  $query = db_select('taxonomy_index', 't');
  $query->addField('t', 'tid');
  $query->condition('t.nid', $nid);
  $result = $query->execute();

  if ($result->rowCount()) {
    foreach ($result as $item) {
      $tids[] = $item->tid;
    }
  }

  return count($tids) ? $tids : NULL;
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: taxonomy:tid
 * dst: taxonomy:tid_and_parents
 *
 * @param $tids
 *   array of taxonomy term ids or a single term id
 *
 * @return
 *   array of taxonomy term ids
 *   or NULL if no value could be mapped
 */
function themekey_taxonomy_tid2tid_and_parents($tids) {
  $tids = is_array($tids) ? $tids : array($tids);
  foreach ($tids as $tid) {
    // note that taxonomy_get_parents_all() returns the term itself
    $parent_terms = taxonomy_get_parents_all($tid);
    $parents = array();
    foreach ($parent_terms as $parent_term) {
      $parents[] = $parent_term->tid;
    }
  }

  return count($parents) ? array_unique($parents) : NULL;
}

