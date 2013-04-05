<?php
/**
 * Implements hook_features_export().
 */
function commerce_flat_rate_services_features_export($data, &$export, $module_name = '') {
  $pipe = array();

  $export['dependencies']['features'] = 'features';
  $export['dependencies']['commerce_features'] = 'commerce_features';
  $export['dependencies']['commerce_flat_rate'] = 'commerce_flat_rate';
  $export['dependencies']['rules'] = 'rules';

  // Get the list of the commerce rates and export them plus the rules
  // component which is related.
  $info = commerce_shipping_services('flat_rate');
  foreach ($data as $service) {
    if (isset($info[$service]['module'])) {
      $export['dependencies'][$info[$service]['module']] = $info[$service]['module'];
    }
    $export['features']['commerce_flat_rate_services'][$service] = $service;
    $export['features']['rules_config']['commerce_flat_rate_services_' . $service] = 'commerce_flat_rate_services_' . $service;
  }
  return $pipe;
}

/**
 * Implements hook_features_export_options().
 */
function commerce_flat_rate_services_features_export_options() {
  $feature_types = array();
  $flat_rate_services = commerce_shipping_services('flat_rate');
  if (!empty($flat_rate_services)) {
    foreach ($flat_rate_services as $service) {
      $feature_types[$service['name']] = $service['display_title'];
    }
  }
  return $feature_types;
}

/**
 * Implements hook_features_export_render().
 */
function commerce_flat_rate_services_features_export_render($module, $data, $export = NULL) {
  $info = commerce_shipping_services('flat_rate');

  $output = array();
  $output[] = '  $items = array(';
  foreach ($data as $service) {
    if (isset($info[$service]) && $flat_rate_service = $info[$service]) {
      $output[] = "    '{$service}' => " . features_var_export($flat_rate_service, '    ') . ",";
    }

  }
  $output[] = '  );';
  $output[] = '  return $items;';
  $output = implode("\n", $output);
  return array('commerce_flat_rate_default_services' => $output);
}

/**
 * Implements hook_features_revert().
 */
function commerce_flat_rate_services_features_revert($module = NULL) {
  // Get default flat rate services

  if (module_hook($module, 'commerce_flat_rate_default_services')) {
    $default_types = module_invoke($module, 'commerce_flat_rate_default_services');
    $existing_types = commerce_shipping_services('flat_rate');
    foreach ($default_types as $type) {
      // Add / or update
      if (!isset($existing_types[$type['name']])) {
        $type['is_new'] = TRUE;
      }
      commerce_flat_rate_service_save($type, TRUE);
    }
  }
  else {
    drupal_set_message(t('Could not load default flat rate services.'), 'error');
    return FALSE;
  }

  // Reset the caches.
  commerce_shipping_services_reset();
  entity_defaults_rebuild();
  rules_clear_cache(TRUE);
  // Schedule a menu rebuild.
  variable_set('menu_rebuild_needed', TRUE);

  return TRUE;
}

/**
 * Implements hook_features_rebuild().
 */
function commerce_flat_rate_services_features_rebuild($module) {
  commerce_shipping_services_reset();
  return commerce_flat_rate_services_features_revert($module);
}
