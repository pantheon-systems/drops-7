<?php
/**
 * @file
 *   commerce_exactor_euc.views.inc
 */

/**
 * Impliments hook_views_data_alter().
 */
function commerce_exactor_euc_views_data_alter(&$data) {
  // Expose the product EUC.
  $data['commerce_product']['exactor_euc'] = array(
    'title' => t('Exactor EUC'),
    'help' => t('The Exactor Unified Code for the product.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
}
