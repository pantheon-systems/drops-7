<?php

/**
 * @file
 * Administrative forms for the Paypal EC module.
 */

/**
 * Form callback: allows the user to capture a prior authorization.
 */
function commerce_paypal_ec_capture_form($form, &$form_state, $order, $transaction) {
  $form_state['order'] = $order;
  $form_state['transaction'] = $transaction;

  // Load and store the payment method instance for this transaction.
  $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  $form_state['payment_method'] = $payment_method;

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
  $default_amount = number_format(commerce_currency_amount_to_decimal($default_amount, $transaction->currency_code), 2, '.', '');

  $description = implode('<br />', array(
    t('Authorization: @amount', array('@amount' => commerce_currency_format($transaction->amount, $transaction->currency_code))),
    t('Order balance: @balance', array('@balance' => commerce_currency_format($balance['amount'], $balance['currency_code']))),
  ));

  $form['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Capture amount'),
    '#description' => $description,
    '#default_value' => $default_amount,
    '#field_suffix' => check_plain($transaction->currency_code),
    '#size' => 16,
  );

  $form = confirm_form($form,
    t('What amount do you want to capture?'),
    'admin/commerce/orders/' . $order->order_id . '/payment',
    '',
    t('Capture'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Validate handler: ensure a valid amount is given.
 */
function commerce_paypal_ec_capture_form_validate($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $amount = $form_state['values']['amount'];

  // Ensure a positive numeric amount has been entered for capture.
  if (!is_numeric($amount) || $amount <= 0) {
    form_set_error('amount', t('You must specify a positive numeric amount to capture.'));
  }

  // Ensure the amount is within the allowed limit for PayPal authorizations.
  $authorization_amount = commerce_currency_amount_to_decimal($transaction->amount, $transaction->currency_code);
  $authorization_amount_upper = commerce_currency_amount_to_decimal($transaction->amount, $transaction->currency_code) + 75;

  if ($amount > $authorization_amount * 1.15 || $amount > $authorization_amount_upper) {
    form_set_error('amount', t('You cannot capture an amount $75 or 115% greater than the authorization amount in PayPal EC.'));
  }

  // If the authorization has expired, display an error message and redirect.
  if (REQUEST_TIME - $transaction->created > 86400 * 29) {
    drupal_set_message(t('This authorization has passed its 29 day limit and cannot be captured.'), 'error');
    drupal_goto('admin/commerce/orders/' . $form_state['order']->order_id . '/payment');
  }
}

/**
 * Submit handler: process a prior authorization capture via PayPal EC.
 */
function commerce_paypal_ec_capture_form_submit($form, &$form_state) {
  $transaction = $form_state['transaction'];

  $order = $form_state['order'];
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
  $order_total = $order_wrapper->commerce_order_total->value();
  $order_total['amount'] = commerce_currency_convert($order_total['amount'], $order_total['currency_code'], $transaction->currency_code);

  // Convert the amout to the commerce format.
  $amount = commerce_currency_decimal_to_amount($form_state['values']['amount'], $transaction->currency_code);

  // Build a name-value pair array for this transaction.
  $nvp = array(
    'METHOD' => 'DoCapture',
    'AUTHORIZATIONID' => $transaction->remote_id,
    'AMT' => commerce_paypal_price_amount($amount, $transaction->currency_code),
    'CURRENCYCODE' => $transaction->currency_code,
    'COMPLETETYPE' => $order_total['amount'] != $amount ? 'NotComplete' : 'Complete',
    'INVNUM' => commerce_paypal_ipn_invoice($order),
  );

  // Submit the capture request to Paypal.
  $response = commerce_paypal_api_request($form_state['payment_method'], $nvp, $order);

  $transaction->payload[REQUEST_TIME . '-capture'] = $response;

  switch ($response['ACK']) {
    case 'Success':
      drupal_set_message(t('Prior authorization captured successfully.'));

      // Update the original transaction amount to the actual capture amount,
      // its remote ID to the capture's transaction ID, and its statuses to
      // indicate successful payment.
      $transaction->amount = $amount;
      $transaction->remote_id = $response['TRANSACTIONID'];
      $transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
      $transaction->remote_status = $response['PAYMENTSTATUS'];

      // Note the capture in the transaction message.
      $transaction->message .= '<br />' . t('Captured: @date', array('@date' => format_date(REQUEST_TIME, 'short')));

      if ($response['PAYMENTSTATUS'] == 'Pending') {
        $transaction->message .= '<br />' . commerce_paypal_short_pending_reason($response['PENDINGREASON']);
      }

      break;

    default:
      // Display an error message but leave the transaction pending.
      drupal_set_message(t('Prior authorization capture failed, so the transaction will remain in a pending status.'), 'error');

      if (isset($response['L_LONGMESSAGE0'])) {
        drupal_set_message(check_plain($response['L_LONGMESSAGE0']), 'error');
      }

      break;
  }

  // Save the updated original transaction.
  commerce_payment_transaction_save($transaction);

  // Redirect back to the current order payment page.
  $form_state['redirect'] = 'admin/commerce/orders/' . $form_state['order']->order_id . '/payment';
}

/**
 * Form callback: allows the user to void a transaction.
 */
function commerce_paypal_ec_void_form($form, &$form_state, $order, $transaction) {
  $form_state['order'] = $order;
  $form_state['transaction'] = $transaction;

  // Load and store the payment method instance for this transaction.
  $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  $form_state['payment_method'] = $payment_method;

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
function commerce_paypal_ec_void_form_submit($form, &$form_state) {
  $transaction = $form_state['transaction'];

  // Build a name-value pair array for this transaction.
  $nvp = array(
    'METHOD' => 'DoVoid',
    'AUTHORIZATIONID' => $transaction->remote_id,
  );

  // Submit the request to Paypal.
  $response = commerce_paypal_api_request($form_state['payment_method'], $nvp, $form_state['order']);

  // Update and save the transaction based on the response.
  $transaction->payload[REQUEST_TIME . '-void'] = $response;

  // If we got an approval response code...
  if ($response['ACK'] == 'Success') {
    drupal_set_message(t('Transaction successfully voided.'));

    // Set the remote and local status accordingly.
    $transaction->status = COMMERCE_PAYMENT_STATUS_FAILURE;
    $transaction->remote_status = COMMERCE_CREDIT_VOID;

    // Update the transaction message to show that it has been voided.
    $transaction->message .= '<br />' . t('Voided: @date', array('@date' => format_date(REQUEST_TIME, 'short')));
  }
  else {
    drupal_set_message(t('The void operation failed, so the transaction will remain in a pending status.'), 'error');
  }

  commerce_payment_transaction_save($transaction);

  $form_state['redirect'] = 'admin/commerce/orders/' . $form_state['order']->order_id . '/payment';
}

/**
 * Form callback: allows the user to issue a credit on a prior transaction.
 */
function commerce_paypal_ec_refund_form($form, &$form_state, $order, $transaction) {
  $form_state['order'] = $order;
  $form_state['transaction'] = $transaction;

  // Load and store the payment method instance for this transaction.
  $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  $form_state['payment_method'] = $payment_method;

  $default_amount = number_format(commerce_currency_amount_to_decimal($transaction->amount, $transaction->currency_code), 2, '.', '');

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
function commerce_paypal_ec_refund_form_validate($form, &$form_state) {
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

  // If the transaction is older than 60 days, display an error message and redirect.
  if (time() - $transaction->created > 86400 * 60) {
    drupal_set_message(t('This transaction has passed its 60 day limit for issuing refunds.'), 'error');
    drupal_goto('admin/commerce/orders/' . $form_state['order']->order_id . '/payment');
  }
}

/**
 * Submit handler: process a refund request.
 */
function commerce_paypal_ec_refund_form_submit($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $payment_method = $form_state['payment_method'];

  $order = $form_state['order'];
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
  $order_total = $order_wrapper->commerce_order_total->value();

  $amount = commerce_currency_decimal_to_amount($form_state['values']['amount'], $order_total['currency_code']);

  // Build a name-value pair array for this transaction.
  $nvp = array(
    'METHOD' => 'RefundTransaction',
    'TRANSACTIONID' => $transaction->remote_id,
    'REFUNDTYPE' => $order_total['amount'] != $amount ? 'Partial' : 'Full',
    'AMT' => commerce_paypal_price_amount($amount, $order_total['currency_code']),
    'CURRENCYCODE' => $order_total['currency_code'],
  );

  // Submit the refund request to Paypal.
  $response = commerce_paypal_api_request($form_state['payment_method'], $nvp, $order);

  // If the credit succeeded...
  if ($response['ACK'] == 'Success') {
    drupal_set_message(t('Refund for @amount issued successfully.', array('@amount' => commerce_currency_format($amount, $transaction->currency_code))));

    // Create a new transaction to record the credit.
    $credit_transaction = commerce_payment_transaction_new($payment_method['method_id'], $order->order_id);
    $credit_transaction->instance_id = $payment_method['instance_id'];
    $credit_transaction->remote_id = $response['REFUNDTRANSACTIONID'];
    $credit_transaction->amount = $amount * -1;
    $credit_transaction->currency_code = $transaction->currency_code;
    $credit_transaction->payload[REQUEST_TIME] = $response;
    $credit_transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
    $credit_transaction->remote_status = COMMERCE_CREDIT_CREDIT;
    $credit_transaction->message = t('Refunded to @remote_id.', array('@remote_id' => $transaction->remote_id));

    // Save the credit transaction.
    commerce_payment_transaction_save($credit_transaction);
  }
  else {
    // Display a failure message and response reason from Payflow.
    drupal_set_message(t('Refund failed: @reason', array('@reason' => $response['L_LONGMESSAGE0'])), 'error');

    // Save the failure response message to the original transaction.
    $transaction->payload[REQUEST_TIME . '-refund'] = $response;
    commerce_payment_transaction_save($transaction);
  }

  $form_state['redirect'] = 'admin/commerce/orders/' . $order->order_id . '/payment';
}
