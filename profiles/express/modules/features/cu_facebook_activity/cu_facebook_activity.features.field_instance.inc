<?php
/**
 * @file
 * cu_facebook_activity.features.field_instance.inc
 */

/**
 * Implements hook_field_default_field_instances().
 */
function cu_facebook_activity_field_default_field_instances() {
  $field_instances = array();

  // Exported field_instance: 'bean-facebook_activity-field_fb_like_faces'.
  $field_instances['bean-facebook_activity-field_fb_like_faces'] = array(
    'bundle' => 'facebook_activity',
    'default_value' => array(
      0 => array(
        'value' => 'true',
      ),
    ),
    'default_value_function' => '',
    'default_value_php' => '',
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'list',
        'settings' => array(),
        'type' => 'list_key',
        'weight' => 4,
      ),
    ),
    'entity_type' => 'bean',
    'fences_wrapper' => 'no_wrapper',
    'field_name' => 'field_fb_like_faces',
    'label' => 'Show Faces',
    'required' => 1,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(),
      'type' => 'options_select',
      'weight' => 1,
    ),
  );

  // Exported field_instance: 'bean-facebook_activity-field_fb_url'.
  $field_instances['bean-facebook_activity-field_fb_url'] = array(
    'bundle' => 'facebook_activity',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'This must be a url to a Facebook Page, not a Facebook user.',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'link',
        'settings' => array(),
        'type' => 'link_plain',
        'weight' => 3,
      ),
    ),
    'entity_type' => 'bean',
    'fences_wrapper' => 'no_wrapper',
    'field_name' => 'field_fb_url',
    'label' => 'URL',
    'required' => 1,
    'settings' => array(
      'absolute_url' => 1,
      'attributes' => array(
        'class' => '',
        'configurable_title' => 0,
        'rel' => '',
        'target' => 'default',
        'title' => '',
      ),
      'display' => array(
        'url_cutoff' => 80,
      ),
      'enable_tokens' => 1,
      'title' => 'none',
      'title_label_use_field_label' => FALSE,
      'title_maxlength' => 128,
      'title_value' => '',
      'url' => 0,
      'user_register_form' => FALSE,
      'validate_url' => 1,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'link',
      'settings' => array(),
      'type' => 'link_field',
      'weight' => 0,
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Show Faces');
  t('This must be a url to a Facebook Page, not a Facebook user.');
  t('URL');

  return $field_instances;
}
