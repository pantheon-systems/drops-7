<?php
/**
 * @file
 * commerce_exactor_euc.info.inc
 */

/**
 * Implements hook_entity_property_info_alter().
 */
function commerce_exactor_euc_entity_property_info_alter(&$info) {

  $info['commerce_product']['properties']['exactor_euc'] = array(
    'label' => t('Exactor EUC'),
    'description' => t('The Exactor Unified Code'),
    'type' => 'text',
    'options list' => 'commerce_exactor_euc_options_list',
    'setter callback' => 'entity_property_verbatim_set',
    'setter permission' => 'administer commerce_product entities',
    'required' => TRUE,
    'schema field' => 'exactor_euc',
  );

}
