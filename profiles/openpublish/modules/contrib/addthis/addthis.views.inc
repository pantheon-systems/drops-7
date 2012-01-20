<?php
// $Id: addthis.views.inc,v 1.2 2009/11/13 22:25:54 wesku Exp $
/**
 * @file
 * Provide views data and handlers for addthis.module
 */

/**
 * Implementation of hook_views_data().
 */
function addthis_views_data() {
  // We're registering the 'custom_teasers' table.
  $data['node']['addthis'] = array(
    'title' => t('AddThis'),
    'help' => t('An <a href="http://www.addthis.com/">AddThis</a> button.'),
    'field' => array(
      'handler' => 'addthis_handler_field_service_links',
    ),
  );

  return $data;
}

/**
 * Implementation of hook_views_handlers().
 */
function addthis_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'addthis'),
    ),
    'handlers' => array(
      'addthis_handler_field_service_links' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
}