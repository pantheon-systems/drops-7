<?php

/**
 * @file
 * Administrative forms for the Hosted Pci module.
 */

/**
 * Form callback: allows the user to capture a prior authorization.
 */
function commerce_hosted_pci_capture_form($form, &$form_state, $order, $transaction) {
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
function commerce_hosted_pci_capture_form_validate($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $amount = $form_state['values']['amount'];

  // Ensure a positive numeric amount has been entered for capture.
  if (!is_numeric($amount) || $amount <= 0) {
    form_set_error('amount', t('You must specify a positive numeric amount to capture.'));
  }

  // Ensure the amount is less than or equal to the authorization amount.
  if ($amount > commerce_currency_amount_to_decimal($transaction->amount, $transaction->currency_code)) {
    form_set_error('amount', t('You cannot capture more than you authorized through Hosted Pci.'));
  }

  // If the authorization has expired, display an error message and redirect.
  if (REQUEST_TIME - $transaction->created > 86400 * 30) {
    drupal_set_message(t('This authorization has passed its 30 day limit cannot be captured.'), 'error');
    drupal_goto('admin/commerce/orders/' . $form_state['order']->order_id . '/payment');
  }
}

/**
 * Submit handler: process a prior authorization capture via AIM.
 */
function commerce_hosted_pci_capture_form_submit($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $order = $form_state['order'];
  // Convert the amount to the commerce format.
  $amount = commerce_currency_decimal_to_amount($form_state['values']['amount'], $transaction->currency_code);
  // Build a name-value pair array for this transaction.
  $data = array();
  $data['pxyTransaction.txnAmount'] = commerce_hosted_pci_price_amount($amount, $transaction->currency_code);
  $data['pxyTransaction.txnCurISO'] = $transaction->currency_code;
  $data['pxyTransaction.processorRefId'] = $transaction->remote_id;
  $data['pxyTransaction.merchantRefId'] = $order->order_id;

  // Request Hosted PCI service.
  $response = commerce_hosted_pci_transaction_process(COMMERCE_CREDIT_CAPTURE_ONLY, $form_state['payment_method'], $order, $data);

  // Update and save the transaction based on the response.
  $transaction->payload[REQUEST_TIME . '-capture'] = $response;

  // The call is valid and the payment gateway has been approved.
  if ($response && $response['status'] == 'success' && $response['pxyResponse_responseStatus'] == 'approved') {
    drupal_set_message(t('Prior authorization captured successfully.'));
    // Update the transaction amount to the actual captured amount.
    $transaction->amount = $amount;
    // Set the remote and local status accordingly.
    $transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
    $transaction->remote_status = COMMERCE_CREDIT_CAPTURE_ONLY;
    // Append a capture indication to the result message.
    $transaction->message .= '<br />' . t('Captured: @date', array('@date' => format_date(REQUEST_TIME, 'short')));
  }
  else {
    // The payment has been rejected, display an error message but leave the
    // transaction pending.
    drupal_set_message(t('Prior authorization capture failed, so the transaction will remain in a pending status.'), 'error');
  }

  commerce_payment_transaction_save($transaction);

  $form_state['redirect'] = 'admin/commerce/orders/' . $form_state['order']->order_id . '/payment';
}


/**
 * Form callback: allows the user to void a transaction.
 */
function commerce_hosted_pci_void_form($form, &$form_state, $order, $transaction) {
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
function commerce_hosted_pci_void_form_submit($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $order = $form_state['order'];
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
  $order_total = $order_wrapper->commerce_order_total->value();

  // Build a name-value pair array for this transaction.
  $data = array();
  $data['pxyTransaction.txnAmount'] = commerce_hosted_pci_price_amount($order_total['amount'], $order_total['currency_code']);
  $data['pxyTransaction.txnCurISO'] = $order_wrapper->commerce_order_total->currency_code->value();
  $data['pxyTransaction.processorRefId'] = $transaction->remote_id;
  $data['pxyTransaction.merchantRefId'] = $order->order_id;

  // Request Hosted PCI service.
  $response = commerce_hosted_pci_transaction_process(COMMERCE_CREDIT_VOID, $form_state['payment_method'], $order, $data);

  $transaction->payload[REQUEST_TIME . '-void'] = $response;
  // The call is valid and the payment gateway has been approved.
  if ($response && $response['status'] == 'success' && $response['pxyResponse_responseStatus'] == 'approved') {
    drupal_set_message(t('Transaction successfully voided.'));
    // Set the remote and local status accordingly.
    $transaction->status = COMMERCE_PAYMENT_STATUS_FAILURE;
    $transaction->remote_status = COMMERCE_CREDIT_VOID;
    // Append a capture indication to the result message.
    $transaction->message .= '<br />' . t('Voided: @date', array('@date' => format_date(REQUEST_TIME, 'short')));
  }
  else {
    // The payment has been rejected, display an error message but leave the
    // transaction in the same status.
    drupal_set_message(t('Void failed'), 'error');
  }

  // Update and save the transaction based on the response.
  commerce_payment_transaction_save($transaction);
  $form_state['redirect'] = 'admin/commerce/orders/' . $form_state['order']->order_id . '/payment';
}


/**
 * Form callback: allows the user to issue a credit on a prior transaction.
 */
function commerce_hosted_pci_credit_form($form, &$form_state, $order, $transaction) {
  $form_state['order'] = $order;
  $form_state['transaction'] = $transaction;
  // Load and store the payment method instance for this transaction.
  $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  $form_state['payment_method'] = $payment_method;

  // Take the amount from the order balance to substract it to the total.
  $balance = commerce_payment_order_balance($order);
  $default_amount = commerce_currency_amount_to_decimal($transaction->amount - $balance['amount'], $transaction->currency_code);

  $form['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Credit amount'),
    '#description' => t('Enter the amount to be credited back to the original credit card.'),
    '#default_value' => $default_amount,
    '#field_suffix' => check_plain($transaction->currency_code),
    '#size' => 16,
  );

  $form = confirm_form($form,
    t('What amount do you want to credit?'),
    'admin/commerce/orders/' . $order->order_id . '/payment',
    '',
    t('Credit'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Validate handler: check the credit amount before attempting credit request.
 */
function commerce_hosted_pci_credit_form_validate($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $amount = $form_state['values']['amount'];

  // Ensure a positive numeric amount has been entered for credit.
  if (!is_numeric($amount) || $amount <= 0) {
    form_set_error('amount', t('You must specify a positive numeric amount to credit.'));
  }

  // Ensure the amount is less than or equal to the captured amount.
  if ($amount > commerce_currency_amount_to_decimal($transaction->amount, $transaction->currency_code)) {
    form_set_error('amount', t('You cannot credit more than you captured through Hosted Pci.'));
  }

  // If the transaction is older than 120 days, display an error message and
  // redirect.
  if (REQUEST_TIME - $transaction->created > 86400 * 120) {
    drupal_set_message(t('This capture has passed its 120 day limit for issuing credits.'), 'error');
    drupal_goto('admin/commerce/orders/' . $form_state['order']->order_id . '/payment');
  }
}

/**
 * Submit handler: process a credit request.
 */
function commerce_hosted_pci_credit_form_submit($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $payment_method = $form_state['payment_method'];

  $order = $form_state['order'];
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
  $order_total = $order_wrapper->commerce_order_total->value();

  $amount = commerce_currency_decimal_to_amount($form_state['values']['amount'], $order_total['currency_code']);

  // Build a name-value pair array for this transaction.
  $data = array();
  $data['pxyTransaction.txnAmount'] = commerce_hosted_pci_price_amount($amount, $order_total['currency_code']);
  $data['pxyTransaction.txnCurISO'] = $order_total['currency_code'];
  $data['pxyTransaction.processorRefId'] = $transaction->remote_id;
  $data['pxyTransaction.merchantRefId'] = $order->order_id;

  // Request Hosted PCI service.
  $response = commerce_hosted_pci_transaction_process(COMMERCE_CREDIT_CREDIT, $form_state['payment_method'], $order, $data);
  // The call is valid and the payment gateway has been approved.
  if ($response && $response['status'] == 'success' && $response['pxyResponse_responseStatus'] == 'approved') {

    drupal_set_message(t('Credit for @amount issued successfully', array('@amount' => commerce_currency_format($amount, $transaction->currency_code))));

    // Create a new transaction to record the credit.
    $credit_transaction = commerce_payment_transaction_new('hosted_pci', $order->order_id);
    $credit_transaction->instance_id = $payment_method['instance_id'];
    $credit_transaction->remote_id = $response['pxyResponse_processorRefId'];
    $credit_transaction->amount = $amount * -1;
    $credit_transaction->currency_code = $transaction->currency_code;
    $credit_transaction->payload[REQUEST_TIME . '-credit'] = $response;
    $credit_transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
    $credit_transaction->remote_status = COMMERCE_CREDIT_CREDIT;
    $credit_transaction->message = t('Credited to @remote_id.', array('@remote_id' => $transaction->remote_id));

    // Save the credit transaction.
    commerce_payment_transaction_save($credit_transaction);
  }
  // The payment has been rejected.
  else {
    // Save the failure response message to the original transaction.
    $transaction->payload[REQUEST_TIME . '-credit'] = $response;

    // Display a failure message from Hosted Pci.
    drupal_set_message(t('Credit failed') . '<br />' . t('@reason', array('@reason' => $response['pxyResponse_fullNativeResp']['txnResponse_responseText'])), 'error');

    commerce_payment_transaction_save($transaction);
  }

  $form_state['redirect'] = 'admin/commerce/orders/' . $order->order_id . '/payment';
}
