<?php

/**
 * @file
 * Default rules configuration for Commerce Kiala Tracking.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function commerce_kiala_ps_default_rules_configuration() {
  $rules = array();

  $rule = rules_reaction_rule();
  $rule->label = t('Submit order to Kiala P&S web service after checkout');
  $rule->tags = array('commerce_kiala_ps');

/** @todo set to active by default once web service has been tested ******/
  $rule->active = FALSE;

  $rule->weight = 10; // trigger after others
  $rule->event('commerce_checkout_complete')
    ->condition(rules_condition('data_is_empty', array(
      'data:select' => 'commerce-order:kiala_packship_ws_access',
    ))->negate())
    ->action('commerce_kiala_ps_rules_ws_ping_create_order', array(
      'order:select' => 'commerce-order',
    ));

  $rules['commerce_kiala_ps_ws_submit_checkout_order'] = $rule;

  return $rules;
}
