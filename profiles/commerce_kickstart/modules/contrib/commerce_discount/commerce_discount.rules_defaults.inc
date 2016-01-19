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
  entity_get_controller('commerce_discount')->resetCache();

  foreach (entity_load('commerce_discount') as $discount) {
    $wrapper = entity_metadata_wrapper('commerce_discount', $discount);
    $wrapper_properties = $wrapper->getPropertyInfo();
    // Only for Commerce Discount wrappers with Commerce Discount Offer defined.
    if (isset($wrapper->commerce_discount_offer)) {
      $offer_bundle = $wrapper->commerce_discount_offer->getBundle();
      if (!isset($offer_types[$offer_bundle])) {
        continue;
      }

      $type = $types[$discount->type];
      $offer_type = $offer_types[$offer_bundle];

      $rule = rules_reaction_rule();

      $rule->label = $discount->label;
      $rule->active = $discount->status;
      $rule->weight = !empty($discount->sort_order) ? $discount->sort_order - 11 : -1;
      $rule->tags = array('Commerce Discount', check_plain($type['label']));

      $rule
        ->event(!empty($offer_type['event']) ? $offer_type['event'] : $type['event'])
        ->action(
          $offer_type['action'], array(
            'entity:select' => $type['entity type'],
            'commerce_discount' => $discount->name,
          )
        );

      // Add the compatibility condition to all discounts. Even if the current
      // discount is compatible with any other discount, we need to prevent it
      // from being added if a previously applied discount indicates it is not
      // compatible with the current one.
      $rule->condition('commerce_discount_compatibility_check', array(
        'commerce_order:select' => $type['entity type'] == 'commerce_order' ? 'commerce-order' : 'commerce-line-item:order',
        'commerce_discount' => $discount->name,
      ));

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

      $rule_machine_name = 'commerce_discount_rule_' . $discount->name;
      // Ensure the name isn't too long.
      if (strlen($rule_machine_name) > 64) {
        // Shorten the name but ensure uniqueness by using a hash.
        $hash = crc32($discount->name);
        $rule_machine_name = substr($rule_machine_name, 0, 64 - strlen($hash) - 1) . '_' . $hash;
      }
      $rules[$rule_machine_name] = $rule;
    }
  }

  return $rules;
}
