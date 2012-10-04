<?php
/**
 * @file
 * openpublish_site_page.strongarm.inc
 */

/**
 * Implements hook_strongarm().
 */
function openpublish_site_page_strongarm() {
  $export = array();

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_anonymous_openpublish_site_page';
  $strongarm->value = 0;
  $export['comment_anonymous_openpublish_site_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_default_mode_openpublish_site_page';
  $strongarm->value = 1;
  $export['comment_default_mode_openpublish_site_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_default_per_page_openpublish_site_page';
  $strongarm->value = '50';
  $export['comment_default_per_page_openpublish_site_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_form_location_openpublish_site_page';
  $strongarm->value = 1;
  $export['comment_form_location_openpublish_site_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_openpublish_site_page';
  $strongarm->value = '1';
  $export['comment_openpublish_site_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_preview_openpublish_site_page';
  $strongarm->value = '1';
  $export['comment_preview_openpublish_site_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_subject_field_openpublish_site_page';
  $strongarm->value = 1;
  $export['comment_subject_field_openpublish_site_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_options_openpublish_site_page';
  $strongarm->value = array(
    0 => 'status',
  );
  $export['node_options_openpublish_site_page'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'pathauto_node_openpublish_site_page_pattern';
  $strongarm->value = '[node:title]';
  $export['pathauto_node_openpublish_site_page_pattern'] = $strongarm;

  return $export;
}
