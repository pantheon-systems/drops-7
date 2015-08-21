<?php
/**
 * @file
 * commerce_kickstart_overrides_test.features.inc
 */

/**
 * Implements hook_field_default_fields_alter().
 */
function commerce_kickstart_overrides_test_field_default_fields_alter(&$data) {
  if (isset($data['node-blog_post-body'])) {
    $data['node-blog_post-body']['field_instance']['label'] = 'Article body'; /* WAS: 'Description' */
    $data['node-blog_post-body']['field_instance']['required'] = 1; /* WAS: 0 */
  }
  if (isset($data['node-blog_post-field_blog_category'])) {
    $data['node-blog_post-field_blog_category']['field_instance']['display']['default']['type'] = 'hidden'; /* WAS: 'taxonomy_term_reference_link' */
    $data['node-blog_post-field_blog_category']['field_instance']['display']['teaser']['type'] = 'hidden'; /* WAS: 'taxonomy_term_reference_link' */
    unset($data['node-blog_post-field_blog_category']['field_instance']['display']['default']['module']);
    unset($data['node-blog_post-field_blog_category']['field_instance']['display']['teaser']['module']);
  }
  if (isset($data['node-blog_post-field_image'])) {
    $data['node-blog_post-field_image']['field_instance']['fences_wrapper'] = ''; /* WAS: '' */
    $data['node-blog_post-field_image']['field_instance']['label'] = 'Featured Image'; /* WAS: 'Image' */
  }
  if (isset($data['node-blog_post-field_tags'])) {
    $data['node-blog_post-field_tags']['field_instance']['display']['default']['type'] = 'hidden'; /* WAS: 'taxonomy_term_reference_link' */
    $data['node-blog_post-field_tags']['field_instance']['display']['teaser']['type'] = 'hidden'; /* WAS: 'taxonomy_term_reference_link' */
    unset($data['node-blog_post-field_tags']['field_instance']['display']['default']['module']);
    unset($data['node-blog_post-field_tags']['field_instance']['display']['teaser']['module']);
  }
  if (isset($data['node-blog_post-title_field'])) {
    $data['node-blog_post-title_field']['field_instance']['display']['default']['weight'] = '4'; /* WAS: 4 */
    $data['node-blog_post-title_field']['field_instance']['display']['teaser']['weight'] = '0'; /* WAS: 0 */
  }
}

/**
 * Implements hook_strongarm_alter().
 */
function commerce_kickstart_overrides_test_strongarm_alter(&$data) {
  if (isset($data['field_bundle_settings_node__blog_post'])) {
    $data['field_bundle_settings_node__blog_post']->value['view_modes']['product_list']['custom_settings'] = FALSE; /* WAS: TRUE */
  }
}
