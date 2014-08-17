<?php

/**
 * @file
 * Checkout pane callback functions for the PayPal Express Checkout module.
 */

/**
 * Checkout pane callback: returns the Express Checkout pane's settings form.
 */
function commerce_paypal_ec_review_pane_settings_form($checkout_pane) {
  $form = array();

  // List the core checkout panes to exclude from the review and confirm page.
  $excluded_panes = array(
    'account',
    'cart_contents',
    'commerce_payment',
    'commerce_payment_redirect',
    'checkout_review',
    'checkout_completion_message',
    'paypal_ec_review',
  );

  // Create a list of checkout panes that may be included on the Express Checkout
  // review and order page. All core Commerce panes listed above and all panes
  // defined by the Customer module will be excluded.
  $options = array();

  foreach (commerce_checkout_panes() as $pane_id => $checkout_pane) {
    if (!in_array($pane_id, $excluded_panes) && $checkout_pane['module'] != 'commerce_customer' ) {
      $options[$pane_id] = check_plain($checkout_pane['name']);
    }
  }

  // If we have available checkout panes for this page...
  if (!empty($options)) {
    // Allow the administrator to choose which panes the customer should see
    // upon returning from Express Checkout.
    $form['commerce_paypal_ec_review_embedded_panes'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Checkout panes to include on the Express Checkout review and confirm page'),
      '#options' => $options,
      '#default_value' => variable_get('commerce_paypal_ec_review_embedded_panes', array()),
    );
  }

  return $form;
}

/**
 * Checkout pane callback: returns a pane allowing the customer to review the
 * final details of the order and provide any final information required.
 *
 */
function commerce_paypal_ec_review_pane_checkout_form(&$form, &$form_state, $checkout_pane, $order) {
  $form_state['build_info']['files']['pane'] = drupal_get_path('module', 'commerce_paypal_ec') . '/includes/commerce_paypal_ec.checkout_pane.inc';
  $form_state['order'] = $order;

  // Adjust the weights of the help text and pane form to appear before the
  // embedded checkout panes.
  $form['help']['#weight'] = -10;

  $pane_form = array(
    '#weight' => -5,
  );

  // Duplicate the review checkout pane's contents.

  $pane_form['review'] = array(
    '#theme' => 'commerce_checkout_review',
    '#data' => array(),
  );

  // Loop through all the pages before the review page...
  foreach (commerce_checkout_pages() as $page_id => $checkout_page) {
    // Exit the loop once the review page is reached.
    if ($page_id == 'review') {
      break;
    }

    // Loop through all the panes on the current page specifying review...
    foreach (commerce_checkout_panes(array('page' => $page_id, 'enabled' => TRUE, 'review' => TRUE)) as $pane_id => $checkout_pane_local) {
      // If the pane has a valid review callback...
      if ($callback = commerce_checkout_pane_callback($checkout_pane_local, 'review')) {
        // Get the review data for this pane.
        $pane_data = $callback($form, $form_state, $checkout_pane_local, $order);

        // Only display the pane if there is data in the pane.
        if (!empty($pane_data)) {
          // Add a row for it in the review data.
          $pane_form['review']['#data'][$pane_id] = array(
            'title' => $checkout_pane_local['title'],
            'data' => $pane_data,
          );
        }
      }
    }
  }

  // Embed other specified checkout panes in this checkout pane.
  foreach (commerce_paypal_ec_embedded_checkout_panes() as $embedded_pane_id) {
    $embedded_pane = commerce_checkout_pane_load($embedded_pane_id);

    // If the checkout pane defines a checkout form callback...
    if ($embedded_pane && $callback = commerce_checkout_pane_callback($embedded_pane, 'checkout_form')) {
      // Get the form for the embedded checkout pane.
      $embedded_pane_form = $callback($form, $form_state, $embedded_pane, $order);

      // Embed it on this checkout pane if the form returned data.
      if (!empty($embedded_pane_form)) {
        $form[$embedded_pane_id] = array(
          '#type' => $embedded_pane['fieldset'] ? 'fieldset' : 'container',
          '#title' => check_plain($embedded_pane['title']),
          '#collapsible' => $embedded_pane['collapsible'],
          '#collapsed' => $embedded_pane['collapsed'],
          '#attributes' => array('class' => array($embedded_pane_id)),
          '#tree' => TRUE,
        ) + $embedded_pane_form;
      }
    }
  }

  $form['pay_now'] = array(
    '#type' => 'submit',
    '#value' => t('Pay Now'),
    '#validate' => array('commerce_paypal_ec_review_pane_checkout_form_validate'),
    '#submit' => array('commerce_paypal_ec_review_pane_checkout_form_submit'),
  );

  return $pane_form;
}

/**
 * Validate handler for the Express Checkout review and confirm page.
 */
function commerce_paypal_ec_review_pane_checkout_form_validate($form, &$form_state) {
  $order = $form_state['order'];
  $form_validate = TRUE;

  // Loop through the enabled checkout panes included in this page to validate
  // and submit them.
  foreach (commerce_paypal_ec_embedded_checkout_panes() as $embedded_pane_id) {
    $embedded_pane_validate = TRUE;

    // Load the checkout pane to find its checkout pane validate callback.
    $embedded_checkout_pane = commerce_checkout_pane_load($embedded_pane_id);

    // If it has a validate callback.
    if ($callback = commerce_checkout_pane_callback($embedded_checkout_pane, 'checkout_form_validate')) {
      // Give it a chance to process the submitted data.
      $embedded_pane_validate &= $callback($form, $form_state, $embedded_checkout_pane, $order);
    }

    // Submit the pane if it validated.
    if ($embedded_pane_validate && $callback = commerce_checkout_pane_callback($embedded_checkout_pane, 'checkout_form_submit')) {
      $callback($form, $form_state, $embedded_checkout_pane, $order);
    }

    // A failed pane makes the form fail.
    $form_validate &= $embedded_pane_validate;
  }

  // Save the updated order object.
  commerce_order_save($order);

  // If a pane failed validation or the form state has otherwise been altered to
  // initiate a rebuild, return without moving to the next checkout page.
  if (!$form_validate || $form_state['rebuild']) {
    $form_state['rebuild'] = TRUE;
  }
}

/**
 * Submit handler for the Express Checkout review and confirm page.
 */
function commerce_paypal_ec_review_pane_checkout_form_submit($form, &$form_state) {
  $order = $form_state['order'];
  $payment_method = commerce_payment_method_instance_load($order->data['payment_method']);

  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
  $charge = $order_wrapper->commerce_order_total->value();

  // Attempt to process the payment.
  if (commerce_paypal_ec_do_payment($payment_method, $order, $charge)) {
    // Proceed to the next page if it succeeded.
    $order_status = commerce_order_status_load($order->status);
    $checkout_page = commerce_checkout_page_load($order_status['checkout_page']);
    $next_page = $checkout_page['next_page'];

    // Update the order status to the next checkout page.
    $order = commerce_order_status_update($order, 'checkout_' . $next_page, FALSE, FALSE);

    // Inform modules of checkout completion if the next page is completed.
    if ($next_page == 'complete') {
      commerce_checkout_complete($order);
    }

    // Redirect to the URL for the new checkout page.
    $form_state['redirect'] = commerce_checkout_order_uri($order);
  }
}
