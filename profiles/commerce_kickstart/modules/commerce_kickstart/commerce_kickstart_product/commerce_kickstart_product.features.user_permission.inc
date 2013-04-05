<?php
/**
 * @file
 * commerce_kickstart_product.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function commerce_kickstart_product_user_default_permissions() {
  $permissions = array();

  // Exported permission: view any commerce_product entity of bundle bags_cases.
  $permissions['view any commerce_product entity of bundle bags_cases'] = array(
    'name' => 'view any commerce_product entity of bundle bags_cases',
    'roles' => array(
      0 => 'administrator',
      1 => 'anonymous user',
      2 => 'authenticated user',
    ),
    'module' => 'commerce_product',
  );

  // Exported permission: view any commerce_product entity of bundle drinks.
  $permissions['view any commerce_product entity of bundle drinks'] = array(
    'name' => 'view any commerce_product entity of bundle drinks',
    'roles' => array(
      0 => 'administrator',
      1 => 'anonymous user',
      2 => 'authenticated user',
    ),
    'module' => 'commerce_product',
  );

  // Exported permission: view any commerce_product entity of bundle hats.
  $permissions['view any commerce_product entity of bundle hats'] = array(
    'name' => 'view any commerce_product entity of bundle hats',
    'roles' => array(
      0 => 'administrator',
      1 => 'anonymous user',
      2 => 'authenticated user',
    ),
    'module' => 'commerce_product',
  );

  // Exported permission: view any commerce_product entity of bundle shoes.
  $permissions['view any commerce_product entity of bundle shoes'] = array(
    'name' => 'view any commerce_product entity of bundle shoes',
    'roles' => array(
      0 => 'administrator',
      1 => 'anonymous user',
      2 => 'authenticated user',
    ),
    'module' => 'commerce_product',
  );

  // Exported permission: view any commerce_product entity of bundle storage_devices.
  $permissions['view any commerce_product entity of bundle storage_devices'] = array(
    'name' => 'view any commerce_product entity of bundle storage_devices',
    'roles' => array(
      0 => 'administrator',
      1 => 'anonymous user',
      2 => 'authenticated user',
    ),
    'module' => 'commerce_product',
  );

  // Exported permission: view any commerce_product entity of bundle tops.
  $permissions['view any commerce_product entity of bundle tops'] = array(
    'name' => 'view any commerce_product entity of bundle tops',
    'roles' => array(
      0 => 'administrator',
      1 => 'anonymous user',
      2 => 'authenticated user',
    ),
    'module' => 'commerce_product',
  );

  // Exported permission: view own commerce_product entities of bundle bags_cases.
  $permissions['view own commerce_product entities of bundle bags_cases'] = array(
    'name' => 'view own commerce_product entities of bundle bags_cases',
    'roles' => array(
      0 => 'administrator',
      1 => 'anonymous user',
      2 => 'authenticated user',
    ),
    'module' => 'commerce_product',
  );

  // Exported permission: view own commerce_product entities of bundle drinks.
  $permissions['view own commerce_product entities of bundle drinks'] = array(
    'name' => 'view own commerce_product entities of bundle drinks',
    'roles' => array(
      0 => 'administrator',
      1 => 'anonymous user',
      2 => 'authenticated user',
    ),
    'module' => 'commerce_product',
  );

  // Exported permission: view own commerce_product entities of bundle hats.
  $permissions['view own commerce_product entities of bundle hats'] = array(
    'name' => 'view own commerce_product entities of bundle hats',
    'roles' => array(
      0 => 'administrator',
      1 => 'anonymous user',
      2 => 'authenticated user',
    ),
    'module' => 'commerce_product',
  );

  // Exported permission: view own commerce_product entities of bundle shoes.
  $permissions['view own commerce_product entities of bundle shoes'] = array(
    'name' => 'view own commerce_product entities of bundle shoes',
    'roles' => array(
      0 => 'administrator',
      1 => 'anonymous user',
      2 => 'authenticated user',
    ),
    'module' => 'commerce_product',
  );

  // Exported permission: view own commerce_product entities of bundle storage_devices.
  $permissions['view own commerce_product entities of bundle storage_devices'] = array(
    'name' => 'view own commerce_product entities of bundle storage_devices',
    'roles' => array(
      0 => 'administrator',
      1 => 'anonymous user',
      2 => 'authenticated user',
    ),
    'module' => 'commerce_product',
  );

  // Exported permission: view own commerce_product entities of bundle tops.
  $permissions['view own commerce_product entities of bundle tops'] = array(
    'name' => 'view own commerce_product entities of bundle tops',
    'roles' => array(
      0 => 'administrator',
      1 => 'anonymous user',
      2 => 'authenticated user',
    ),
    'module' => 'commerce_product',
  );

  return $permissions;
}
