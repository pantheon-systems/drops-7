<?php

/**
 * Implements hook_features_export().
 */
function commerce_order_type_features_export($data, &$export, $module_name = '') {
  $pipe = array();

  $export['dependencies']['features'] = 'features';
  $export['dependencies']['commerce_features'] = 'commerce_features';
  $export['dependencies']['commerce_order_types'] = 'commerce_order_types';

  $info = commerce_order_types_order_types();
  foreach ($data as $type) {
    $export['dependencies'][$info[$type]['module']] = $info[$type]['module'];
    $export['features']['commerce_order_type'][$type] = $type;

    $fields = field_info_instances('commerce_order', $type);
    foreach ($fields as $name => $field) {
      $pipe['field'][] = "commerce_order-{$field['bundle']}-{$field['field_name']}";
    }
  }

  return $pipe;
}

/**
 * Implements hook_features_export_options().
 */
function commerce_order_type_features_export_options() {
  $feature_types = array();
  $order_types = commerce_order_types_order_types();
  if (!empty($order_types)) {
    foreach ($order_types as $type => $order_type) {
      $feature_types[$type] = $order_type['name'];
    }
  }
  return $feature_types;
}

/**
 * Implements hook_features_export_render().
 */
function commerce_order_type_features_export_render($module, $data, $export = NULL) {
  $order_types = commerce_order_types_order_types();
  $output = array();
  $output[] = '  $items = array(';
  $schema = drupal_get_schema('commerce_order_types_order_types');
  foreach ($data as $type) {
    if (isset($order_types[$type]) && $order_type = $order_types[$type]) {
      foreach ($order_type as $key => $type) {
        if (!empty($schema['fields'][$key]['serialize'])) {
          $order_type[$key] = unserialize($type);
        }
      }
      $output[] = "    '{$type}' => " . features_var_export($order_type, '    ') . ",";
    }
  }
  $output[] = '  );';
  $output[] = '  return $items;';
  $output = implode("\n", $output);
  return array('commerce_order_default_types' => $output);
}

/**
 * Implements hook_features_revert().
 */
function commerce_order_type_features_revert($module = NULL) {
  // Get default line item types
  if (module_hook($module, 'commerce_order_default_types')) {
    $default_types = module_invoke($module, 'commerce_order_default_types');
    $existing_types = commerce_order_types_order_types();
    foreach ($default_types as $type) {
      // Add or update.
      if (!isset($existing_types[$type['type']])) {
        $type['is_new'] = TRUE;
      }
      commerce_order_types_commerce_order_type_save($type, TRUE);
    }
  }
  else {
    drupal_set_message(t('Could not load default order types.'), 'error');
    return FALSE;
  }

  // Reset the static cache.
  drupal_static_reset('commerce_order_types_order_types');
  // Schedule a menu rebuild.
  variable_set('menu_rebuild_needed', TRUE);

  return TRUE;
}

/**
 * Implements hook_features_rebuild().
 */
function commerce_order_type_features_rebuild($module) {
  return commerce_order_type_features_revert($module);
}
