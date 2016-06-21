<?php

/**
 * @file
 * Documents hooks invoked by the Commerce Authorize.Net module.
 */


/**
 * Allows modules to alter the parameters of an AIM API request immediately
 * prior to its submission to Authorize.Net.
 *
 * @param $nvp
 *   An associative array of name-value-pairs that constitute the AIM API
 *   request; note that this array contains sensitive data in the form of API
 *   credentials and payment card data that should never be logged or retained
 *   elsewhere in the Drupal database or filesystem.
 * @param $payment_method
 *   The payment method instance array associated with this API request.
 */
function hook_commerce_authnet_aim_request_alter($nvp, $payment_method) {
  // No example.
}

/**
 * Allows modules to alter the SimpleXMLElement object used to construct a CIM
 * API request immediately prior to its submission to Authorize.Net.
 *
 * @param $api_request_element
 *   The SimpleXMLElement object containing the parameters of the API request;
 *   note that this object contains sensitive data in the form of API
 *   credentials and payment card data that should never be logged or retained
 *   elsewhere in the Drupal database or filesystem.
 * @param $payment_method
 *   The payment method instance array associated with this API request.
 * @param $request_type
 *   The name of the request type to submit.
 */
function hook_commerce_authnet_cim_request_alter($api_request_element, $payment_method, $request_type) {
  // No example.
}
