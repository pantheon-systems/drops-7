<?php

/**
 * @file
 * Provides metadata for the entity.
 */

/*
 * Implements hook_entity_property_info_alter().
 */
function commerce_kiala_entity_property_info_alter(&$info) {
  // Commerce Line item
  if (isset($info['commerce_line_item'])) {
    // Line item bundles
    if (isset($info['commerce_line_item']['bundles'])) {
      // Shipping line items
      if (isset($info['commerce_line_item']['bundles']['shipping']['properties'])) {
        $info['commerce_line_item']['bundles']['shipping']['properties']['kiala_point'] = array(
          'label' => t('Kiala Point'),
          'type' => 'struct',
          'description' => t('The Kiala Point selected for this shipping line item.'),
          'getter callback' => 'commerce_kiala_line_item_property_get',
          'property info' => commerce_kiala_line_item_point_property_info_callback(),
          'computed' => TRUE,
        );
      }
    }
  }

  // Commerce Order
  if (isset($info['commerce_order']) && isset($info['commerce_order']['properties'])) {
    $order_properties = &$info['commerce_order']['properties'];
    $order_properties['kiala_point'] = array(
      'label' => t('Kiala Point'),
      'type' => 'struct',
      'description' => t('The Kiala Point selected for this order.'),
      'getter callback' => 'commerce_kiala_order_property_get',
      'property info' => commerce_kiala_line_item_point_property_info_callback(),
      'computed' => TRUE,
    );
    $order_properties['kiala_point_id'] = array(
      'label' => t('Kiala Point ID'),
      'type' => 'integer',
      'description' => t('The Kiala Point ID selected for this order.'),
      'getter callback' => 'commerce_kiala_order_property_get',
      'computed' => TRUE,
    );
    $order_properties['kiala_point_url'] = array(
      'label' => t('Kiala Point details map url'),
      'type' => 'uri',
      'description' => t('The url to the local Kiala Point details map.'),
      'getter callback' => 'commerce_kiala_order_property_get',
      'computed' => TRUE,
    );
    $order_properties['kiala_point_remote_url'] = array(
      'label' => t('Kiala Point details map url (Remote)'),
      'type' => 'uri',
      'description' => t('The url to the remote Kiala Point details map.'),
      'getter callback' => 'commerce_kiala_order_property_get',
      'computed' => TRUE,
    );
    $order_properties['kiala_point_remote_search_url'] = array(
      'label' => t('Kiala Point search map url (Remote)'),
      'type' => 'uri',
      'description' => t('The url to the remote Kiala Point search map based on the recipient\'s address.'),
      'getter callback' => 'commerce_kiala_order_property_get',
      'computed' => TRUE,
    );
  }
}
