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
  $rule->label = 'Calculate Taxes with Exactor CALC';
  $rule->requires = array(
    'rules',
    'commerce_exactor_calc',
    'entity',
  );
  $rule->event('commerce_order_presave');

  $rule_status = rules_or();

  $rule_status->condition(rules_and()
    ->condition('data_is', array(
      'data:select' => 'commerce-order:status',
      'op' => 'IN',
      'value' => array(
        'checkout_review' => 'checkout_review',
      ),
    ))
    ->condition(rules_condition('data_is', array(
      'data:select' => 'commerce-order-unchanged:status',
      'op' => 'IN',
      'value' => array(
        'checkout_review' => 'checkout_review',
      ),
    ))->negate()));

  $rule_status->condition(rules_and()
    ->condition('data_is', array(
      'data:select' => 'commerce-order:state',
      'op' => 'IN',
      'value' => array(
        'pending' => 'pending',
        'completed' => 'completed',
      ),
    ))
    ->condition(rules_condition('data_is', array(
      'data:select' => 'commerce-order-unchanged:state',
      'op' => 'IN',
      'value' => array(
        'checkout' => 'checkout',
      ),
    ))->negate()));

  $rule->condition($rule_status);
  $rule->condition('entity_has_field', array('entity:select' => 'commerce-order', 'field' => 'commerce_customer_billing'));
  $rule->condition('entity_has_field', array('entity:select' => 'commerce-order:commerce-customer-billing', 'field' => 'commerce_customer_address'));

  // If the shipping module is enabled use the shipping profile.
  if (module_exists('commerce_shipping')) {
    $rule->condition('entity_has_field', array('entity:select' => 'commerce-order', 'field' => 'commerce_customer_shipping'));
    $rule->condition('entity_has_field', array('entity:select' => 'commerce-order:commerce-customer-shipping', 'field' => 'commerce_customer_address'));

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
  $rule->label = 'Commit with Exactor CALC';
  $rule->requires = array(
    'rules',
    'commerce_exactor_calc',
    'entity',
  );
  $rule->event('commerce_order_update');
  $rule->condition('data_is', array('data:select' => 'commerce-order:state', 'value' => 'completed'));
  $rule->condition(rules_condition('data_is', array('data:select' => 'commerce-order-unchanged:state', 'value' => 'completed'))->negate());
  $rule->action('commerce_exactor_calc_commit_request', array('order:select' => 'commerce-order'));

  $configs['commerce_exactoclac_commit_order'] = $rule;

  // Refund Order.
  $rule = rules_reaction_rule();
  $rule->label = 'Refund order with Exactor CALC';
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

  // Return to Cart.
  $rule = rules_reaction_rule();
  $rule->label = 'Remove Exactor on Returning to Cart from Checkout';
  $rule->requires = array(
    'rules',
    'commerce_exactor_calc',
    'entity',
  );
  $rule->event('commerce_order_presave');
  $rule->condition(rules_condition('data_is', array('data:select' => 'commerce-order:status', 'value' => 'cart')));
  $rule->condition(rules_condition('data_is', array('data:select' => 'commerce-order-unchanged:status', 'value' => 'cart'))->negate());
  $rule->action('commerce_exactor_calc_delete_lines', array('order:select' => 'commerce-order'));

  $configs['rules_remove_exactor_on_returning_to_cart_from_checkout'] = $rule;

  return $configs;
}
