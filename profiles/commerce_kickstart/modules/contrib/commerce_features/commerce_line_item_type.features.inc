<?php

/**
 * Implements hook_features_export().
 */
function commerce_line_item_type_features_export($data, &$export, $module_name = '') {
  $pipe = array();

  // Add dependencies.
  $export['dependencies']['features'] = 'features';
  $export['dependencies']['commerce_features'] = 'commerce_features';
  $export['dependencies']['commerce_custom_product'] = 'commerce_custom_product';

  foreach ($data as $type) {
    $export['features']['commerce_line_item_type'][$type] = $type;

    // Fetch fields of the profile type and add them as dependency
    $fields = field_info_instances('commerce_line_item', $type);
    foreach ($fields as $field) {
      $pipe['field'][] = "commerce_line_item-{$field['bundle']}-{$field['field_name']}";
    }
  }
  return $pipe;
}

/**
 * Implements hook_features_export_options().
 */
function commerce_line_item_type_features_export_options() {
  $feature_types = array();
  $line_item_types = commerce_custom_product_commerce_line_item_type_info();
  if (!empty($line_item_types)) {
    foreach ($line_item_types as $type => $line_item_type) {
      $feature_types[$type] = $line_item_type['name'];
    }
  }
  return $feature_types;
}

/**
 * Implements hook_features_export_render().
 */
function commerce_line_item_type_features_export_render($module, $data, $export = NULL) {
  $line_item_types = commerce_custom_product_commerce_line_item_type_info();
  $output = array();
  $output[] = '  $items = array(';
  foreach ($data as $type) {
    if (isset($line_item_types[$type]) && $line_item_type = $line_item_types[$type]) {
      $output[] = "    '{$type}' => " . features_var_export($line_item_type, '    ') . ",";
    }
  }
  $output[] = '  );';
  $output[] = '  return $items;';
  $output = implode("\n", $output);
  return array('commerce_line_item_default_types' => $output);
}

/**
 * Implements hook_features_revert().
 */
function commerce_line_item_type_features_revert($module) {
  // Get default line item types
  if (module_hook($module, 'commerce_line_item_default_types')) {
    $default_types = module_invoke($module, 'commerce_line_item_default_types');
    foreach ($default_types as $type => $line_item_type) {
      $line_item_type['type'] = $type;
      commerce_custom_product_line_item_type_save($line_item_type, TRUE, TRUE);
    }
  }
  else {
    drupal_set_message(t('Could not load default line item types.'), 'error');
    return FALSE;
  }

  // Reset the static cache.
  commerce_line_item_types_reset();
  // Schedule a menu rebuild.
  variable_set('menu_rebuild_needed', TRUE);

  return TRUE;
}

/**
 * Implements hook_features_rebuild().
 */
function commerce_line_item_type_features_rebuild($module) {
  return commerce_line_item_type_features_revert($module);
}

