<?php
/**
 * @file
 * commerce_kickstart_product.autosku_pattern.inc
 */

/**
 * Implements hook_default_commerce_autosku_patterns().
 */
function commerce_kickstart_product_default_commerce_autosku_patterns() {
  $export = array();

  $commerce_autosku_patterns = new stdClass();
  $commerce_autosku_patterns->disabled = FALSE; /* Edit this to true to make a default commerce_autosku_patterns disabled initially */
  $commerce_autosku_patterns->api_version = 1;
  $commerce_autosku_patterns->product_type = 'bags_cases';
  $commerce_autosku_patterns->pattern = '[commerce-product:title]';
  $commerce_autosku_patterns->advanced = array(
    'update_existing' => 0,
    'hide_sku' => 1,
    'case' => 'uppercase',
  );
  $export['bags_cases'] = $commerce_autosku_patterns;

  $commerce_autosku_patterns = new stdClass();
  $commerce_autosku_patterns->disabled = FALSE; /* Edit this to true to make a default commerce_autosku_patterns disabled initially */
  $commerce_autosku_patterns->api_version = 1;
  $commerce_autosku_patterns->product_type = 'drinks';
  $commerce_autosku_patterns->pattern = '[commerce-product:title]';
  $commerce_autosku_patterns->advanced = array(
    'update_existing' => 0,
    'hide_sku' => 1,
    'case' => 'uppercase',
  );
  $export['drinks'] = $commerce_autosku_patterns;

  $commerce_autosku_patterns = new stdClass();
  $commerce_autosku_patterns->disabled = FALSE; /* Edit this to true to make a default commerce_autosku_patterns disabled initially */
  $commerce_autosku_patterns->api_version = 1;
  $commerce_autosku_patterns->product_type = 'hats';
  $commerce_autosku_patterns->pattern = '[commerce-product:title]';
  $commerce_autosku_patterns->advanced = array(
    'update_existing' => 0,
    'hide_sku' => 1,
    'case' => 'uppercase',
  );
  $export['hats'] = $commerce_autosku_patterns;

  $commerce_autosku_patterns = new stdClass();
  $commerce_autosku_patterns->disabled = FALSE; /* Edit this to true to make a default commerce_autosku_patterns disabled initially */
  $commerce_autosku_patterns->api_version = 1;
  $commerce_autosku_patterns->product_type = 'shoes';
  $commerce_autosku_patterns->pattern = '[commerce-product:title]';
  $commerce_autosku_patterns->advanced = array(
    'update_existing' => 0,
    'hide_sku' => 1,
    'case' => 'uppercase',
  );
  $export['shoes'] = $commerce_autosku_patterns;

  $commerce_autosku_patterns = new stdClass();
  $commerce_autosku_patterns->disabled = FALSE; /* Edit this to true to make a default commerce_autosku_patterns disabled initially */
  $commerce_autosku_patterns->api_version = 1;
  $commerce_autosku_patterns->product_type = 'storage_devices';
  $commerce_autosku_patterns->pattern = '[commerce-product:title]';
  $commerce_autosku_patterns->advanced = array(
    'update_existing' => 0,
    'hide_sku' => 1,
    'case' => 'uppercase',
  );
  $export['storage_devices'] = $commerce_autosku_patterns;

  $commerce_autosku_patterns = new stdClass();
  $commerce_autosku_patterns->disabled = FALSE; /* Edit this to true to make a default commerce_autosku_patterns disabled initially */
  $commerce_autosku_patterns->api_version = 1;
  $commerce_autosku_patterns->product_type = 'tops';
  $commerce_autosku_patterns->pattern = '[commerce-product:title]';
  $commerce_autosku_patterns->advanced = array(
    'update_existing' => 0,
    'hide_sku' => 1,
    'case' => 'uppercase',
  );
  $export['tops'] = $commerce_autosku_patterns;

  return $export;
}
