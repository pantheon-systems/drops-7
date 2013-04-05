<?php
/**
 * @file
 * commerce_kickstart_product.features.inc
 */

/**
 * Implements hook_commerce_flat_rate_default_services().
 */
function commerce_kickstart_product_commerce_flat_rate_default_services() {
  $items = array(
    'express_shipping' => array(
      'name' => 'express_shipping',
      'base' => 'express_shipping',
      'display_title' => 'Express shipping: 1 business day',
      'description' => 'An express shipping service with additional fee.',
      'shipping_method' => 'flat_rate',
      'rules_component' => TRUE,
      'price_component' => 'flat_rate_express_shipping',
      'weight' => 0,
      'callbacks' => array(
        'rate' => 'commerce_flat_rate_service_rate_order',
        'details_form' => 'express_shipping_details_form',
        'details_form_validate' => 'express_shipping_details_form_validate',
        'details_form_submit' => 'express_shipping_details_form_submit',
      ),
      'module' => 'commerce_flat_rate',
      'title' => 'Express Shipping',
      'base_rate' => array(
        'amount' => '1500',
        'currency_code' => 'USD',
        'data' => array(),
      ),
      'data' => array(),
      'admin_list' => TRUE,
    ),
    'free_shipping' => array(
      'name' => 'free_shipping',
      'base' => 'free_shipping',
      'display_title' => 'Free shipping: 5 - 8 business days',
      'description' => 'A free shipping service for an order over a certain amount.',
      'shipping_method' => 'flat_rate',
      'rules_component' => TRUE,
      'price_component' => 'flat_rate_free_shipping',
      'weight' => 1,
      'callbacks' => array(
        'rate' => 'commerce_flat_rate_service_rate_order',
        'details_form' => 'free_shipping_details_form',
        'details_form_validate' => 'free_shipping_details_form_validate',
        'details_form_submit' => 'free_shipping_details_form_submit',
      ),
      'module' => 'commerce_flat_rate',
      'title' => 'Free Shipping',
      'base_rate' => array(
        'amount' => '0',
        'currency_code' => 'USD',
        'data' => array(),
      ),
      'data' => array(),
      'admin_list' => TRUE,
    ),
    'standard_shipping' => array(
      'name' => 'standard_shipping',
      'base' => 'standard_shipping',
      'display_title' => 'Standard shipping: 3 - 5 business days',
      'description' => 'A standard shipping service.',
      'shipping_method' => 'flat_rate',
      'rules_component' => TRUE,
      'price_component' => 'flat_rate_standard_shipping',
      'weight' => 2,
      'callbacks' => array(
        'rate' => 'commerce_flat_rate_service_rate_order',
        'details_form' => 'standard_shipping_details_form',
        'details_form_validate' => 'standard_shipping_details_form_validate',
        'details_form_submit' => 'standard_shipping_details_form_submit',
      ),
      'module' => 'commerce_flat_rate',
      'title' => 'Standard Shipping',
      'base_rate' => array(
        'amount' => '800',
        'currency_code' => 'USD',
        'data' => array(),
      ),
      'data' => array(),
      'admin_list' => TRUE,
    ),
  );
  return $items;
}

/**
 * Implements hook_commerce_product_default_types().
 */
function commerce_kickstart_product_commerce_product_default_types() {
  $items = array(
    'bags_cases' => array(
      'type' => 'bags_cases',
      'name' => 'Bags & Cases',
      'description' => '<em>Bags & Cases</em> are products which contain a size and a color.',
      'help' => '',
      'revision' => '1',
    ),
    'drinks' => array(
      'type' => 'drinks',
      'name' => 'Drinks',
      'description' => '<em>Drinks</em> are products which contain a color.',
      'help' => '',
      'revision' => '1',
    ),
    'hats' => array(
      'type' => 'hats',
      'name' => 'Hats',
      'description' => '<em>Hats</em> are products which contain a size and a color.',
      'help' => '',
      'revision' => '1',
    ),
    'shoes' => array(
      'type' => 'shoes',
      'name' => 'Shoes',
      'description' => '<em>Shoes</em> are products which contain a size and a color.',
      'help' => '',
      'revision' => '1',
    ),
    'storage_devices' => array(
      'type' => 'storage_devices',
      'name' => 'Storage Devices',
      'description' => '<em>Storage devices</em> are products which contain a capacity.',
      'help' => '',
      'revision' => '1',
    ),
    'tops' => array(
      'type' => 'tops',
      'name' => 'Tops',
      'description' => '<em>Tops</em> are products which contain a size and a color.',
      'help' => '',
      'revision' => '1',
    ),
  );
  return $items;
}

/**
 * Implements hook_ctools_plugin_api().
 */
function commerce_kickstart_product_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_image_default_styles().
 */
function commerce_kickstart_product_image_default_styles() {
  $styles = array();

  // Exported image style: product_full.
  $styles['product_full'] = array(
    'name' => 'product_full',
    'effects' => array(
      1 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '400',
          'height' => '550',
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: product_medium.
  $styles['product_medium'] = array(
    'name' => 'product_medium',
    'effects' => array(
      1 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '230',
          'height' => '260',
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: product_thumbnail.
  $styles['product_thumbnail'] = array(
    'name' => 'product_thumbnail',
    'effects' => array(
      1 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '110',
          'height' => '130',
        ),
        'weight' => '1',
      ),
    ),
  );

  return $styles;
}

/**
 * Implements hook_node_info().
 */
function commerce_kickstart_product_node_info() {
  $items = array(
    'bags_cases' => array(
      'name' => t('Bags & Cases'),
      'base' => 'node_content',
      'description' => t('A <em>Bags & Cases</em> is a content type which contain product variations.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'drinks' => array(
      'name' => t('Drinks'),
      'base' => 'node_content',
      'description' => t('A <em>Drinks</em> is a content type which contain product variations.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'hats' => array(
      'name' => t('Hats'),
      'base' => 'node_content',
      'description' => t('A <em>Hats</em> is a content type which contain product variations.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'shoes' => array(
      'name' => t('Shoes'),
      'base' => 'node_content',
      'description' => t('A <em>Shoes</em> is a content type which contain product variations.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'storage_devices' => array(
      'name' => t('Storage Devices'),
      'base' => 'node_content',
      'description' => t('A <em>Storage Devices</em> is a content type which contain product variations.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'tops' => array(
      'name' => t('Tops'),
      'base' => 'node_content',
      'description' => t('A <em>Tops</em> is a content type which contain product variations.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
