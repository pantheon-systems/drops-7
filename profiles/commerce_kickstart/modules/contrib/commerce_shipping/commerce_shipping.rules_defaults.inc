<?php

/**
 * @file
 * Default rules configurations for Shipping.
 */


/**
 * Implements hook_default_rules_configuration().
 */
function commerce_shipping_default_rules_configuration() {
  $rules = array();

  // When an order's status is updating to "Shopping cart" from any other status,
  // its shipping line items will be deleted. This captures any cart contents
  // change via Add to Cart forms / the Shopping cart Views form. It also happens
  // when the customer cancels out of the checkout form.
  $rule = rules_reaction_rule();

  $rule->label = t('Delete shipping line items on shopping cart updates');
  $rule->active = TRUE;

  $rule
    ->event('commerce_order_update')
    ->condition('data_is', array(
      'data:select' => 'commerce-order:status',
      'op' => '==',
      'value' => 'cart',
    ))
    ->condition(rules_condition('data_is', array(
      'data:select' => 'commerce-order-unchanged:status',
      'op' => '==',
      'value' => 'cart',
    ))->negate())
    ->action('commerce_shipping_delete_shipping_line_items', array(
      'commerce_order:select' => 'commerce-order',
    ));

  $rules['commerce_shipping_cart_update_delete'] = $rule;

  // Loop over every shipping service looking for services requiring a default
  // component.
  foreach (commerce_shipping_services() as $name => $shipping_service) {
    if ($shipping_service['rules_component']) {
      // Create a new rules component.
      $rule = rule(commerce_shipping_service_component_variables());

      $rule->label = t('Rate @title', array('@title' => $shipping_service['title']));

      // Add the action to rate the current service.
      $rule
        ->action('commerce_shipping_service_rate_order', array(
          'shipping_service_name' => $name,
          'commerce_order:select' => 'commerce-order',
        ));

      $rules['commerce_shipping_service_' . $name] = $rule;
    }
  }

  // Loop over every shipping method and define a rule to collect rates for
  // shipping services assigned to that method.
  foreach (commerce_shipping_methods() as $name => $shipping_method) {
    // Create a new rule.
    $rule = rules_reaction_rule();

    $rule->label = t('Collect rates: @title', array('@title' => $shipping_method['title']));
    $rule->active = $shipping_method['active'];

    // Add the action to invoke every shipping service's component matching this
    // shipping method.
    $rule
      ->event('commerce_shipping_collect_rates')
      ->action('commerce_shipping_method_collect_rates', array(
          'shipping_method_name' => $name,
          'commerce_order:select' => 'commerce-order',
        ));

    $rules['commerce_shipping_method_' . $name] = $rule;
  }

  return $rules;
}

/**
 * Returns an array of variables for use in shipping service components.
 */
function commerce_shipping_service_component_variables() {
  return array(
    'commerce_order' => array(
      'type' => 'commerce_order',
      'label' => t('Order'),
    ),
  );
}
