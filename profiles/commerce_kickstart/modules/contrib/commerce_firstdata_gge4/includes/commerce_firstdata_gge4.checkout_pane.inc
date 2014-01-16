<?php

/**
 * @file
 * Checkout pane callback functions.
 */


/**
 * Checkout pane callback: returns a First Data customer transaction record
 * for the most recent payload on the order's last transaction
 */
function commerce_firstdata_gge4_ctr_pane_checkout_form($form, &$form_state, $checkout_pane, $order) {
  $pane_form = array();
  if (empty($order->data['payment_method'])) {
    return $pane_form;
  }
  $transactions = commerce_payment_transaction_load_multiple(array(), array(
    'instance_id' => $order->data['payment_method'],
    'order_id' => $order->order_id
  ));
  $transaction = array_pop($transactions);
  if (empty($transaction->payload)) {
    return $pane_form;
  }

  end($transaction->payload);
  $payload_timestamp = key($transaction->payload);
  $output =  theme('commerce_firstdata_gge4_ctr', array(
    'transaction' => $transaction,
    'payload_timestamp' => $payload_timestamp,
  ));
  $pane_form['ctr'] = array(
    '#markup' => $output,
    '#attached' => array('css' => array(drupal_get_path('module', 'commerce_firstdata_gge4') . '/css/commerce_firstdata_gge4.css')),
  );

  return $pane_form;
}
