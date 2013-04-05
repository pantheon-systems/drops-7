<?php

/**
 * Implements hook_features_export().
 */
function commerce_tax_rate_features_export($data, &$export, $module_name = '') {
  $pipe = array();

  $export['dependencies']['features'] = 'features';
  $export['dependencies']['commerce_features'] = 'commerce_features';
  $export['dependencies']['commerce_tax_ui'] = 'commerce_tax_ui';
  $export['dependencies']['rules'] = 'rules';

  // Get the list of the commerce rates and export them plus the rules
  // component which is related.
  $info = commerce_tax_rates();
  foreach ($data as $rate) {
    $export['dependencies'][$info[$rate]['module']] = $info[$rate]['module'];
    $export['features']['commerce_tax_rate'][$rate] = $rate;
    $export['features']['rules_config']['commerce_tax_rate_' . $rate] = 'commerce_tax_rate_' . $rate;
  }
  return $pipe;
}

/**
 * Implements hook_features_export_options().
 */
function commerce_tax_rate_features_export_options() {
  $feature_types = array();
  $tax_rates = commerce_tax_rates();
  if (!empty($tax_rates)) {
    foreach($tax_rates as $rate) {
      $feature_types[$rate['name']] = $rate['title'];
    }
  }
  return $feature_types;
}

/**
 * Implements hook_features_export_render().
 */
function commerce_tax_rate_features_export_render($module, $data, $export = NULL) {
  $info = commerce_tax_rates();

  $output = array();
  $output[] = '  $items = array(';
  foreach ($data as $type) {
    if (isset($info[$type]) && $tax_rate = $info[$type]) {
      $output[] = "    '{$type}' => " . features_var_export($tax_rate, '    ') . ",";
    }

  }
  $output[] = '  );';
  $output[] = '  return $items;';
  $output = implode("\n", $output);
  return array('commerce_tax_default_rates' => $output);
}

/**
 * Implements hook_features_revert().
 */
function commerce_tax_rate_features_revert($module = NULL) {
  // Get default tax rates.
  if (module_hook($module, 'commerce_tax_default_rates')) {
    $default_rates = module_invoke($module, 'commerce_tax_default_rates');
    $existing_rates = commerce_tax_rates();
    foreach ($default_rates as $rate) {
      // Add / or update
      if (!isset($existing_rates[$rate['name']])) {
        $rate['is_new'] = TRUE;
      }
      // Use UI function because it provides already the logic we need
      module_invoke('commerce_tax_ui', 'tax_rate_save', $rate, TRUE);
    }
  }
  else {
    drupal_set_message(t('Could not load default tax rates.'), 'error');
    return FALSE;
  }

  // Reset the caches.
  commerce_tax_rates_reset();
  entity_defaults_rebuild();
  rules_clear_cache(TRUE);
  // Schedule a menu rebuild.
  variable_set('menu_rebuild_needed', TRUE);

  return TRUE;
}

/**
 * Implements hook_features_rebuild().
 */
function commerce_tax_rate_features_rebuild($module) {
  return commerce_tax_rate_features_revert($module);
}
