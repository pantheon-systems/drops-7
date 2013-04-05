<?php

/**
 * @file
 * Default rule configurations for commerce discount.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function commerce_discount_default_rules_configuration() {
  $types = commerce_discount_types();
  $offer_types = commerce_discount_offer_types();

  $rules = array();

  // Create a rule for each commerce discount entity.
  foreach(entity_load('commerce_discount') as $discount) {
    $rule = rules_reaction_rule();

    $rule->label = $discount->label;
    $rule->active = $discount->enabled;

    $type = $types[$discount->type];
    $wrapper = entity_metadata_wrapper('commerce_discount', $discount->name);
    $offer_type = $offer_types[$wrapper->commerce_discount_offer->getBundle()];
    $rule
      ->event($type['event'])
      ->action($offer_type['action'], array(
        'entity:select' => $type['entity type'],
        'commerce_discount' => $discount->name,
      ));

    // Let other modules alter the rule object, with configuration specifc
    // to commerce discount. We don't invoke an alter function, as it can
    // be already acheived by implementing
    // hook_default_rules_configuration_alter().
    module_invoke_all('commerce_discount_rule_build', $rule, $discount);

    // Add the commerce discount to the rule configuration, so other may act
    // according to it, in hook_default_rules_configuration_alter().
    $rule->commerce_discount = $discount;

    $rules['commerce_discount_rule_' . $discount->name] = $rule;
  }
  return $rules;
}
