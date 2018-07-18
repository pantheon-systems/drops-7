<?php

/**
 * @file
 * Drush integration for the Metatag Importer module.
 */

/**
 * Implements hook_drush_command().
 */
function metatag_importer_drush_command() {
  $items['metatag-convert-metatags-quick'] = array(
    'description' => dt('Convert data from Metatags Quick into Metatag'),
    'drupal dependencies' => array('metatag', 'metatag_importer'),
    'aliases' => array('mtcmq'),
  );

  $items['metatag-convert-nodewords'] = array(
    'description' => dt('Convert data from Nodewords into Metatag.'),
    'drupal dependencies' => array('metatag', 'metatag_importer'),
    'aliases' => array('mtcnw'),
  );

  $items['metatag-convert-page-title'] = array(
    'description' => dt('Convert data from Page Title into Metatag.'),
    'drupal dependencies' => array('metatag', 'metatag_importer'),
    'aliases' => array('mtcpt'),
  );

  return $items;
}

/**
 * Callback to convert all Metatags Quick data.
 */
function drush_metatag_importer_metatag_convert_metatags_quick() {
  if (!drush_confirm(dt('Ready to convert all data from Metatags Quick?'))) {
    return;
  }

  include 'metatag_importer.metatags_quick.inc';
  metatag_importer_metatags_quick_import();
}

/**
 * Callback to convert all Nodewords data.
 */
function drush_metatag_importer_metatag_convert_nodewords() {
  if (!drush_confirm(dt('Ready to convert all data from Nodewords?'))) {
    return;
  }

  // Need to make sure the Nodewords table actually exists.
  if (!db_table_exists('nodewords')) {
    drush_set_error('metatag_importer', dt('Could not find the nodewords table!'));
    return;
  }

  // Offload all of the logic to the code contained in the admin file.
  include 'metatag_importer.nodewords.inc';

  // Start the import.
  $types = array_keys(_metatag_importer_list_nodewords());
  _metatag_importer_import($types);

  drush_print(dt('Data converesion finished.'));
}

/**
 * Callback to convert Page Title data.
 */
function drush_metatag_importer_metatag_convert_page_title() {
  if (!db_table_exists('page_title')) {
    drush_set_error('metatag_importer', dt('Could not find the page_title table!'));
    return;
  }

  $records = db_query("SELECT COUNT(id) FROM {page_title} WHERE type IN ('node', 'taxonomy_term', 'user')")->fetchField();

  if (empty($records)) {
    return dt('There are no page_title records to convert!');
  }

  if (!drush_confirm(dt('Ready to convert !count record(s) from Page Title?', array('!count' => $records)))) {
    return;
  }

  include 'metatag_importer.page_title.inc';

  // Start the importer.
  $count = metatag_importer_for_page_title();

  drush_print(dt('Converted !count record(s) from the Page Title module.', array('!count' => $count)));
}
