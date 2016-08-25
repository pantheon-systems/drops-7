<?php

/**
 * @file
 * Custom Drupal 7 RequestMehod class for Google reCAPTCHA library.
 */

namespace ReCaptcha\RequestMethod;

use ReCaptcha\RequestMethod;
use ReCaptcha\RequestParameters;

/**
 * Sends POST requests to the reCAPTCHA service.
 */
class Drupal7Post implements RequestMethod {

  /**
   * URL to which requests are POSTed.
   * @const string
   */
  const SITE_VERIFY_URL = 'https://www.google.com/recaptcha/api/siteverify';

  /**
   * Submit the POST request with the specified parameters.
   *
   * @param RequestParameters $params Request parameters
   * @return string Body of the reCAPTCHA response
   */
  public function submit(RequestParameters $params) {

    $options = array(
      'headers' => array(
        'Content-type' => 'application/x-www-form-urlencoded',
      ),
      'method' => 'POST',
      'data' => $params->toQueryString(),
    );
    $response = drupal_http_request(self::SITE_VERIFY_URL, $options);

    return isset($response->data) ? $response->data : '';
  }
}
