<?php

/**
 * @file
 * Form callbacks.
 */

/**
 * Form callback: create or edit a card.
 *
 * Payment method access validated in menu callback
 *
 * @param $card
 *   The card object to edit through the form.
 * @param $account
 *   For new cards, the customer's user account.
 *
 * @see commerce_firstdata_gge4_cardonfile_form()
 */
function commerce_firstdata_gge4_cardonfile_form($form, &$form_state, $card) {
  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'commerce_firstdata_gge4') . '/includes/commerce_firstdata_gge4.forms.inc';

  // Set for validate and submit handlers.
  $form_state['card'] = $card;

  // Load API controller
  $gateway = commerce_firstdata_gge4_get_controller($card->instance_id);

  // Create zero dollar charge
  $charge = array(
    'amount' => 0,
    'currency_code' => commerce_default_currency(),
  );

  // Build the request state
  $request_state = array(
    'charge' => $charge,
    'card' => $card,
  );

  // Preference given to hosted payment if enabled
  if ($gateway->isActive('hpp')) {
    $form += $gateway->get('hpp')->paymentForm($form, $form_state, $request_state);

    // Override storage options
    $form['commerce_cardonfile_store'] = array(
      '#type' => 'hidden',
      '#value' => TRUE,
    );

    return $form;
  }

  drupal_set_message(t('This payment gateway is not configured for this operation.'));
  return $form;
}
