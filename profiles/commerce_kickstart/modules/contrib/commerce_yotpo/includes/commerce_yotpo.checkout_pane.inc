<?php
/**
 * @file
 * Checkout pane callback functions for the Commerce Yotpo module.
 */

/**
 * Checkout pane callback.
 *
 * Returns a pane allowing the customer to review the details of the order.
 */
function commerce_yotpo_checkout_pane_checkout_form($form, &$form_state, $checkout_pane, $order) {
  $pane_form = array();
  $order_currency = $order->commerce_order_total[LANGUAGE_NONE][0]['currency_code'];
  $order_amount = commerce_currency_amount_to_decimal($order->commerce_order_total[LANGUAGE_NONE][0]['amount'], $order_currency);
  // Otherwise add any enabled checkout panes that are visible on review.
  $pane_form['yotpo'] = array(
    '#type' => 'markup',
    '#markup' => '<img src="https://api.yotpo.com/conversion_tracking.gif?order_amount=' . $order_amount . '&order_id=' . $order->order_id . '&currency=' . $order_currency . '&app_key=' . variable_get('commerce_yotpo_id', FALSE) . '" width="1" height="1">',
  );

  return $pane_form;
}
