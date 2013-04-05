<?php

/**
 * Implements hook_views_data_alter().
 */
function commerce_backoffice_order_views_data_alter(&$data) {
  $data['views_entity_commerce_order']['operations_dropbutton'] = array(
    'field' => array(
      'title' => t('Operations links (Dropbutton)'),
      'help' => t('Display the available operations links for the order in a dropbutton.'),
      'handler' => 'commerce_backoffice_order_handler_field_order_operations',
    ),
  );
}
