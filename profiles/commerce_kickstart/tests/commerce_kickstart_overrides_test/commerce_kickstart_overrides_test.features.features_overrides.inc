<?php
/**
 * @file
 * commerce_kickstart_overrides_test.features.features_overrides.inc
 */

/**
 * Implements hook_features_override_default_overrides().
 */
function commerce_kickstart_overrides_test_features_override_default_overrides() {
  // This code is only used for UI in features. Exported alters hooks do the magic.
  $overrides = array();

  // Exported overrides for: field
  $overrides["field.node-blog_post-body.field_instance|label"] = 'Article body';
  $overrides["field.node-blog_post-body.field_instance|required"] = 1;
  $overrides["field.node-blog_post-field_blog_category.field_instance|display|default|module"]["DELETED"] = TRUE;
  $overrides["field.node-blog_post-field_blog_category.field_instance|display|default|type"] = 'hidden';
  $overrides["field.node-blog_post-field_blog_category.field_instance|display|teaser|module"]["DELETED"] = TRUE;
  $overrides["field.node-blog_post-field_blog_category.field_instance|display|teaser|type"] = 'hidden';
  $overrides["field.node-blog_post-field_image.field_instance|fences_wrapper"] = '';
  $overrides["field.node-blog_post-field_image.field_instance|label"] = 'Featured Image';
  $overrides["field.node-blog_post-field_tags.field_instance|display|default|module"]["DELETED"] = TRUE;
  $overrides["field.node-blog_post-field_tags.field_instance|display|default|type"] = 'hidden';
  $overrides["field.node-blog_post-field_tags.field_instance|display|teaser|module"]["DELETED"] = TRUE;
  $overrides["field.node-blog_post-field_tags.field_instance|display|teaser|type"] = 'hidden';
  $overrides["field.node-blog_post-title_field.field_instance|display|default|weight"] = '4';
  $overrides["field.node-blog_post-title_field.field_instance|display|teaser|weight"] = '0';

  // Exported overrides for: variable
  $overrides["variable.field_bundle_settings_node__blog_post.value|view_modes|product_list|custom_settings"] = FALSE;

 return $overrides;
}
