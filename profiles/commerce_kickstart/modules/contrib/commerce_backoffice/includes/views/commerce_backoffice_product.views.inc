<?php

/**
 * Implements hook_views_data_alter().
 */
function commerce_backoffice_product_views_data_alter(&$data) {
  $vocabularies_to_fetch = array();
  foreach (field_info_instances('commerce_product') as $bundle => $instances) {
    foreach ($instances as $field_name => $instance) {
      $field = field_info_field($field_name);
      if ($field['type'] == 'taxonomy_term_reference') {
        $vocabularies_to_fetch[$field['settings']['allowed_values'][0]['vocabulary']] = $field_name;
      }
    }
  }
  $data['views_entity_commerce_product']['terms'] = array(
    'title' => t('All taxonomy terms'),
    'help' => t('Display all taxonomy terms associated with a product.'),
    'field' => array(
      'handler' => 'commerce_backoffice_handler_field_term_entity_tid',
      'no group by' => TRUE,
      'vocabularies_to_fetch' => $vocabularies_to_fetch,
    ),
  );
  $data['commerce_product']['quick_edit_form'] = array(
    'real field' => 'product_id',
    'field' => array(
      'title' => t('Quick Edit Form'),
      'help' => t('Display a form for editing the price and status of a variation.'),
      'handler' => 'commerce_backoffice_product_handler_field_product_quick_edit_form',
    ),
  );

  $data['views_entity_node']['operations_dropbutton'] = array(
    'field' => array(
      'title' => t('Operations links (Dropbutton)'),
      'help' => t('Display the available operations links for the node in a dropbutton.'),
      'handler' => 'commerce_backoffice_handler_field_node_operations',
    ),
  );
  $data['node']['product_display_term_node_tid_multiple'] = array(
    'title' => 'Product Display Has taxonomy terms (Multiple)',
    'help' => 'Display content if it has one of (or all of) the selected terms.',
    'filter' => array(
      'handler' => 'commerce_backoffice_handler_filter_term_node_tid',
      'product display' => TRUE,
    ),
  );
}
