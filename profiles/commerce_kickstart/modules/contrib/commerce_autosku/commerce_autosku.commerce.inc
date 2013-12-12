<?php

/**
 * @file
 * Contains Drupal Commerce specific hook implementations.
 * Included automatically.
 */

/**
 * Implements hook_commerce_product_type_update().
 */
function commerce_autosku_commerce_product_type_update($type) {
  if(isset($type['autosku_status']) && !$type['autosku_status']) {
    $type['autosku']['pattern'] = FALSE;
  }
  ctools_include('export');
  if(!$record = ctools_export_crud_load('commerce_autosku_patterns', $type['type'])) {
    $record = ctools_export_crud_new('commerce_autosku_patterns');
  }
  $record->product_type = $type['type'];
  $record->pattern = !empty($type['autosku']['pattern']) ? $type['autosku']['pattern'] : FALSE;
  $record->advanced = $type['autosku']['advanced'];

  ctools_export_crud_save('commerce_autosku_patterns', $record);
}

/**
 * Implements hook_commerce_product_type_insert().
 */
function commerce_autosku_commerce_product_type_insert($type) {
  if(isset($type['autosku_status']) && !$type['autosku_status']) {
    $type['autosku']['pattern'] = FALSE;
  }
  ctools_include('export');
  $record = ctools_export_crud_new('commerce_autosku_patterns');
  $record->product_type = $type['type'];
  $record->pattern = !empty($type['autosku']['pattern']) ? $type['autosku']['pattern'] : FALSE;
  $record->advanced = isset($type['autosku']['advanced']) ? $type['autosku']['advanced'] : array();

  ctools_export_crud_save('commerce_autosku_patterns', $record);
}

/**
 * Implements hook_commerce_product_type_delete().
 */
function commerce_autosku_commerce_product_type_delete($type) {
  ctools_include('export');
  if($record = ctools_export_crud_load('commerce_autosku_patterns', $type['type'])) {
    ctools_export_crud_delete('commerce_autosku_patterns', $record);
  }
}

/**
 * Implements hook_commerce_product_insert().
 */
function commerce_autosku_commerce_product_insert($product) {
  if (_commerce_autosku_is_temporary_sku($product->sku)) {
    unset($product->is_new);
    //Trigger immediate resave now that we have the ID.
    commerce_product_save($product);
    //Reset is_new in case anyone needs it after us.
    $product->is_new = TRUE;
  }
}

/**
 * Implements hook_commerce_product_presave().
 */
function commerce_autosku_commerce_product_presave($product) {
  $settings = commerce_autosku_get_settings($product);

  if ($settings) {
    if (_commerce_autosku_needs_temporary_sku($product, $settings)) {
      $product->sku = _commerce_autosku_get_temporary_sku();
    }
    //@todo: Replace TRUE with a check if we should update existing SKUS.
    elseif (empty($product->sku) || _commerce_autosku_is_temporary_sku($product->sku) || $settings['advanced']['update_existing']) {
      // Determine the casing to use for the generated SKU from the settings.
      if (!empty($settings['advanced']['case'])) {
        $case = $settings['advanced']['case'];
      }
      else {
        $case = 'original';
      }

      // Generate the SKU.
      $product->sku = commerce_autosku_generate_sku($product, $settings['pattern'], $case);
    }
  }
}

/**
 * Implements hook_commerce_product_type_info_alter().
 */
function commerce_autosku_commerce_product_type_info_alter(&$types) {
  if (!empty($types)) {
    ctools_include('export');
    $patterns = ctools_export_crud_load_multiple('commerce_autosku_patterns', array_keys($types));

    foreach($types as $name => &$type) {
      if(isset($patterns[$name])) {
        $type['autosku'] = (array) $patterns[$name];
      }
      elseif(empty($type['autosku'])) {
        $type['autosku'] = FALSE;
      }
    }
  }
}
