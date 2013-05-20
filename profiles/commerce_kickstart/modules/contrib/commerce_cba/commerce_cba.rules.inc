<?php
/**
 * @file
 *
 * Rules integration for Commerce checkout by Amazon.
 */

/**
 * Implements hook_rules_condition_info().
 */
function commerce_cba_rules_condition_info() {
  $conditions = array();

  $conditions['commerce_cba_amazon_contract'] = array(
    'label' => t('Contract from Amazon is set in a Cookie'),
    'description' => t('Returns TRUE if there\'s a purchase contract id in the cookies, FALSE otherwise. Used to check if the checkout is being processed through checkout by Amazon'),
    'group' => t('Commerce Checkout by Amazon'),
    'parameter' => array(),
    'callbacks' => array(
      'execute' => 'commerce_cba_rules_amazon_contract',
    ),
  );
  $conditions['commerce_cba_amazon_wallet'] = array(
    'label' => t('User has selected a payment method from the Amazon Wallet widget'),
    'description' => t('Checks if the user has selected one of the payment methods available in the wallet'),
    'group' => t('Commerce Checkout by Amazon'),
    'parameter' => array(
      'commerce_order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
      ),
    ),
    'callbacks' => array(
      'execute' => 'commerce_cba_rules_amazon_wallet',
    ),
  );

  $conditions['commerce_cba_amazon_address'] = array(
    'label' => t('User has selected an address from the Amazon Address widget'),
    'description' => t('Checks if the user has selected an address for a given customer profile'),
    'group' => t('Commerce Checkout by Amazon'),
    'parameter' => array(
      'commerce_order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
      ),
      'commerce_customer' => array(
        'type' => 'commerce_customer_profile',
        'label' => t('Customer profile'),
      ),
    ),
    'callbacks' => array(
      'execute' => 'commerce_cba_rules_amazon_address',
    ),
  );

  return $conditions;
}

function commerce_cba_rules_amazon_contract() {
  $purchase_contract_id = commerce_cba_get_purchase_contract_id();
  return isset($purchase_contract_id);
}

function commerce_cba_rules_amazon_wallet($order) {
  if (!isset($order->data['commerce_cba'])) {
    return FALSE;
  }
  return isset($order->data['commerce_cba']['wallet']);
}

function commerce_cba_rules_amazon_address($order, $customer_profile) {
  if (!isset($order->data['commerce_cba'])) {
    return FALSE;
  }
  return isset($order->data['commerce_cba'][$customer_profile->type]);
}
