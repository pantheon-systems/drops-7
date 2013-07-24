<?php

/**
 * @file
 * Administrative forms for the PayPal WPP module.
 */


/**
 * Form callback: allows the user to capture a prior authorization.
 */
function commerce_paypal_wpp_capture_form($form, &$form_state, $order, $transaction) {
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
function commerce_paypal_wpp_capture_form_validate($form, &$form_state) {
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
 * Submit handler: process a prior authorization capture via WPP.
 */
function commerce_paypal_wpp_capture_form_submit($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $amount = $form_state['values']['amount'];

  $order = $form_state['order'];
  $payment_method = $form_state['payment_method'];
  $authorization_valid = TRUE;

  // If the original authorization was more than 3 days ago, PayPal's honor
  // period is over and a reauthorization is required before capturing.
  if (REQUEST_TIME - $transaction->created > 86400 * 3) {
    // Build a name-value pair array for the reauthorization.
    $nvp = array(
      'METHOD' => 'DoReauthorization',
      'AUTHORIZATIONID' => $transaction->remote_id,
      'AMT' => commerce_paypal_price_amount($transaction->amount, $transaction->currency_code),
      'CURRENCYCODE' => $transaction->currency_code,
    );

    // Submit the reauthorization request.
    $response = commerce_paypal_api_request($payment_method, $nvp, $order);
    $transaction->payload[REQUEST_TIMESTAMP . '-reauthorization'] = $response;

    // If the response contains an authorization ID...
    if (!empty($response['AUTHORIZATIONID'])) {
      // Update the original transaction with the new ID to use when capturing.
      $transaction->remote_id = $response['AUTHORIZATIONID'];
    }
    else {
      // Otherwise do not allow the capture to proceed.
      $transaction->message .= '<br />' . t('Reauthorization failed: @time', array('@time' => format_date(REQUEST_TIME, 'short')));
      $authorization_valid = FALSE;

      // Display a failure message on the redirect.
      drupal_set_message(t('PayPal requires a reauthorization before capture after 3 days have passed since the initial authorization.'), 'error');
      drupal_set_message(t('Reauthorization failed with the following error, so the transaction could not capture and will remain in a pending status.'), 'error');
      drupal_set_message(check_plain($response['L_LONGMESSAGE0']), 'error');
    }
  }

  // If the authorization is valid or successfully reauthorized...
  if ($authorization_valid) {
    // Determine the remaining balance if the capture is successful.
    $transaction_balance = $transaction->amount - commerce_currency_decimal_to_amount($amount, $transaction->currency_code);

    // Prepare a name-value pair array to capture the requested amount.
    $nvp = array(
      'METHOD' => 'DoCapture',
      'AUTHORIZATIONID' => $transaction->remote_id,
      'AMT' => $amount,
      'CURRENCYCODE' => $transaction->currency_code,
      'COMPLETETYPE' => $transaction_balance > 0 ? 'NotComplete' : 'Complete',
    );

    // Submit the capture request request to PayPal.
    $response = commerce_paypal_api_request($payment_method, $nvp, $order);
    $transaction->payload[REQUEST_TIME . '-capture'] = $response;

    switch ($response['ACK']) {
      case 'SuccessWithWarning':
      case 'Success':
        drupal_set_message(t('Prior authorization captured successfully.'));

        // If this capture did not complete the authorization and subsequent
        // captures can be processed against it, create a follow-up transaction
        // to represent the remaining authorization amount.
        if ($nvp['COMPLETETYPE'] == 'NotComplete') {
          $new_transaction = clone($transaction);
          unset($new_transaction->transaction_id, $new_transaction->revision_id);
          $new_transaction->amount = $transaction_balance;
          commerce_payment_transaction_save($new_transaction);

          drupal_set_message(t("A follow-up transaction was made to represent the remaining authorization amount that can be captured within PayPal's allowed time limit."));
        }

        // Update the original transaction amount to the actual capture amount,
        // its remote ID to the capture's transaction ID, and its statuses to
        // indicate successful payment.
        $transaction->amount = commerce_currency_decimal_to_amount($amount, $transaction->currency_code);
        $transaction->remote_id = $response['TRANSACTIONID'];
        $transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
        $transaction->remote_status = $response['PAYMENTSTATUS'];

        // Note the capture in the transaction message.
        $transaction->message .= '<br />' . t('Captured: @date', array('@date' => format_date(REQUEST_TIME, 'short')));
        break;

      default:
        // Display an error message but leave the transaction pending.
        drupal_set_message(t('Prior authorization capture failed, so the transaction will remain in a pending status.'), 'error');
        drupal_set_message(check_plain($response['L_LONGMESSAGE0']), 'error');
        break;
    }
  }

  // Save the updated original transaction.
  commerce_payment_transaction_save($transaction);

  // Redirect back to the current order payment page.
  $form_state['redirect'] = 'admin/commerce/orders/' . $form_state['order']->order_id . '/payment';
}
