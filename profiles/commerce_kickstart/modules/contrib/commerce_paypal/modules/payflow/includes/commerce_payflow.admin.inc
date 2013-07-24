<?php

/**
 * @file
 * Administrative forms for the Payflow Link module.
 */


/**
 * Form callback: allows the user to capture a prior authorization.
 */
function commerce_payflow_link_capture_form($form, &$form_state, $order, $transaction) {
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
function commerce_payflow_link_capture_form_validate($form, &$form_state) {
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
    form_set_error('amount', t('You cannot capture an amount $75 or 115% greater than the authorization amount in PayPal WPP.'));
  }

  // If the authorization has expired, display an error message and redirect.
  if (REQUEST_TIME - $transaction->created > 86400 * 29) {
    drupal_set_message(t('This authorization has passed its 29 day limit and cannot be captured.'), 'error');
    drupal_goto('admin/commerce/orders/' . $form_state['order']->order_id . '/payment');
  }
}

/**
 * Submit handler: process a prior authorization capture via Payflow Pro.
 */
function commerce_payflow_link_capture_form_submit($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $amount = $form_state['values']['amount'];

  $order = $form_state['order'];
  $payment_method = $form_state['payment_method'];

  // Prepare a name-value pair array to capture the requested amount.
  $nvp = array(
    'TRXTYPE' => 'D',
    'ORIGID' => $transaction->remote_id,
    'AMT' => $amount,
    'CAPTURECOMPLETE' => 'Y',
  );

  // Submit the capture request to Payflow Pro.
  $response = commerce_payflow_api_request($payment_method, 'pro', $nvp, $order);
  $transaction->payload[REQUEST_TIME . '-capture'] = $response;

  switch (intval($response['RESULT'])) {
    case 0:
      drupal_set_message(t('Prior authorization captured successfully.'));

      // Update the original transaction amount to the actual capture amount,
      // its remote ID to the capture's transaction ID, and its statuses to
      // indicate successful payment.
      $transaction->amount = commerce_currency_decimal_to_amount($amount, $transaction->currency_code);
      $transaction->remote_id = $response['PNREF'];
      $transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
      $transaction->remote_status = 'D';

      // Note the capture in the transaction message.
      $transaction->message .= '<br />' . t('Captured: @date', array('@date' => format_date(REQUEST_TIME, 'short')));

      // Add the PayPal fees to the message if available.
      if (!empty($response['FEEAMT'])) {
        $transaction->message .= '<br />' . t('PayPal fees: @feeamt', array('@feeamt' => $response['FEEAMT']));
      }

      break;

    default:
      // Display an error message but leave the transaction pending.
      drupal_set_message(t('Prior authorization capture failed, so the transaction will remain in a pending status.'), 'error');
      drupal_set_message(check_plain($response['RESPMSG']), 'error');
      break;
  }

  // Save the updated original transaction.
  commerce_payment_transaction_save($transaction);

  // Redirect back to the current order payment page.
  $form_state['redirect'] = 'admin/commerce/orders/' . $form_state['order']->order_id . '/payment';
}

/**
 * Form callback: allows the user to perform a reference transaction.
 */
