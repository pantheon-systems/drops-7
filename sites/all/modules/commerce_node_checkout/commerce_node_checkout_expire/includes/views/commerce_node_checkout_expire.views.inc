<?php
/**
 * @file
 *   Provide Views integration.
 */

/**
 * Implements hook_views_data_alter().
 */
function commerce_node_checkout_expire_views_data_alter(&$data) {
  $data['node']['node_relist_form']['moved to'] = array('views_entity_node', 'node_relist_form');
  $data['views_entity_node']['node_relist_form'] = array(
    'field' => array(
      'title' => t('Node relist form'),
      'help' => t('Display the form to relist this node.'),
      'handler' => 'commerce_node_checkout_expire_field_node_relist_form',
    ),
  );
}

