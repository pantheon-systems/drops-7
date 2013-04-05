<?php

/**
 * Implements hook_features_export().
 */
function commerce_coupon_type_features_export($data, &$export, $module_name = '') {
  $pipe = array();

  $map = features_get_default_map('commerce_coupon_type');
  $export['dependencies']['features'] = 'features';
  $export['dependencies']['commerce_features'] = 'commerce_features';
  $export['dependencies']['commerce_coupon'] = 'commerce_coupon';

  $info = commerce_coupon_get_types();
  foreach ($data as $type) {
    $export['dependencies'][$info[$type]->module] = $info[$type]->module;
    $export['features']['commerce_coupon_type'][$type] = $type;

    $fields = field_info_instances('commerce_coupon', $type);
    foreach ($fields as $name => $field) {
      $pipe['field'][] = "commerce_coupon-{$field['bundle']}-{$field['field_name']}";
    }
  }

  return $pipe;
}

/**
 * Implements hook_features_export_options().
 */
function commerce_coupon_type_features_export_options() {
  $feature_types = array();
  $coupon_types = commerce_coupon_get_types();
  if (!empty($coupon_types)) {
    foreach($coupon_types as $type) {
      $feature_types[$type->type] = $type->label;
    }
  }
  return $feature_types;
}

/**
 * Implements hook_features_export_render().
 */
function commerce_coupon_type_features_export_render($module, $data, $export = NULL) {
  $info = commerce_coupon_get_types();
  $output = array();
  $output[] = '  $items = array(';
  foreach ($data as $type) {
    if (isset($info[$type]) && $coupon_type = $info[$type]) {
      $output[] = "    '{$type}' => array(";
      foreach($coupon_type as $key => $value) {
        $output[] = "      '{$key}' => " . features_var_export($value, '      ') . ",";
      }
      $output[] = "    ),";
    }
  }
  $output[] = '  );';
  $output[] = '  return $items;';
  $output = implode("\n", $output);
  return array('commerce_coupon_default_types' => $output);
}

/**
 * Implements hook_features_revert().
 */
function commerce_coupon_type_features_revert($module = NULL) {
  // Get default coupon types
  if (module_hook($module, 'commerce_coupon_default_types')) {
    $default_types = module_invoke($module, 'commerce_coupon_default_types');
    $existing_types = commerce_coupon_get_types();
    foreach ($default_types as $type) {
      // Add / or update
      if (!isset($existing_types[$type['type']])) {
        $type['is_new'] = TRUE;
      }
      // Use UI function because it provides already the logic we need
      $type = new CommerceCouponType($type);

      module_invoke('commerce_coupon', 'type_save', $type);
    }
  }
  else {
    drupal_set_message(t('Could not load default coupon types.'), 'error');
    return FALSE;
  }

  // Reset the caches.
  entity_defaults_rebuild();
  rules_clear_cache(TRUE);
  // Schedule a menu rebuild.
  variable_set('menu_rebuild_needed', TRUE);
  return TRUE;
}

/**
 * Implements hook_features_rebuild().
 */
function commerce_coupon_type_features_rebuild($module) {
  return commerce_coupon_type_features_revert($module);
}
