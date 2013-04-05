<?php

/**
 * Implements hook_features_export().
 */
function commerce_tax_type_features_export($data, &$export, $module_name = '') {
  $pipe = array();

  $export['dependencies']['features'] = 'features';
  $export['dependencies']['commerce_features'] = 'commerce_features';
  $export['dependencies']['commerce_tax'] = 'commerce_tax';
  $export['dependencies']['rules'] = 'rules';

  // Get the list of the commerce types and export them plus the rules
  // component which is related.
  $info = commerce_tax_types();
  foreach ($data as $type) {
    $export['dependencies'][$info[$type]['module']] = $info[$type]['module'];
    $export['features']['commerce_tax_type'][$type] = $type;
  }
  return $pipe;
}

/**
 * Implements hook_features_export_options().
 */
function commerce_tax_type_features_export_options() {
  $feature_types = array();
  $tax_types = commerce_tax_types();
  if (!empty($tax_types)) {
    foreach($tax_types as $type) {
      $feature_types[$type['name']] = $type['title'];
    }
  }
  return $feature_types;
}

/**
 * Implements hook_features_export_render().
 */
function commerce_tax_type_features_export_render($module, $data, $export = NULL) {
  $info = commerce_tax_types();
  $output = array();
  $output[] = '  $items = array(';
  foreach ($data as $type) {
    if (isset($info[$type]) && $tax_type = $info[$type]) {
      $output[] = "    '{$type}' => " . features_var_export($tax_type, '    ') . ",";
    }
  }
  $output[] = '  );';
  $output[] = '  return $items;';
  $output = implode("\n", $output);
  return array('commerce_tax_default_types' => $output);
}

/**
 * Implements hook_features_revert().
 */
function commerce_tax_type_features_revert($module = NULL) {
  // Get default tax types
  if (module_hook($module, 'commerce_tax_default_types')) {
    $default_types = module_invoke($module, 'commerce_tax_default_types');
    $existing_types = commerce_tax_types();
    foreach ($default_types as $type) {
      // Add / or update
      if (!isset($existing_types[$type['name']])) {
        $type['is_new'] = TRUE;
      }
      // Use UI function because it provides already the logic we need
      module_invoke('commerce_tax_ui', 'tax_type_save', $type, TRUE);
    }
  }
  else {
    drupal_set_message(t('Could not load default tax types.'), 'error');
    return FALSE;
  }

  // Reset the caches.
  commerce_tax_types_reset();
  entity_defaults_rebuild();
  rules_clear_cache(TRUE);
  // Schedule a menu rebuild.
  variable_set('menu_rebuild_needed', TRUE);

  return TRUE;
}

/**
 * Implements hook_features_rebuild().
 */
function commerce_tax_type_features_rebuild($module) {
  return commerce_tax_type_features_revert($module);
}
