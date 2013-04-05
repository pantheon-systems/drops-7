<?php

/*
 * Implements hook_entity_property_info_alter().
 */
function commerce_kiala_track_entity_property_info_alter(&$info) {
  // Put the kiala tracking info on the order
  if (isset($info['commerce_order'])) {
    $info['commerce_order']['properties']['kiala_tracking_object'] = array(
      'type' => 'struct',
      'label' => t('Kiala tracking number'),
      'getter callback' => 'commerce_kiala_track_order_property_getter',
      'property info' => array(
        'tracking_number' => array(
          'type' => 'text',
          'label' => t('Tracking number'),
          'getter callback' => 'commerce_kiala_track_order_property_getter',
        ),         
      )
    );
  }
}