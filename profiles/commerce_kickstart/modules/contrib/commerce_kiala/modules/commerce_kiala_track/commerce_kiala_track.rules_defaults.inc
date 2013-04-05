<?php

/**
 * @file
 * Default rules configuration for Commerce Kiala Tracking.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function commerce_kiala_track_default_rules_configuration() {
  $rules = array();

  $rule = rules_reaction_rule();
  $rule->label = t('Update Kiala tracking number on pack and ship log insert');
  $rule->tags = array('commerce_kiala_track');
  $rule->active = TRUE;
  $rule->event('commerce_kiala_ps_ws_log_insert')
    ->condition(rules_condition('data_is_empty', array(
      'data:select' => 'commerce-kiala-ps-ws-log:order-id',
    ))->negate())
    ->condition('data_is', array(
      'data:select' => 'commerce-kiala-ps-ws-log:method',
      'value' => 'createOrder',
    ))
    ->condition('data_is', array(
      'data:select' => 'commerce-kiala-ps-ws-log:status',
      'value' => 'success',
    ))
    ->condition(rules_condition('data_is_empty', array(
      'data:select' => 'commerce-kiala-ps-ws-log:tracking-number',
    ))->negate())
    ->action('commerce_kiala_track_set_order_tracking', array(
      'order:select' => 'commerce-kiala-ps-ws-log:order',
      'tracking_number:select' => 'commerce-kiala-ps-ws-log:tracking-number',
    ));

  $rules['commerce_kiala_track_update_by_ps_ws'] = $rule;

  return $rules;
}
