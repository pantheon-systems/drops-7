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
      $data = array('commerce-product' => $product);
      $sku = token_replace($settings['pattern'], $data, array('clear' => TRUE));
      $sku = commerce_autosku_cleanstring($sku);
      $sku = drupal_substr($sku, 0, 255);
      if ($settings['advanced']['case']) {
        switch ($settings['advanced']['case']) {
          case 'uppercase':
            $sku = drupal_strtoupper($sku);
            break;
          case 'lowercase':
            $sku = drupal_strtolower($sku);
        }
      }

      //Borrowed from pathauto...
      if (!commerce_product_validate_sku_unique($sku, $product->product_id)) {
        $original_sku = $sku;
        $i = 0;

        do {
          $sku = drupal_substr($original_sku, 0, (255 - drupal_strlen(COMMERCE_AUTOSKU_REPLACEMENT . $i)));
          $sku = commerce_autosku_cleanstring($sku . COMMERCE_AUTOSKU_REPLACEMENT . $i);
          $i++;
        } while (!commerce_product_validate_sku_unique($sku, $product->product_id));
      }
      $product->sku = $sku;
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