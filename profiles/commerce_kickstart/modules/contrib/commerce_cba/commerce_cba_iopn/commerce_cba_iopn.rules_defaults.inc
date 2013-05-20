<?php
/**
 * @file
 *
 * Default rules shipped by IOPN checkout by Amazon module.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function commerce_cba_iopn_default_rules_configuration() {
  $rules = array();

  $rule = rules_reaction_rule();
  $rule->label = t('New order notification received from Amazon');
  $rule->active = TRUE;

  $rule
    ->event('commerce_cba_iopn_new_order_notification')
    ->action('commerce_cba_iopn_save_customer_profile', array(
      'commerce_order:select' => 'commerce-order',
      'customer_profile_field' => 'billing',
      'shipping_address:select' => 'shipping-address',
      'provides' => array('commerce_customer_profile', 'Customer profile type'),
    ))
    ->action('commerce_order_update_status', array(
      'commerce_order:select' => 'commerce-order',
      'order_status' => 'amazon_new',
    ));

  $rules['commerce_cba_iopn_new_order_notification'] = $rule;

  $rule = rules_reaction_rule();
  $rule->label = t('Ready to ship order notification received from Amazon');
  $rule->active = TRUE;

  $rule
    ->event('commerce_cba_iopn_rts_order_notification')
    ->action('commerce_order_update_status', array(
      'commerce_order:select' => 'commerce-order',
      'order_status' => 'amazon_rts',
    ));

  $rules['commerce_cba_iopn_rts_order_notification'] = $rule;

  $rule = rules_reaction_rule();
  $rule->label = t('Cancel order notification received from Amazon');
  $rule->active = TRUE;

  $rule
    ->event('commerce_cba_iopn_cancel_order_notification')
    ->action('commerce_order_update_status', array(
      'commerce_order:select' => 'commerce-order',
      'order_status' => 'canceled',
    ));

  $rules['commerce_cba_iopn_cancel_order_notification'] = $rule;




  return $rules;
}
