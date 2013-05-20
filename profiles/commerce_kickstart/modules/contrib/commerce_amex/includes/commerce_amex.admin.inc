<?php

/**
 * @file
 * Administrative forms for the Amex module.
 */

/**
 * Form callback: allows the user to capture a prior authorization.
 */
function commerce_amex_capture_form($form, &$form_state, $order, $transaction) {
  $form_state['order'] = $order;
  $form_state['transaction'] = $transaction;

  // Load and store the payment method instance for this transaction.
  $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  $form_state['payment_method'] = $payment_method;

  $balance = commerce_payment_order_balance($order);

  if ($balance['amount'] > 0 && $balance['amount'] < $transaction->amount) {
    $default_amount = $balance['amount'];
  }
  else {
    $default_amount = $transaction->amount;
  }

  // Convert the price amount to a user friendly decimal value.
  $default_amount = commerce_currency_amount_to_decimal($default_amount, $transaction->currency_code);

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
function commerce_amex_capture_form_validate($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $amount = $form_state['values']['amount'];

  // Ensure a positive numeric amount has been entered for capture.
  if (!is_numeric($amount) || $amount <= 0) {
    form_set_error('amount', t('You must specify a positive numeric amount to capture.'));
  }

  // Ensure the amount is less than or equal to the authorization amount.
  if ($amount > commerce_currency_amount_to_decimal($transaction->amount, $transaction->currency_code)) {
    form_set_error('amount', t('You cannot capture more than you authorized through Amex.'));
  }

  // If the authorization has expired, display an error message and redirect.
  if (time() - $transaction->created > 86400 * 30) {
    drupal_set_message(t('This authorization has passed its 30 day limit cannot be captured.'), 'error');
    drupal_goto('admin/commerce/orders/' . $form_state['order']->order_id . '/payment');
  }
}

/**
 * Submit handler: process a prior authorization capture via AIM.
 */
function commerce_amex_capture_form_submit($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $amount = $form_state['values']['amount'];

  $response = commerce_amex_capture_transaction($transaction, $amount);

  $form_state['redirect'] = 'admin/commerce/orders/' . $form_state['order']->order_id . '/payment';
}

/**
 * Form callback: allows the user to capture a prior authorization.
 */
function commerce_amex_void_form($form, &$form_state, $order, $transaction) {
  $form_state['order'] = $order;
  $form_state['transaction'] = $transaction;

  // Load and store the payment method instance for this transaction.
  $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  $form_state['payment_method'] = $payment_method;

  $balance = commerce_payment_order_balance($order);

  $description = implode('<br />', array(
    t('Authorization: @amount', array('@amount' => commerce_currency_format($transaction->amount, $transaction->currency_code))),
    t('Order balance: @balance', array('@balance' => commerce_currency_format($balance['amount'], $balance['currency_code']))),
  ));

  $form = confirm_form($form,
    t('Do you want to VOID this transaction'),
    'admin/commerce/orders/' . $order->order_id . '/payment',
    '',
    t('Void '),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Validate handler: ensure a valid amount is given.
 */
function commerce_amex_void_form_validate($form, &$form_state) {
  $transaction = $form_state['transaction'];

  // If the authorization has expired, display an error message and redirect.
  if (time() - $transaction->created > 86400 * 30) {
    drupal_set_message(t('This authorization has passed its 30 day limit cannot be voided.'), 'error');
    drupal_goto('admin/commerce/orders/' . $form_state['order']->order_id . '/payment');
  }
}

/**
 * Submit handler: process a prior authorization capture via AIM.
 */
function commerce_amex_void_form_submit($form, &$form_state) {
  $transaction = $form_state['transaction'];

  $response = commerce_amex_void_transaction($transaction);

  $form_state['redirect'] = 'admin/commerce/orders/' . $form_state['order']->order_id . '/payment';
}

/**
 * Form callback: allows the user to refun a prior capture.
 */
function commerce_amex_refund_form($form, &$form_state, $order, $transaction) {
  $form_state['order'] = $order;
  $form_state['transaction'] = $transaction;

  // Load and store the payment method instance for this transaction.
  $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  $form_state['payment_method'] = $payment_method;

  $default_amount = $transaction->amount;

  // Convert the price amount to a user friendly decimal value.
  $default_amount = commerce_currency_amount_to_decimal($default_amount, $transaction->currency_code);

  $description = implode('<br />', array(
    t('Captured: @amount', array('@amount' => commerce_currency_format($transaction->amount, $transaction->currency_code))),
  ));

  $form['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Refund amount'),
    '#description' => $description,
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
 * Validate handler: ensure a valid amount is given.
 */
function commerce_amex_refund_form_validate($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $amount = $form_state['values']['amount'];

  // Ensure a positive numeric amount has been entered for capture.
  if (!is_numeric($amount) || $amount <= 0) {
    form_set_error('amount', t('You must specify a positive numeric amount to refund.'));
  }

  // Ensure the amount is less than or equal to the authorization amount.
  if ($amount > commerce_currency_amount_to_decimal($transaction->amount, $transaction->currency_code)) {
    form_set_error('amount', t('You cannot refund more than you captured through Amex.'));
  }

  // If the authorization has expired, display an error message and redirect.
  if (time() - $transaction->created > 86400 * 30) {
    drupal_set_message(t('This authorization has passed its 30 day limit cannot be refunded.'), 'error');
    drupal_goto('admin/commerce/orders/' . $form_state['order']->order_id . '/payment');
  }
}

/**
 * Submit handler: process a prior authorization capture via AIM.
 */
function commerce_amex_refund_form_submit($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $amount = $form_state['values']['amount'];

  $response = commerce_amex_refund_transaction($transaction, $amount);

  $form_state['redirect'] = 'admin/commerce/orders/' . $form_state['order']->order_id . '/payment';
}

/**
 * Menu callback.
 */
function commerce_amex_update_transaction($order, $transaction) {
  if ($transaction->payment_method == 'amex_hosted' && $transaction->status == COMMERCE_PAYMENT_STATUS_AMEX_REVIEW) {
    $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
    $url = $payment_method['settings']['txn_url'] . AMEX_TXN_PATH . $payment_method['settings']['merchant_id'] . '/order/' . (10000000000 + $transaction->transaction_id) . '/transaction/' . $transaction->transaction_id;
    $result = _commerce_amex_get_request($url, $payment_method['settings']['password']);
    if (isset($result->result) && $result->result == 'SUCCESS' && isset($result->response->risk->reviewResult)) {
      switch ($result->response->risk->reviewResult) {
        case 'ORDER_RELEASED':
          switch ($payment_method['settings']['txn_type']) {
            case AMEX_OP_AUTH:
              $transaction->status = COMMERCE_PAYMENT_STATUS_PENDING;
              break;
            case AMEX_OP_PAY:
              $transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
          }
          $transaction->remote_status = $result->response->gatewayCode;
          $transaction->message = $result->response->acquirerMessage;
          drupal_set_message(t('Transaction has been RELEASED'));
          break;
        case 'ORDER_CANCELLED':
          $transaction->status = COMMERCE_PAYMENT_STATUS_AMEX_CANCELED;
          $transaction->remote_status = $result->response->risk->reviewResult;
          $transaction->message = $result->response->acquirerMessage;
          drupal_set_message(t('Transaction has been CANCELED'));
          break;
        default:
          drupal_set_message(t('Transaction is still in REVIEW'));
          break;
      }
      $transaction->payload = $result;
      $transaction = commerce_payment_transaction_save($transaction);
    }
    else {
      $transaction = _commerce_amex_error_process($result, $transaction);
    }
  }
  drupal_goto('admin/commerce/orders/' . $order->order_id . '/payment');
}
