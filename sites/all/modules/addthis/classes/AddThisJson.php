<?php
/**
 * @file
 * A class containing utility methods for json-related functionality.
 */

class AddThisJson {

  public function decode($url) {
    $response = drupal_http_request($url);
    $responseOk = $response->code == 200;
    return $responseOk ? drupal_json_decode($response->data) : NULL;
  }
}
