<?php

/**
 * @file
 * Implementations of Commerce Hooks
 */


/**
 * Implements hook_commerce_kiala_settings_info().
 */
function commerce_kiala_ps_commerce_kiala_settings_info() {
  return array(
    'packship_platform_originator' => array(
      'group' => 'packship',
      'default' => 'CG_1.0',
    ),
    'packship_plugin_number_prefix' => array(
      'group' => 'packship',
      'default' => 'CG_1.0_',
    ),
    'packship_type' => array(
      'group' => 'packship',
      'default' => 'desktop',
    ),
    'packship_ws_sender_id' => array(
      'group' => 'packship',
      'default' => '',
      'sensitive' => TRUE,
    ),
    'packship_ws_password' => array(
      'group' => 'packship',
      'default' => '',
      'sensitive' => TRUE,
      'password' => TRUE,
    ),
    'packship_ws_url' => array(
      'group' => 'packship',
      'default' => 'http://packandship-ws.kiala.com/psws/order?wsdl',
    ),
    'packship_ws_max_weight' => array(
      'group' => 'packship',
      'default' => '',
    ),
    'packship_ws_params' => array(
      'group' => 'packship',
      'default' => array(
      ),
    ),
  );
}
