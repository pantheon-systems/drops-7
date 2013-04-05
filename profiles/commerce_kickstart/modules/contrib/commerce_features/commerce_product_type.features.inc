<?php

/**
 * Implements hook_features_export().
 */
function commerce_product_type_features_export($data, &$export, $module_name = '') {
  $pipe = array();

  // Add required dependencies.
  $export['dependencies']['features'] = 'features';
  $export['dependencies']['commerce_features'] = 'commerce_features';
  $export['dependencies']['commerce_product_ui'] = 'commerce_product_ui';

  foreach ($data as $type) {
    $export['features']['commerce_product_type'][$type] = $type;

    $fields = field_info_instances('commerce_product', $type);
    foreach ($fields as $field) {
      $pipe['field'][] = "commerce_product-{$field['bundle']}-{$field['field_name']}";
    }
  }

  return $pipe;
}

/**
 * Implements hook_features_export_options().
 */
function commerce_product_type_features_export_options() {
  $feature_types = array();
  $product_types = commerce_product_ui_commerce_product_type_info();
  if (!empty($product_types)) {
    foreach ($product_types as $type) {
      $feature_types[$type['type']] = $type['name'];
    }
  }
  return $feature_types;
}

/**
 * Implements hook_features_export_render().
 */
function commerce_product_type_features_export_render($module, $data, $export = NULL) {
  $info = commerce_product_ui_commerce_product_type_info();
  $output = array();
  $output[] = '  $items = array(';
  foreach ($data as $type) {
    if (isset($info[$type]) && $product_type = $info[$type]) {
      $output[] = "    '{$type}' => " . features_var_export($product_type, '    ') . ",";
    }
  }
  $output[] = '  );';
  $output[] = '  return $items;';
  $output = implode("\n", $output);
  return array('commerce_product_default_types' => $output);
}

/**
 * Implements hook_features_revert().
 */
function commerce_product_type_features_revert($module = NULL) {
  // Get default product types
  if (module_hook($module, 'commerce_product_default_types')) {
    $default_types = module_invoke($module, 'commerce_product_default_types');
    // Read the product types from the database instead of calling all
    $existing_types = commerce_product_ui_commerce_product_type_info();
    foreach ($default_types as $type) {
      // Add or update.
      if (!isset($existing_types[$type['type']])) {
        $type['is_new'] = TRUE;
      }
      // Use UI function because it provides already the logic we need
      commerce_product_ui_product_type_save($type, TRUE, TRUE);
    }
  }
  else {
    drupal_set_message(t('Could not load default product types.'), 'error');
    return FALSE;
  }

  // Reset the static cache.
  commerce_product_types_reset();
  // Schedule a menu rebuild.
  variable_set('menu_rebuild_needed', TRUE);

  return TRUE;
}

/**
 * Implements hook_features_rebuild().
 */
function commerce_product_type_features_rebuild($module) {
  return commerce_product_type_features_revert($module);
}
