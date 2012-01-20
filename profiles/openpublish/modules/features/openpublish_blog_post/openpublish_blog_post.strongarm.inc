<?php
/**
 * @file
 * openpublish_blog_post.strongarm.inc
 */

/**
 * Implements hook_strongarm().
 */
function openpublish_blog_post_strongarm() {
  $export = array();

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_anonymous_openpublish_blog_post';
  $strongarm->value = 0;
  $export['comment_anonymous_openpublish_blog_post'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_default_mode_openpublish_blog_post';
  $strongarm->value = 1;
  $export['comment_default_mode_openpublish_blog_post'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_default_per_page_openpublish_blog_post';
  $strongarm->value = '50';
  $export['comment_default_per_page_openpublish_blog_post'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_form_location_openpublish_blog_post';
  $strongarm->value = 1;
  $export['comment_form_location_openpublish_blog_post'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_openpublish_blog_post';
  $strongarm->value = '2';
  $export['comment_openpublish_blog_post'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_preview_openpublish_blog_post';
  $strongarm->value = '1';
  $export['comment_preview_openpublish_blog_post'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_subject_field_openpublish_blog_post';
  $strongarm->value = 1;
  $export['comment_subject_field_openpublish_blog_post'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_options_openpublish_blog_post';
  $strongarm->value = array(
    0 => 'status',
  );
  $export['node_options_openpublish_blog_post'] = $strongarm;

  return $export;
}
