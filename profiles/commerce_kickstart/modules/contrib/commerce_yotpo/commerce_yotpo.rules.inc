<?php
/**
 * @file
 * commerce_yotpo.rules.inc
 */

/**
 * Implements hook_rules_action_info().
 */
function commerce_yotpo_rules_action_info() {
  $actions = array();

  $actions['commerce_yotpo_create_purchase'] = array(
    'label' => t('Yotpo create purchase'),
    'parameter' => array(
      'commerce_order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
        'save' => TRUE,
      ),
    ),
    'group' => t('Commerce Yotpo'),
    'callbacks' => array(
      'execute' => 'commerce_yotpo_create_purchase',
    ),
  );

  return $actions;
}