function commerce_payflow_link_reference_form($form, &$form_state, $order, $transaction) {
  $form_state['order'] = $order;
  $form_state['transaction'] = $transaction;

  // Load and store the payment method instance for this transaction.
  if ($transaction->data['commerce_payflow']['tender'] == 'P') {
    $payment_method = commerce_payment_method_instance_load($transaction->data['commerce_payflow']['original_instance']);
  }
  else {
    $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  }
  $form_state['payment_method'] = $payment_method;

  $balance = commerce_payment_order_balance($order);

  // Convert the balance to the transaction currency.
  if ($balance['currency_code'] != $transaction->currency_code) {
    $balance['amount'] = commerce_currency_convert($balance['amount'], $balance['currency_code'], $transaction->currency_code);
    $balance['currency_code'] = $transaction->currency_code;
  }

  // Convert the price amount to a user friendly decimal value or leave it blank
  // if there is no oustanding balance on the order.
  if ($balance['amount'] > 0) {
    $default_amount = number_format(commerce_currency_amount_to_decimal($balance['amount'], $transaction->currency_code), 2, '.', '');
  }
  else {
    $default_amount = '';
  }

  $description = implode('<br />', array(
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
function commerce_payflow_link_reference_form_validate($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $amount = $form_state['values']['amount'];

  // Ensure a positive numeric amount has been entered for capture.
  if (!is_numeric($amount) || $amount <= 0) {
    form_set_error('amount', t('You must specify a positive numeric amount to capture.'));
  }

  // If the original transaction tender was PayPal, ensure there is a billing
  // agreement ID in place.
  if (!empty($transaction->data['commerce_payflow']['tender']) &&
    $transaction->data['commerce_payflow']['tender'] == 'P' &&
    empty($transaction->data['commerce_payflow']['baid'])) {
    drupal_set_message(t('This transaction was initiated from a PayPal account, but it has no billing agreement ID for use in reference transactions.'), 'error');
    drupal_goto('admin/commerce/orders/' . $form_state['order']->order_id . '/payment');
  }

  // If the transaction has expired, display an error message and redirect.
  if (REQUEST_TIME - $transaction->created > 86400 * 365) {
    drupal_set_message(t('This transaction has passed its 365 day limit and can no longer be used for reference transactions.'), 'error');
    drupal_goto('admin/commerce/orders/' . $form_state['order']->order_id . '/payment');
  }
}

/**
 * Submit handler: process a reference transaction via Payflow Pro.
 */
function commerce_payflow_link_reference_form_submit($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $amount = $form_state['values']['amount'];

  $order = $form_state['order'];
  $payment_method = $form_state['payment_method'];

  // Prepare a name-value pair array to capture the requested amount.
  $nvp = array(
    'BUTTONSOURCE' => $payment_method['buttonsource'],
    'TRXTYPE' => 'S',
    'ORIGID' => $transaction->remote_id,
    'AMT' => $amount,
    'TENDER' => 'C',
  );

  // If the reference transaction needs to go through PayPal, add the additional
  // parameters for the billing agreement.
  if (!empty($transaction->data['commerce_payflow']['tender']) && $transaction->data['commerce_payflow']['tender'] == 'P') {
    $nvp['TENDER'] = 'P';
    $nvp['ACTION'] = 'D';
    $nvp['BAID'] = $transaction->data['commerce_payflow']['baid'];

    // Ensure the transaction references the original PNREF.
    if (!empty($transaction->data['commerce_payflow']['pnref'])) {
      $nvp['ORIGID'] = $transaction->data['commerce_payflow']['pnref'];
    }

    // Update the BUTTONSOURCE to indicate Express Checkout.
    $ec_payment_method = commerce_payment_method_instance_load('paypal_ec|' . $payment_method['settings']['paypal_ec_instance']);
    $nvp['BUTTONSOURCE'] = $ec_payment_method['buttonsource'];
  }

  // Submit the reference transaction request to Payflow Pro.
  $response = commerce_payflow_api_request($payment_method, 'pro', $nvp, $order);

  // Create a new transaction to represent the reference transaction.
  $new_transaction = clone($transaction);
  unset($new_transaction->transaction_id, $new_transaction->revision_id, $new_transaction->created, $new_transaction->changed);
  $new_transaction->amount = commerce_currency_decimal_to_amount($amount, $transaction->currency_code);
  $new_transaction->payload[REQUEST_TIME . '-reference'] = $response;
  $new_transaction->remote_id = $response['PNREF'];
  $new_transaction->remote_status = 'S';

  if ($transaction->data['commerce_payflow']['tender'] == 'P') {
    if (!empty($response['PPREF'])) {
      $new_transaction->remote_id = $response['PPREF'];
    }

    if (!empty($response['PAYMENTSTATUS'])) {
      $new_transaction->remote_status = commerce_payflow_paypal_remote_status($response['PAYMENTSTATUS']);
    }
  }

  $message = array();

  if (isset($response['RESULT']) && intval($response['RESULT']) === 0) {
    drupal_set_message(t('Reference transaction processed successfully.'));
    $message[] = t('Reference transaction from @origid.', array('@origid' => $transaction->remote_id));

    // Add the PayPal billing agreement ID and fees if given.
    if (!empty($response['BAID'])) {
      $message[] = t('Billing agreement ID: @baid', array('@baid' => $response['BAID']));
    }
    if (!empty($response['FEEAMT'])) {
      $message[] = t('PayPal fees: @feeamt', array('@feeamt' => $response['FEEAMT']));
    }

    $new_transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
  }
  else {
    // Display an error message but leave the transaction pending.
    drupal_set_message(t('Refrence transaction failed: @reason', array('@reason' => $response['RESPMSG'])), 'error');
    $message[] = t('Failed reference transaction from @origid.', array('@origid' => $transaction->remote_id));
    $new_transaction->status = COMMERCE_PAYMENT_STATUS_FAILURE;
  }

  $new_transaction->message = implode('<br />', $message);

  // Save the new transaction.
  commerce_payment_transaction_save($new_transaction);

  // Redirect back to the current order payment page.
  $form_state['redirect'] = 'admin/commerce/orders/' . $form_state['order']->order_id . '/payment';
}

/**
 * Form callback: allows the user to void a transaction.
 */
function commerce_payflow_link_void_form($form, &$form_state, $order, $transaction) {
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
function commerce_payflow_link_void_form_submit($form, &$form_state) {
  $transaction = $form_state['transaction'];

  // Build a name-value pair array for this transaction.
  $nvp = array(
    'TRXTYPE' => 'V',
    'ORIGID' => $transaction->remote_id,
  );

  // Submit the request to Payflow Pro.
  $response = commerce_payflow_api_request($form_state['payment_method'], 'pro', $nvp, $form_state['order']);

  // Update and save the transaction based on the response.
  $transaction->payload[REQUEST_TIME . '-void'] = $response;

  // If we got an approval response code...
  if (intval($response['RESULT']) === 0) {
    drupal_set_message(t('Transaction successfully voided.'));

    // Set the remote and local status accordingly.
    $transaction->remote_id = $response['PNREF'];
    $transaction->status = COMMERCE_PAYMENT_STATUS_FAILURE;
    $transaction->remote_status = 'V';

    // Update the transaction message to show that it has been voided.
    $transaction->message .= '<br />' . t('Voided: @date', array('@date' => format_date(REQUEST_TIME, 'short')));
  }
  else {
    drupal_set_message(t('Prior authorization capture failed, so the transaction will remain in a pending status.'), 'error');
    drupal_set_message(check_plain($response['RESPMSG']), 'error');
  }

  commerce_payment_transaction_save($transaction);

  $form_state['redirect'] = 'admin/commerce/orders/' . $form_state['order']->order_id . '/payment';
}

/**
 * Form callback: allows the user to issue a credit on a prior transaction.
 */
function commerce_payflow_link_refund_form($form, &$form_state, $order, $transaction) {
  $form_state['order'] = $order;
  $form_state['transaction'] = $transaction;

  // Load and store the payment method instance for this transaction.
  $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  $form_state['payment_method'] = $payment_method;

  $default_amount = number_format(commerce_currency_amount_to_decimal($transaction->amount, $transaction->currency_code), 2, '.', '');

  $form['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Refund amount'),
    '#description' => t('Enter the amount to be refunded back to the original credit card.'),
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
function commerce_payflow_link_refund_form_validate($form, &$form_state) {
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
 * Submit handler: process a refund via Payflow Pro.
 */
function commerce_payflow_link_refund_form_submit($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $amount = $form_state['values']['amount'];
  $order = $form_state['order'];
  $payment_method = $form_state['payment_method'];

  // Prepare a name-value pair array to capture the requested amount.
  $nvp = array(
    'TRXTYPE' => 'C',
    'ORIGID' => $transaction->remote_id,
    'AMT' => $amount,
  );

  // Submit the refund request to Payflow Pro.
  $response = commerce_payflow_api_request($payment_method, 'pro', $nvp, $order);

  // If the credit succeeded...
  if (intval($response['RESULT']) === 0) {
    $credit_amount = commerce_currency_decimal_to_amount($amount, $transaction->currency_code);
    drupal_set_message(t('Refund for @amount issued successfully.', array('@amount' => commerce_currency_format($credit_amount, $transaction->currency_code))));

    // Create a new transaction to record the credit.
    $credit_transaction = commerce_payment_transaction_new($payment_method['method_id'], $order->order_id);
    $credit_transaction->instance_id = $payment_method['instance_id'];
    $credit_transaction->remote_id = $response['PNREF'];
    $credit_transaction->amount = $credit_amount * -1;
    $credit_transaction->currency_code = $transaction->currency_code;
    $credit_transaction->payload[REQUEST_TIME] = $response;
    $credit_transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
    $credit_transaction->remote_status = 'C';
    $credit_transaction->message = t('Refunded to @remote_id.', array('@remote_id' => $transaction->remote_id));

    // Save the credit transaction.
    commerce_payment_transaction_save($credit_transaction);
  }
  else {
    // Display a failure message and response reason from Payflow.
    drupal_set_message(t('Refund failed: @reason', array('@reason' => $response['RESPMSG'])), 'error');

    // Save the failure response message to the original transaction.
    $transaction->payload[REQUEST_TIME . '-refund'] = $response;
    commerce_payment_transaction_save($transaction);
  }

  $form_state['redirect'] = 'admin/commerce/orders/' . $order->order_id . '/payment';
}
