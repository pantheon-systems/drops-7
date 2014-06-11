<?php
/**
 * @file
 * commerce_yotpo.rules_defaults.inc
 */

/**
 * Implements hook_default_rules_configuration().
 */
function commerce_yotpo_default_rules_configuration() {
  // Post Order.
  $rule = rules_reaction_rule();
  $rule->label = 'Yotpo: Mail after purchase';
  $rule->requires = array(
    'rules',
    'commerce_yotpo',
    'entity',
  );
  $rule->event('commerce_order_update');
  $rule->condition('data_is', array('data:select' => 'commerce-order:state', 'value' => 'completed'));
  $rule->condition(rules_condition('data_is', array('data:select' => 'commerce-order-unchanged:state', 'value' => 'completed'))->negate());
  $rule->action('commerce_yotpo_create_purchase', array('order:select' => 'commerce-order'));

  $configs['commerce_yotpo_create_purchase'] = $rule;

  return $configs;
}
