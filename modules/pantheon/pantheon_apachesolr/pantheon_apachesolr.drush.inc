<?php

/**
 * @file
 * Drush commands for Pantheon Apachesolr.
 */

/**
 * Implements hook_drush_command().
 */
function pantheon_apachesolr_drush_command() {
  $items = array();

  $items['pantheon-apachesolr-schema-post'] = array(
    'description' => 'Posts a schema file to the Solr server',
    'examples' => array(
      'drush pantheon-apachesolr-schema-post sites/all/modules/contrib/apachesolr/solr-conf/solr-3.x/schema.xml' => dt('Posts the solr-3.x schema'),
    ),
    'arguments' => array(
      'file' => dt('The path to the schema file, relative to the Drupal root'),
    ),
  );

  return $items;
}

/**
 * Post a schema file.
 */
function drush_pantheon_apachesolr_schema_post($file) {

  $vars = array('!file' => $file);
  if (filesize($file) < 1) {
    drush_log(dt("The file !file is empty or does not exist", $vars), 'error');
  }
  else {
    $success = pantheon_apachesolr_post_schema_exec($file);
    if ($success) {
      drush_log(dt("The file !file posted successfully", $vars), 'ok');
    }
    else {
      drush_log(dt("The file !file failed to post successfully", $vars), 'error');
    }
  }
}
