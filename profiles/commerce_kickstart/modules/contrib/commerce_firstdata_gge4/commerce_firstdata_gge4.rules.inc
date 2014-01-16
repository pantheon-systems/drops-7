<?php

/**
 * @file
 * Rules integration for First Data payments.
 *
 * @addtogroup rules
 * @{
 */

/**
 * Implements hook_rules_action_info().
 */
function commerce_firstdata_gge4_rules_action_info() {
  $actions = array();

  $actions['commerce_firstdata_gge4_transaction_operate'] = array(
    'label' => t('Commerce First Data GGe4: Operate on an existing Payment Transaction'),
    'group' => t('Commerce Payment'),
    'parameter' => array(
      'transaction' => array(
        'type' => 'commerce_payment_transaction',
        'label' => t('Payment Transaction'),
        'description' => t('The transaction must have a payment method that is controlled by the Commerce First Data GGe4 module. If not, then no operation is performed.'),
        'save' => FALSE,
      ),
      'txn_type' => array(
        'type' => 'text',
        'label' => t('Operation'),
        'description' => t('This operation will be performed on the selected payment transaction for the same amount as the transaction.'),
        'options list' => 'commerce_firstdata_gge4_transaction_operation_options',
        'restriction' => 'input',
      ),
    ),
    'provides' => array(
      'saved_transaction' => array(
        'type' => 'commerce_payment_transaction',
        'label' => t('Saved Payment Transaction'),
        'description' => t('The transaction that was saved due to the operation.  This could be one of the following: a new transaction (ex. for a credit), the updated selected transaction, (ex. for an auth capture), or the selected transaction unchanged if an unexpected error occurs.'),
        'save' => FALSE,
      ),
    ),
    'callbacks' => array(
      'execute' => 'commerce_firstdata_gge4_rules_action_transaction_operate',
    ),
  );


  return $actions;
}

/**
 * Action callback: commerce_firstdata_gge4_transaction_operate
 */
function commerce_firstdata_gge4_rules_action_transaction_operate($transaction, $txn_type) {
  // DENY if not First Data GGe4
  if (!_commerce_firstdata_gge4_check_payment_method($transaction->payment_method)) {
    return;
  }

  // DENY if no remote id
  if (empty($transaction->remote_id)) {
    return;
  }

  // Load payment method for settings
  $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  if (empty($payment_method)) {
    return;
  }

  // Initialize API controller
  $gateway = commerce_firstdata_gge4_get_controller($payment_method['settings']);

  // DENY if no web service
  if (!$gateway->isActive('web_service')) {
    return;
  }
  
  // Build request state
  $charge = array(
    'amount' => abs($transaction->amount),
    'currency_code' => $transaction->currency_code,
  );
  
  $request_state = array(
    'txn_type' => $txn_type,
    'charge' => $charge,
    'previous_transaction' => $transaction,
    'payment_method' => $payment_method,
  );

  $response = $gateway->get('web_service')->request($request_state);
  if (empty($response['transaction_approved'])) {
    // Display an error message but leave the transaction pending.
    $error_messages = $gateway->get('web_service')->getErrorMessages($response);
    watchdog('commerce_firstdata_gge4', 'Request errors occurred during rules action execution: @error_message', array('@error_message' => implode('; ', $error_messages)), WATCHDOG_ERROR);
  }

  // A saved transaction exists for success or failure,
  // else fallback to the passed transaction
  return array(
    'saved_transaction' => isset($request_state['transaction']) ? $request_state['transaction'] : $transaction,
  );
}

/**
 * @}
 */
