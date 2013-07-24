<?php
/**
 * Implements hook_rules_action_info().
 */
function commerce_payleap_rules_action_info() {
  $items = array();
  $items['commerce_payleap_capture_pending'] = array(
    'label' => t('Force capture on authorisation transactions.'),
    'group' => t('Commerce PayLeap'),
    'parameter' => array(
      'order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
        'description' => t('The order to process, all pending transaction will be processed.'),
      ),
      'transaction' => array(
        'type' => 'commerce_transaction',
        'label' => t('Transaction'),
        'description' => t('The pending transaction, specify a single transaction to process.'),
        'optional' => TRUE,
      ),
    ),
    'base' => 'commerce_payleap_rules_capture_pending',
    'access callback' => 'commerce_payleap_rules_integration_access',
  );

  $items['commerce_payleap_cancel_transaction'] = array(
    'label' => t('Cancel/Refund transaction'),
    'group' => t('Commerce PayLeap'),
    'parameter' => array(
      'order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
        'description' => t('The original order to cancel transactions for.'),
      ),
      'transaction' => array(
        'type' => 'commerce_transaction',
        'label' => t('Transaction'),
        'description' => t('The transaction to cancel, if this field is set only this transaction will be  canceled'),
        'optional' => TRUE,
      ),
    ),
    'base' => 'commerce_payleap_rules_cancel_capture',
    'access callback' => 'commerce_payleap_rules_integration_access',
  );
  return $items;
}

/**
 * PayLeap Rules integration access callback.
 */
function commerce_payleap_rules_integration_access($type, $name) {
  // Grant everyone access to conditions.
  return $type == 'condition' || user_access('administer orders');
}


/**
 * Action: Capture a pending transaction.
 */
function commerce_payleap_rules_capture_pending($order, $transaction = FALSE) {
  if ($transaction) {
    $payments = array();
    $payments[0]->transaction_id = $transaction->transaction_id;
    $payments[0]->commerce_payment_transaction_status = $transaction->transaction_id;
  }
  else {
    $payments =  views_get_view_result('commerce_payment_order', 'defaults', $order->order_id);
  }

  // Process all transactions.
  foreach ($payments as $payment) {
    // Process only pending transactions.
    if ($payment->commerce_payment_transaction_status != COMMERCE_PAYMENT_STATUS_PENDING) {
      continue;
    }
    $wrapper = entity_metadata_wrapper('commerce_payment_transaction', $payment->transaction_id);
    $transaction = $wrapper->value();
    $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
    $amount = commerce_currency_amount_to_decimal($transaction->amount, $transaction->currency_code);
    $result = commerce_payleap_transaction_request($transaction, $payment_method, $amount, 'Force');
  }
}

/**
 * Action: Cancel a pending transaction.
 */
function commerce_payleap_rules_cancel_capture($order, $transaction = FALSE) {
  if ($transaction) {
    $payments = array();
    $payments[0]->transaction_id = $transaction->transaction_id;
    $payments[0]->commerce_payment_transaction_status = $transaction->transaction_id;
  }
  else {
    $payments =  views_get_view_result('commerce_payment_order', 'defaults', $order->order_id);
  }

  // Process all transactions.
  foreach ($payments as $payment) {
    $wrapper = entity_metadata_wrapper('commerce_payment_transaction', $payment->transaction_id);
    $transaction = $wrapper->value();
    $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
    $amount = commerce_currency_amount_to_decimal($transaction->amount, $transaction->currency_code);
    $result = commerce_payleap_transaction_request($transaction, $payment_method, $amount, 'Void');
  }
}

