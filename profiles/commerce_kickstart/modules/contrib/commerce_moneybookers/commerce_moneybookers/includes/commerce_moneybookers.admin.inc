<?php

/**
 * @file
 * Administrative forms for the Moneybookers payment gateway module.
 */

/**
 * Form callback: allows the user to capture a prior authorization.
 */
function commerce_moneybookers_capture_form($form, &$form_state, $order, $transaction) {
  $form_state['order'] = $order;
  $form_state['transaction'] = $transaction;
  $form_state['payment_method'] = commerce_payment_method_instance_load($transaction->instance_id);

  $balance = commerce_payment_order_balance($order);

  // Convert the balance to the transaction currency.
  if ($balance['currency_code'] != $transaction->currency_code) {
    $balance['amount'] = commerce_currency_convert($balance['amount'], $balance['currency_code'], $transaction->currency_code);
    $balance['currency_code'] = $transaction->currency_code;
  }

  if ($balance['amount'] > 0 && $balance['amount'] < $transaction->amount) {
    $default_amount = $balance['amount'];
  }
  else {
    $default_amount = $transaction->amount;
  }

  // Convert the price amount to a user friendly decimal value.
  $default_amount = commerce_currency_amount_to_decimal($default_amount, $transaction->currency_code);

  $description = implode('<br />', array(
    t('authorization: @amount', array('@amount' => commerce_currency_format($transaction->amount, $transaction->currency_code))),
    t('order balance: @balance', array('@balance' => commerce_currency_format($balance['amount'], $balance['currency_code']))),
  ));

  $form['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('capture amount'),
    '#description' => $description,
    '#default_value' => $default_amount,
    '#field_suffix' => check_plain($transaction->currency_code),
    '#size' => 16,
  );

  $form = confirm_form($form,
    t('what amount do you want to capture?'),
    'admin/commerce/orders/' . $order->order_id . '/payment',
    '',
    t('capture'),
    t('cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Validate handler: ensure a valid amount is given.
 */
function commerce_moneybookers_capture_form_validate($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $amount = $form_state['values']['amount'];

  // Ensure a positive numeric amount has been entered for capture.
  if (!is_numeric($amount) || $amount <= 0) {
    form_set_error('amount', t('You must specify a positive numeric amount to capture.'));
  }

  // Ensure the amount is less than or equal to the authorization amount.
  if ($amount > commerce_currency_amount_to_decimal($transaction->amount, $transaction->currency_code)) {
    form_set_error('amount', t('You cannot capture more than you authorized through Be2Bill.'));
  }

  // If the authorization has expired, display an error message and redirect.
  if (REQUEST_TIME - $transaction->created > 86400 * 29) {
    drupal_set_message(t('This authorization has passed its 29 day limit and cannot be captured.'), 'error');
    drupal_goto('admin/commerce/orders/' . $form_state['order']->order_id . '/payment');
  }
}

/**
 * Submit handler: process the capture request on a prior authorization.
 */
function commerce_moneybookers_capture_form_submit($form, &$form_state) {
  $order = $form_state['order'];
  $transaction = $form_state['transaction'];
  $payment_method = $form_state['payment_method'];

  // Convert the amout to the commerce format.
  $amount = commerce_currency_decimal_to_amount($form_state['values']['amount'], $transaction->currency_code);

  $charge = array(
    'amount' => commerce_moneybookers_price_amount($amount, $transaction->currency_code),
    'currency' => $transaction->currency_code,
  );

  // Build a name-value pair array for this transaction.
  $data = commerce_moneybookers_request_capture($order, $transaction, $charge);
  // Submit the capture request to Moneybookers.
  $response = commerce_moneybookers_api_request($payment_method, $data);

  $transaction->payload[REQUEST_TIME . '-capture'] = $response;

  if (!isset($response['PROCESSING.RESULT']) || $response['PROCESSING.RESULT'] != 'ACK' || $response['PROCESSING.CODE'] != 'CC.CP.90.00') {
    // Display an error message but leave the transaction pending.
    drupal_set_message(t('Prior authorization capture failed. The transaction will remain in a pending status.'), 'error');
    if ($response['PROCESSING.RETURN']) {
      drupal_set_message(check_plain($response['PROCESSING.RETURN']), 'error');
    }
  }
  else {
    $transaction->amount = $amount;
    $transaction->remote_id = $response['IDENTIFICATION.UNIQUEID'];
    $transaction->remote_status = $response['PROCESSING.CODE'];
    $transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;

    // Note the capture in the transaction message.
    $transaction->message .= '<br />' . t('Captured: @date', array('@date' => format_date(REQUEST_TIME, 'short')));

    drupal_set_message(t('Prior authorization captured successfully.'));
  }

  // Save the updated original transaction.
  commerce_payment_transaction_save($transaction);

  // Redirect back to the current order payment page.
  $form_state['redirect'] = 'admin/commerce/orders/' . $form_state['order']->order_id . '/payment';
}

/**
 * Form callback: allows the user to void a transaction.
 */
function commerce_moneybookers_void_form($form, &$form_state, $order, $transaction) {
  $form_state['order'] = $order;
  $form_state['transaction'] = $transaction;
  $form_state['payment_method'] = commerce_payment_method_instance_load($transaction->instance_id);

  $form['markup'] = array(
    '#markup' => t('Are you sure that you want to void this transaction?'),
  );

  $form = confirm_form($form,
    t('Are you sure that you want to void this transaction?'),
    'admin/commerce/orders/' . $order->order_id . '/payment',
    '',
    t('Void'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit handler: process the void request.
 */
function commerce_moneybookers_void_form_submit($form, &$form_state) {
  $order = $form_state['order'];
  $transaction = $form_state['transaction'];
  $payment_method = $form_state['payment_method'];

  // Build a name-value pair array for this transaction.
  $data = commerce_moneybookers_request_void($order, $transaction);

  // Submit the capture request to Moneybookers.
  $response = commerce_moneybookers_api_request($payment_method, $data);

  $transaction->payload[REQUEST_TIME . '-void'] = $response;

  if (!isset($response['PROCESSING.RESULT']) || $response['PROCESSING.RESULT'] != 'ACK' || $response['PROCESSING.CODE'] != 'CC.RV.90.00') {
    // Display an error message but leave the transaction pending.
    drupal_set_message(t('The void operation failed, so the transaction will remain in a pending status.'), 'error');
    if ($response['PROCESSING.RETURN']) {
      drupal_set_message(check_plain($response['PROCESSING.RETURN']), 'error');
    }
  }
  else {
    drupal_set_message(t('Transaction successfully voided.'));
    $transaction->status = COMMERCE_PAYMENT_STATUS_FAILURE;
    $transaction->remote_status = COMMERCE_CREDIT_VOID;

    // Update the transaction message to show that it has been voided.
    $transaction->message .= '<br />' . t('Voided: @date', array('@date' => format_date(REQUEST_TIME, 'short')));
  }

  // Save the updated original transaction.
  commerce_payment_transaction_save($transaction);

  $form_state['redirect'] = 'admin/commerce/orders/' . $form_state['order']->order_id . '/payment';
}

/**
 * Form callback: allows the user to refund a credit on a prior transaction.
 */
function commerce_moneybookers_refund_form($form, &$form_state, $order, $transaction) {
  $form_state['order'] = $order;
  $form_state['transaction'] = $transaction;
  $form_state['payment_method'] = commerce_payment_method_instance_load($transaction->instance_id);

  $default_amount = commerce_currency_amount_to_decimal($transaction->amount, $transaction->currency_code);

  $form['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Refund amount'),
    '#description' => t('Enter the amount to be refunded back to the PayPal account.'),
    '#default_value' => $default_amount,
    '#field_suffix' => check_plain($transaction->currency_code),
    '#size' => 16,
  );

  $form = confirm_form($form,
    t('What amount do you want to refund?'),
    'admin/commerce/orders/' . $order->order_id . '/payment',
    '',
    t('Refund'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Validate handler: check the credit amount before attempting a refund request.
 */
function commerce_moneybookers_refund_form_validate($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $amount = $form_state['values']['amount'];

  // Ensure a positive numeric amount has been entered for refund.
  if (!is_numeric($amount) || $amount <= 0) {
    form_set_error('amount', t('You must specify a positive numeric amount to refund.'));
  }

  // Ensure the amount is less than or equal to the captured amount.
  if ($amount > commerce_currency_amount_to_decimal($transaction->amount, $transaction->currency_code)) {
    form_set_error('amount', t('You cannot refund more than you captured.'));
  }

  // If the transaction is older than 60 days, display an error message and
  // redirect.
  if (time() - $transaction->created > 86400 * 60) {
    drupal_set_message(t('This transaction has passed its 60 day limit for issuing refunds.'), 'error');
    drupal_goto('admin/commerce/orders/' . $form_state['order']->order_id . '/payment');
  }
}

/**
 * Submit handler: process a refund request.
 */
function commerce_moneybookers_refund_form_submit($form, &$form_state) {
  $order = $form_state['order'];
  $transaction = $form_state['transaction'];
  $payment_method = $form_state['payment_method'];

  // Convert the amout to the commerce format.
  $amount = commerce_currency_decimal_to_amount($form_state['values']['amount'], $transaction->currency_code);

  $charge = array(
    'amount' => commerce_moneybookers_price_amount($amount, $transaction->currency_code),
    'currency' => $transaction->currency_code,
  );

  // Build a name-value pair array for this transaction.
  $data = commerce_moneybookers_request_refund($order, $transaction, $charge);

  // Submit the capture request to Moneybookers.
  $response = commerce_moneybookers_api_request($payment_method, $data);

  if (!isset($response['PROCESSING.RESULT']) || $response['PROCESSING.RESULT'] != 'ACK' || $response['PROCESSING.CODE'] != 'CC.RF.90.00') {
    // Display an error message but leave the transaction pending.
    drupal_set_message(t('Refund failed. @reason', array('@reason' => check_plain($response['PROCESSING.RETURN']))), 'error');

    // Save the failure response message to the original transaction.
    $transaction->payload[REQUEST_TIME . '-refund'] = $response;
    commerce_payment_transaction_save($transaction);
  }
  else {
    // Create a new transaction to record the credit.
    $credit_transaction = commerce_payment_transaction_new($payment_method['method_id'], $order->order_id);
    $credit_transaction->instance_id = $payment_method['instance_id'];
    $credit_transaction->remote_id = $response['IDENTIFICATION.UNIQUEID'];
    $credit_transaction->amount = $amount * -1;
    $credit_transaction->currency_code = $transaction->currency_code;
    $credit_transaction->payload[REQUEST_TIME] = $response;
    $credit_transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
    $credit_transaction->remote_status = $response['PROCESSING.CODE'];
    $credit_transaction->message = t('Refunded: @date', array('@date' => format_date(REQUEST_TIME, 'short')));

    // Save the credit transaction.
    commerce_payment_transaction_save($credit_transaction);

    drupal_set_message(t('Refund for @amount issued successfully.', array('@amount' => commerce_currency_format($amount, $transaction->currency_code))));
  }

  // Save the updated original transaction.
  commerce_payment_transaction_save($transaction);

  $form_state['redirect'] = 'admin/commerce/orders/' . $order->order_id . '/payment';
}
