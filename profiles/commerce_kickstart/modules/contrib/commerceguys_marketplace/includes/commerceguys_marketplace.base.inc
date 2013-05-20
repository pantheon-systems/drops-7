<?php

class CommerceGuysMarketplaceNoCredentialsException extends RuntimeException {}
class CommerceGuysMarketplaceAuthorizationException extends RuntimeException {}

/**
 * Defines the base manager class for interacting with remote marketplace items.
 */
abstract class CommerceGuysMarketplaceManagerBase {

  /**
   * Cache of items retrieved from the remote service.
   *
   * @var array
   */
  protected $loadedItems;

  /**
   * Constructs a CommerceGuysMarketplaceManagerBase object.
   *
   * @param $client
   *   An array with client credentials used to get an access token.
   * @param $endpoint
   *   The endpoint url of the remote service.
   * @param $resource
   *   The name of the resource managed by this class. Specified by the
   *   child class when calling the parent constructor.
   */
  function __construct($client, $endpoint, $resource = NULL) {
    if (empty($resource)) {
      throw new Exception("Cannot create an instance of marketplace manager without a specified resource.");
    }

    $this->client = $client;
    $this->endpoint = $endpoint;
    $this->resource = $resource;
  }

  /**
   * Loads an item from the remote service.
   *
   * @param $id
   *   The id of the item to retrieve.
   *
   * @return
   *   The loaded item if found, FALSE otherwise.
   */
  public function load($id) {
    if (!isset($this->loadedItems[$id])) {
      $item = FALSE;
      $response = drupal_http_request($this->endpoint . '/' . $this->resource . '/' . $id);
      $data = json_decode($response->data);
      if (is_object($data)) {
        $item = $data;
      }

      $this->loadedItems[$id] = $item;
    }

    return $this->loadedItems[$id];
  }

  /**
   * Returns an OAuth2 access token for communicating with the remote API.
   *
   * If there is no token, or the token is expired, performs an authorization
   * grant request against the remote OAuth2 server.
   *
   * @throws CommerceGuysMarketplaceNoCredentialsException
   *   If the client credentials are empty.
   * @throws CommerceGuysMarketplaceAuthorizationException
   *   If the authorization grant request failed.
   */
  protected function getAccessToken() {
    if (empty($this->client)) {
      throw new CommerceGuysMarketplaceNoCredentialsException("Missing client credentials.");
    }

    $access_token = variable_get('commerceguys_marketplace_access_token', array());
    if (empty($access_token['token']) || $access_token['expires'] < time()) {
      $data = array(
        'grant_type' => 'client_credentials',
        'client_id' => $this->client['client_key'],
        'client_secret' => $this->client['client_secret'],
      );
      $options = array(
        'method' => 'POST',
        'data' => http_build_query($data),
        'headers' => array(
          'Content-Type' => 'application/x-www-form-urlencoded',
        ),
      );
      $response = drupal_http_request(COMMERCEGUYS_MARKETPLACE_URL . '/oauth2/token', $options);
      $result = json_decode($response->data);
      // Handle errors
      if ($response->code != 200 || isset($result->error)) {
        throw new CommerceGuysMarketplaceAuthorizationException($result->error_description);
      }

      $access_token = array(
        'token' => $result->access_token,
        'expires' => time() + $result->expires_in,
      );
      variable_set('commerceguys_marketplace_access_token', $access_token);
    }

    return $access_token;
  }
}
