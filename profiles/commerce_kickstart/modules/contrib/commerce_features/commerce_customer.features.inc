<?php

/**
 * Implements hook_features_export().
 */
function commerce_customer_features_export($data, &$export, $module_name = '') {
  $pipe = array();

  // Add commerce_customer_profile as dependency
  $export['dependencies']['features'] = 'features';
  $export['dependencies']['commerce_features'] = 'commerce_features';
  $export['dependencies']['commerce_customer_ui'] = 'commerce_customer_ui';

  $info = commerce_customer_profile_types();
  foreach ($data as $type) {
    // Add module dependencies
    $export['dependencies'][$info[$type]['module']] = $info[$type]['module'];
    $export['features']['commerce_customer_type'][$type] = $type;

    // Fetch fields of the profile type and add them as dependency
    $fields = field_info_instances('commerce_customer_profile', $type);
    foreach ($fields as $name => $field) {
      $pipe['field'][] = "commerce_customer_profile-{$field['bundle']}-{$field['field_name']}";
    }
  }
  return $pipe;
}

/**
 * Implements hook_features_export_options().
 */
function commerce_customer_features_export_options() {
  $feature_types = array();
  $profile_types = commerce_customer_profile_types();
  if (!empty($profile_types)) {
    foreach ($profile_types as $type) {
      $feature_types[$type['type']] = $type['name'];
    }
  }
  return $feature_types;
}

/**
 * Implements hook_features_export_render().
 */
function commerce_customer_features_export_render($module, $data) {
  // Return nothing, since the appropriate code has to exist already
  return array();
}

/**
 * Implements hook_features_revert().
 */
function commerce_customer_features_revert($module) {
  // Nothing to do here besides recaching - fields are handled by the fields
  // implementation of features.
  // Re-Cache
  drupal_static_reset('commerce_customer_profile_types');
  commerce_customer_profile_types();

  return TRUE;
}

/**
 * Implements hook_features_rebuild().
 */
function commerce_customer_features_rebuild($module) {
  return commerce_customer_features_revert($module);
}
