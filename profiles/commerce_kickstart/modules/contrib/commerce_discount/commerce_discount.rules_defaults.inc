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
  foreach (entity_load('commerce_discount') as $discount) {
    $wrapper = entity_metadata_wrapper('commerce_discount', $discount);
    $wrapper_properties = $wrapper->getPropertyInfo();
    // Only for Commerce Discount wrappers with Commerce Discount Offer defined.
    if (in_array('commerce_discount_offer', array_keys($wrapper_properties))) {
      $type = $types[$discount->type];
      $offer_type = $offer_types[$wrapper->commerce_discount_offer->getBundle()];

      $rule = rules_reaction_rule();

      $rule->label = $discount->label;
      $rule->active = $discount->status;
      $rule->tags = array('Commerce Discount', check_plain($type['label']));

      $rule
        ->event(!empty($offer_type['event']) ? $offer_type['event'] : $type['event'])
        ->action(
          $offer_type['action'], array(
            'entity:select' => $type['entity type'],
            'commerce_discount' => $discount->name,
          )
        );

      // Let other modules alter the rule object, with configuration specific
      // to commerce discount. We don't invoke an alter function, as it can
      // be already achieved by implementing
      // hook_default_rules_configuration_alter().
      module_invoke_all('commerce_discount_rule_build', $rule, $discount);

      // Let inline_conditions fields add their own conditions.
      foreach ($wrapper_properties as $field_name => $property) {
        if (stripos($property['type'], 'inline_conditions') !== FALSE) {
          inline_conditions_build($rule, $wrapper->$field_name->value());
        }
      }

      // Add the commerce discount to the rule configuration, so other may act
      // according to it, in hook_default_rules_configuration_alter().
      $rule->commerce_discount = $discount;

      $rules['commerce_discount_rule_' . $discount->name] = $rule;
    }
  }

  return $rules;
}
