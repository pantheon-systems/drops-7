<?php

/**
 * @file
 * This file contains no working PHP code; it exists to provide additional
 * documentation for doxygen as well as to document hooks in the standard
 * Drupal manner.
 */

/**
 * Allows modules to alter the name-value pair array used to request Hosted PCI
 * before it is submitted.
 *
 * You can alter or send additional settings refering to the technical
 * documentation.
 *
 * @param array &$data
 *   The name-value pair array for the API request.
 * @param object $order
 *   If available, the full order object the payment request is being submitted
 *   for.
 * @param array $payment_method
 *   The payment method instance array associated with this API request.
 */
function hook_commerce_hosted_pci_transaction_process_alter(&$data, $order, $payment_method) {
  $data['foo'] = 'bar';
}
