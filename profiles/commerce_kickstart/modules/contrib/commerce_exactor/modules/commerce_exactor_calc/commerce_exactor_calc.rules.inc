<?php
/**
 * @file
 *   commerce_exactor_calc.rules.inc
 * Rules integrations for ExactoCALC
 */

/**
 * Impliments hook_rules_action_info().
 */
function commerce_exactor_calc_rules_action_info() {
  $actions = array();

  $actions['commerce_exactor_calc_invoice_request'] = array(
    'label' => t('Submit an order for tax calculation'),
    'parameter' => array(
      'order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
        'save' => TRUE,
      ),
      'billing_address' => array(
        'type' => 'addressfield',
        'label' => t('Billing Address'),
      ),
      'shipping_address' => array(
        'type' => 'addressfield',
        'label' => t('Shipping Address'),
      ),
      'from_address' => array(
        'type' => 'addressfield',
        'label' => t('From Address'),
        'optional' => TRUE,
      ),
    ),
    'provides' => array(
      'avs_billing_address' => array(
        'type' => 'addressfield',
        'label' => t('Billing Address'),
      ),
      'avs_shipping_address' => array(
        'type' => 'integer',
        'label' => t('Shipping Address'),
      ),
    ),
    'group' => t('Commerce Exactor'),
    'callbacks' => array(
      'execute' => 'commerce_exactor_calc_invoice_request',
    ),
  );

  $actions['commerce_exactor_calc_commit_request'] = array(
    'label' => t('Commit an order for tax reporting'),
    'parameter' => array(
      'order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
        'save' => TRUE,
      ),
    ),
    'group' => t('Commerce Exactor'),
    'callbacks' => array(
      'execute' => 'commerce_exactor_calc_commit_request',
    ),
  );

  $actions['commerce_exactor_calc_refund_request'] = array(
    'label' => t('Refund an order for tax reporting'),
    'parameter' => array(
      'order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
        'save' => TRUE,
      ),
    ),
    'group' => t('Commerce Exactor'),
    'callbacks' => array(
      'execute' => 'commerce_exactor_calc_refund_request',
    ),
  );

  return $actions;
}

/**
 * Rule callback
 */
function commerce_exactor_calc_invoice_request($order, $billing_address, $shipping_address, $from_address = NULL) {

  if (!variable_get('exactor_account_id') && !variable_get('exactor_user_id')) {
    drupal_set_message(t('Exactor Settings not Configured'));
    return;
  }

  $invoice_request = commerce_exactor_calc_invoice_request_xml($order, $billing_address, $shipping_address, $from_address = NULL);
  $result = commerce_exactor_calc_tax_request($invoice_request);

  // Delete any existing shipping line items from the order.
  commerce_exactor_calc_delete_exactocalc_line_items($order, TRUE);

  if (isset($result->InvoiceResponse->TotalTaxAmount) && $result->InvoiceResponse->TotalTaxAmount > 0) {
    // Extract the unit price from the calculated rate.
    $tax_price = array(
      'amount' => commerce_currency_decimal_to_amount((string) $result->InvoiceResponse->TotalTaxAmount, (string) $result->InvoiceResponse->CurrencyCode),
      'currency_code' => (string) $result->InvoiceResponse->CurrencyCode,
      'data' => array(),
    );

    $data['exactocalc']['InvoiceResponce']['TransactionId'] = (string) $result->InvoiceResponse->TransactionId;

    // Create a new exactor line item with the calculated rate from the form.
    $line_item = commerce_exactor_calc_line_item_new($tax_price, $order->order_id, $data);

    // Save and add the line item to the order.
    commerce_exactor_calc_add_exactocalc_line_item($line_item, $order, TRUE);

    $order->data['exactor_calc']['InvoiceResponce']['TransactionId'] = (string) $result->InvoiceResponse->TransactionId;
  }
}

/**
 * Rule callback
 */
function commerce_exactor_calc_commit_request($order) {
  if (!variable_get('exactor_account_id') && !variable_get('exactor_user_id')) {
    drupal_set_message(t('Exactor Settings not Configured'));
    return;
  }

  if (isset($order->data['exactor_calc']['InvoiceResponce']['TransactionId'])) {
    $commit_request = commerce_exactor_calc_commit_request_xml($order);
    $result = commerce_exactor_calc_tax_request($commit_request);

    if (isset($result->CommitResponse)) {
      $order->data['exactor_calc']['CommitResponce']['TransactionId'] = (string) $result->CommitResponse->TransactionId;
    }
  }
}

/**
 * Rule callback
 */
function commerce_exactor_calc_refund_request($order) {
  if (!variable_get('exactor_account_id') && !variable_get('exactor_user_id')) {
    drupal_set_message(t('Exactor Settings not Configured'));
    return;
  }

  if (isset($order->data['exactor_calc']['CommitResponce']['TransactionId'])) {
    $refund_request = commerce_exactor_calc_refund_request_xml($order);
    $result = commerce_exactor_calc_tax_request($refund_request);

    if (isset($result->CommitResponse)) {
      $order->data['exactor_calc']['RefundResponce']['TransactionId'] = (string) $result->RefundResponse->TransactionId;
    }
  }
}
