<?php
/**
 * @file
 * commerce_exactor_calc.rules_defaults.inc
 */

/**
 * Implements hook_default_rules_configuration().
 */
function commerce_exactor_calc_default_rules_configuration() {

  // Calculate Tax.
  $rule = rules_reaction_rule();
  $rule->label = 'Calculate Taxes with ExactoCALC';
  $rule->requires = array(
    'rules',
    'commerce_exactor_calc',
    'entity',
  );
  $rule->event('commerce_order_presave');

  $rule_status = rules_or()
   ->condition('data_is', array(
     'data:select' => 'commerce-order:status',
     'op' => 'IN',
     'value' => array(
       'checkout_checkout' => 'checkout_checkout',
       'checkout_shipping' => 'checkout_shipping',
       'checkout_review' => 'checkout_review',
     ))
   )
   ->condition('data_is', array(
     'data:select' => 'commerce-order:state',
     'op' => 'IN',
     'value' => array(
       'pending' => 'pending',
       'cart' => 'cart',
     ))
   );

  $rule->condition($rule_status);
  $rule->condition('entity_has_field', array('entity:select' => 'commerce-order', 'field' => 'commerce_customer_billing'));
  $rule->condition('entity_has_field', array('entity:select' => 'commerce-order:commerce-customer-billing', 'field' => 'commerce_customer_address'));
  $rule->condition(rules_condition('data_is_empty', array('data:select' => 'commerce-order:commerce-customer-billing:commerce-customer-address:country'))->negate());

  // If the shipping module is enabled use the shipping profile.
  if (module_exists('commerce_shipping')) {
    $rule->condition('entity_has_field', array('entity:select' => 'commerce-order', 'field' => 'commerce_customer_shipping'));
    $rule->condition('entity_has_field', array('entity:select' => 'commerce-order:commerce-customer-shipping', 'field' => 'commerce_customer_address'));
    $rule->condition(rules_condition('data_is_empty', array('data:select' => 'commerce-order:commerce-customer-shipping:commerce-customer-address:country'))->negate());

    $rule->action('commerce_exactor_calc_invoice_request', array(
      'order:select' => 'commerce-order',
      'billing_address:select' => 'commerce-order:commerce-customer-billing:commerce-customer-address',
      'shipping_address:select' => 'commerce-order:commerce-customer-shipping:commerce-customer-address',
    ));
  }

  // If shipping is not enabled use the billing address
  // for both billing and shipping.
  else {
    $rule->action('commerce_exactor_calc_invoice_request', array(
      'order:select' => 'commerce-order',
      'billing_address:select' => 'commerce-order:commerce-customer-billing:commerce-customer-address',
      'shipping_address:select' => 'commerce-order:commerce-customer-billing:commerce-customer-address',
    ));
  }

  $configs['commerce_exactoclac_calculate_tax'] = $rule;

  // Commit Order.
  $rule = rules_reaction_rule();
  $rule->label = 'Commit with ExactoCALC';
  $rule->requires = array(
    'rules',
    'commerce_exactor_calc',
    'entity',
  );
  $rule->event('commerce_order_presave');
  $rule->condition('data_is', array('data:select' => 'commerce-order:state', 'value' => 'completed'));
  $rule->condition(rules_condition('data_is', array('data:select' => 'commerce-order-unchanged:state', 'value' => 'completed'))->negate());
  $rule->action('commerce_exactor_calc_commit_request', array('order:select' => 'commerce-order'));

  $configs['commerce_exactoclac_commit_order'] = $rule;

  // Refund Order.
  $rule = rules_reaction_rule();
  $rule->label = 'Refund order with ExactoCALC';
  $rule->requires = array(
    'rules',
    'commerce_exactor_calc',
    'entity',
  );
  $rule->event('commerce_order_presave');
  $rule->condition(rules_condition('data_is', array('data:select' => 'commerce-order:status', 'value' => 'completed'))->negate());
  $rule->condition(rules_condition('data_is', array('data:select' => 'commerce-order-unchanged:status', 'value' => 'completed')));
  $rule->action('commerce_exactor_calc_refund_request', array('order:select' => 'commerce-order'));

  $configs['commerce_exactor_calc_refund_order'] = $rule;

  return $configs;
}
