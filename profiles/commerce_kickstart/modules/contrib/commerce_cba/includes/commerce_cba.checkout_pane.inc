<?php

/**
 * @file
 * Callback functions for the checkout panes.
 */


/**
 * Checkout pane callback: returns the shipping service pane's settings form.
 */
function commerce_cba_addresswidget_pane_settings_form($checkout_pane) {
  $form = array();

  return $form;
}

/**
 * Checkout pane callback: builds a shipping quote selection form.
 */
function commerce_cba_addresswidget_pane_checkout_form($form, &$form_state, $checkout_pane, $order) {
  // Build the checkout pane form.
  $pane_form = commerce_cba_amazonaddresswidget_pane_form($order);
  if (!empty($pane_form)) {
    $form_state['commerce_cba']['amazon-delivery-address'] = 'amazon-delivery-address';
  }

  return $pane_form;
}

/**
 * Checkout pane callback: builds a shipping quote selection form.
 */
function commerce_cba_addresswidget_pane_review($form, &$form_state, $checkout_pane, $order) {
  // Build the pane for checkout review pane.
  $pane_form = commerce_cba_amazonaddresswidget_pane_form($order, TRUE);

  return drupal_render($pane_form);
}

/**
 * Checkout pane callback: validate the Amazon address checkout pane.
 */
function commerce_cba_addresswidget_pane_checkout_form_validate($form, &$form_state, $checkout_pane, $order) {
  // Check if the Amazon address widget checkout pane form is in the checkout form.
  if (!empty($form[$checkout_pane['pane_id']])) {
    // Validate the address selection if needed.
    if (isset($form_state['commerce_cba']['amazon-delivery-address']) && !isset($order->data['commerce_cba']['amazon-delivery-address'])) {
      form_set_error('amazon-delivery-address', t('Please select one of the addresses to continue'));
      return FALSE;
    }
    // Amazon call to get the address data.
    commerce_cba_form_commerce_checkout_validate($form, $form_state);
  }
  return TRUE;
}

/**
 * Checkout pane callback: submit the Amazon address checkout pane.
 */
function commerce_cba_addresswidget_pane_checkout_form_submit(&$form, &$form_state, $checkout_pane, $order) {
  // Save the Amazon delivery address to shipping customer profile address.
  if (module_exists('commerce_shipping')) {
    commerce_cba_form_commerce_checkout_add_address($form, $form_state, $order);
  }
}

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
  $pane_form = array();
  $orderdetailswidget = commerce_cba_orderdetailswidget($order);
  if (!empty($orderdetailswidget)) {
    $pane_form['cba_orderdetailswidget'] = $orderdetailswidget;
  }

  return $pane_form;
}
