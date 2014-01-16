<?php

/**
 * @file
 * Callback functions for the checkout panes.
 */


/**
 * Checkout pane callback: returns the shipping service pane's settings form.
 */
function commerce_cba_orderdetailswidget_pane_settings_form($checkout_pane) {
  $form = array();

  // @todo: order details as link
  /*
  $form['commerce_cba_orderdetailswidget_display'] = array(
    '#type' => 'select',
    '#title' => t('Display mode'),
    '#options' => array('order' => t('Order data'), 'link' => t('Order link')),
    '#default_value' => variable_get('commerce_cba_orderdetailswidget_display', 'order'),
  );
  */

  return $form;
}

/**
 * Checkout pane callback: builds a shipping quote selection form.
 */
function commerce_cba_orderdetailswidget_pane_checkout_form($form, &$form_state, $checkout_pane, $order) {
  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $pane_form['cba_orderdetailswidget'] = commerce_cba_orderdetailswidget($order);

  return $pane_form;
}
