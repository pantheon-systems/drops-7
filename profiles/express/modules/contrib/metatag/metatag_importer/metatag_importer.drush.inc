<?php
/**
 * @file
 * Drush integration for the Metatag Importer module.
 */

/**
 * Implements hook_drush_command().
 */
function metatag_importer_drush_command() {
  // This needs to be fixed.
  // $items['metatag-convert-nodewords'] = array(
  //   'description' => dt('Convert data from Nodewords into Metatag.'),
  //   'drupal dependencies' => array('metatag'),
  // );

  $items['metatag-convert-page-title'] = array(
    'description' => dt('Convert data from Page Title into Metatag.'),
    'drupal dependencies' => array('metatag'),
  );
  
  return $items;
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
    drush_set_error('metatag_importer', dt('Could not find the nodewords table.'));
    return;
  }

  // Offload all of the logic to the code contained in the admin file.
  include('metatag_importer.admin.inc');

  // Start the import.
  // @todo This isn't working.
  _metatag_importer_import();

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

  include('metatag_importer.page_title.inc');

  // Start the importer.
  $count = metatag_importer_for_page_title();

  drush_print(dt('Converted !count record(s) from the Page Title module.', array('!count' => $count)));
}
