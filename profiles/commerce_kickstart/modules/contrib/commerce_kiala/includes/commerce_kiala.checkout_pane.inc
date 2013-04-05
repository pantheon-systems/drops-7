<?php

/**
 * @file
 *   Checkout pane hooks
 */

/**
 * commerce_kiala_order_point_pane: form callback.
 */
function commerce_kiala_order_point_pane_checkout_form($form, &$form_state, $checkout_pane, $order) {
  global $user;

  $pane_form = array();
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);

  $point_id = $order_wrapper->kiala_point_id->value();
  if (!empty($point_id)) {
    $pane_form['commerce_kiala_point_details'] = array(
      '#markup' => theme('commerce_kiala_point_details', array(
        'point_details' => $order_wrapper->kiala_point->point_details->value(),
        'show_more_link' => FALSE,
      )),
    );
  }

  return $pane_form;
}
