<?php

/**
 * @file
 * Administrative forms for the First Data Global Gateway e4 module.
 */

/**
 * Transaction operation form
 */
function commerce_firstdata_gge4_transaction_form($form, &$form_state, $order, $transaction, $txn_type) {
  $form_state['order'] = $order;
  $form_state['transaction'] = $transaction;
  $form_state['transaction_type'] = $txn_type;

  // Load and store the payment method instance for this transaction.
  $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  $form_state['payment_method'] = $payment_method;

  // Load the api controller
  $gateway = commerce_firstdata_gge4_get_controller($payment_method['instance_id']);

  // Disable
  if (!$gateway->isActive('web_service')) {
    $form['#disabled'] = TRUE;
    drupal_set_message(t('The web service API is not enabled for this payment method.'), 'error');
  }

  // Get type info
  $txn_type_info = $gateway->transactionType($txn_type);
  $tokens = array(
    '@action' => $txn_type_info['action_word']['present'],
    '@short_label' => $txn_type_info['short_label'],
  );

  // Form with no amount input
  if (empty($txn_type_info['variable_amount'])) {
    $form['markup'] = array(
      '#markup' => t('Are you sure that you want to @action this transaction?', $tokens),
    );

    // Add transaction amount as a positive number
    $form['amount'] = array(
      '#type' => 'value',
      '#value' => commerce_currency_amount_to_decimal(abs($transaction->amount), $transaction->currency_code),
    );

    $form = confirm_form($form,
      t('Are you sure that you want to @action this transaction?', $tokens),
      'admin/commerce/orders/' . $order->order_id . '/payment',
      '',
      t('@action', $tokens),
      t('cancel'),
      'confirm'
    );

    return $form;
  }

  // Form with amount input
  $currency = commerce_currency_load($transaction->currency_code);
  
  // Get order balance
  $order_balance = commerce_payment_order_balance($order);

  // Get max allowed amount per type definitions
  $max = $gateway->transactionMaxAmount($txn_type, $transaction);

  // Balance limit according to ledger type
  $txn_balance = NULL;
  if (isset($txn_type_info['ledger_type']) && $txn_type_info['ledger_type'] == 'credit') {
    // Limit to remaining balance for credit
    $txn_balance = commerce_firstdata_gge4_transaction_balance($transaction);
    if ($txn_balance['amount'] > 0) {
      $max = $txn_balance;
    }
    else {
      $form['#disabled'] = TRUE;
      drupal_set_message(t('The total amount of this transaction has been credited via other transactions.'), 'error');
    }
  }
  else {
    // Limit to remaining order balance
    /** @todo: still need to limit to balance if can capture more than auth ? **/
    if ($order_balance['amount'] > 0 && $order_balance['amount'] < $transaction->amount) {
      $max = $order_balance;
    }
  }

  // Convert the price amounts to decimal value.
  $max_value = commerce_currency_amount_to_decimal($max['amount'], $max['currency_code']);
  $default_amount = min($max['amount'], $transaction->amount);
  $default_value = commerce_currency_amount_to_decimal($default_amount, $transaction->currency_code);

  // Format for textfield element
  $default_value = number_format($default_value, $currency['decimals'], '.', '');

  // Build descriptions
  $descriptions = array();
  if ($max['amount'] != $transaction->amount) {
    $descriptions[] = '<strong>' . t('Available for @action: @amount', $tokens + array('@amount' => commerce_currency_format($max['amount'], $max['currency_code']))) . '</strong>';
  }
  
  $descriptions[] = t('Original transaction: @amount', array('@amount' => commerce_currency_format($transaction->amount, $transaction->currency_code)));
  $descriptions[] = t('Order balance: @amount', array('@amount' => commerce_currency_format($order_balance['amount'], $order_balance['currency_code'])));

  $form['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('@short_label amount', $tokens),
    '#description' => implode('<br />', $descriptions),
    '#default_value' => $default_value,
    '#field_suffix' => check_plain($transaction->currency_code),
    '#size' => 16,
    '#element_validate' => array('commerce_firstdata_gge4_element_validate_number_range'),
    '#min_value' => 0,
    '#max_value' => $max_value,
  );

  $form = confirm_form($form,
    t('What amount do you want to @action?', $tokens),
    'admin/commerce/orders/' . $order->order_id . '/payment',
    '',
    t('@action', $tokens),
    t('cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Validate handler for transaction form
 */
function commerce_firstdata_gge4_transaction_form_validate($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $txn_type = $form_state['transaction_type'];
  $order = $form_state['order'];
  $payment_method = $form_state['payment_method'];
  $values = &$form_state['values'];
  
  // Validate API controller
  $gateway = commerce_firstdata_gge4_get_controller($payment_method['instance_id']);

  // DENY if no web service
  if (!$gateway->isActive('web_service')) {
    form_error($form, t('The web service API is not enabled for this payment method.'));
    return;
  }

  // Build the request state
  $charge = array(
    'amount' => abs($transaction->amount),
    'currency_code' => $transaction->currency_code,
  );
  if (isset($values['amount'])) {
    $charge['amount'] = commerce_currency_decimal_to_amount(abs($values['amount']), $transaction->currency_code);
  }

  $request_state = array(
    'txn_type' => $txn_type,
    'charge' => $charge,
    'previous_transaction' => $transaction,
    'order' => $order,
    'payment_method' => $payment_method,
  );

  // DENY if not a valid request
  if (!$gateway->get('web_service')->requestValidate($request_state)) {
    $error_messages = $gateway->get('web_service')->getErrorMessages(NULL, $request_state);
    form_error($form, t('@error', array('@error' => reset($error_messages))));
    return;
  }
}

/**
 * Submit handler for transaction form
 */
function commerce_firstdata_gge4_transaction_form_submit($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $txn_type = $form_state['transaction_type'];
  $order = $form_state['order'];
  $payment_method = $form_state['payment_method'];
  $values = &$form_state['values'];

  // Initialize API controller
  $gateway = commerce_firstdata_gge4_get_controller($payment_method['instance_id']);

  $txn_type_info = $gateway->transactionType($txn_type);
  $tokens = array(
    '@action' => $txn_type_info['action_word']['present'],
    '@short_label' => $txn_type_info['short_label'],
  );


  // Build request state
  $charge = array(
    'amount' => abs($transaction->amount),
    'currency_code' => $transaction->currency_code,
  );
  if (isset($values['amount'])) {
    $charge['amount'] = commerce_currency_decimal_to_amount(abs($values['amount']), $transaction->currency_code);
  }

  $request_state = array(
    'txn_type' => $txn_type,
    'charge' => $charge,
    'previous_transaction' => $transaction,
    'order' => $order,
    'payment_method' => $payment_method,
  );

  $response = $gateway->get('web_service')->request($request_state);
  if (empty($response['transaction_approved'])) {
    // Display an error message but leave the transaction pending.
    drupal_set_message(t('@short_label failed.', $tokens), 'error');
    foreach ($gateway->get('web_service')->getErrorMessages($response) as $error_message) {
      drupal_set_message(t('@error', array('@error' => $error_message)), 'error');
    }
  }
  else {
    drupal_set_message(t('@short_label was successful.', $tokens));
  }

  $form_state['redirect'] = 'admin/commerce/orders/' . $form_state['order']->order_id . '/payment';
}
