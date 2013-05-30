<?php

class DrupalOAuth2Client extends DrupalOAuthClient {

  public function getAccessToken($endpoint = NULL, $options = array()) {
    if ($this->accessToken) {
      return clone $this->accessToken;
    }

    $options += array(
      'params' => array(),
      'realm' => NULL,
      'get' => FALSE,
    );

    if (empty($endpoint)) {
      if (!empty($this->consumer->configuration['access_endpoint'])) {
        $endpoint = $this->consumer->configuration['access_endpoint'];
      }
      else {
        $endpoint = '/oauth/access_token';
      }
    }


    $response = $this->get($endpoint, array(
      'token' => TRUE,
      'params' => $options['params'],
      'realm' => $options['realm'],
      'get' => $options['get'],
    ));

    $params = array();
    parse_str($response, $params);
    if (empty($params['access_token'])) {
      $params = drupal_json_decode($response);
    }
    // FB doet het weer anders... pff
    if (isset($params['expires'])) {
      $params['expires_in'] = $params['expires'];
    }

    if (empty($params['access_token'])) {
      throw new Exception('No valid access token was returned');
    }

    // Check if we've has recieved this token previously and if so use the old one
    //TODO: Is this safe!? What if eg. multiple users are getting the same access token from the provider?
    $this->accessToken = DrupalOAuthToken::loadByKey($params['access_token'], $this->consumer);

    if (!$this->accessToken) {
      $expires = 0;
      if (isset($params['expires_in']) && $params['expires_in']) {
        $expires = REQUEST_TIME + $params['expires_in'];
      }
      $this->accessToken = new DrupalOAuthToken($params['access_token'], '', $this->consumer, array(
        'type' => OAUTH_COMMON_TOKEN_TYPE_ACCESS,
        'expires' => $expires,
      ));
      // TODO: doe iets met refresh token. $params['refresh_token']
    }


    return clone $this->accessToken;
  }

  public function getAuthorizationUrl($endpoint = NULL, $options = array()) {
    $options += array(
      'params' => array(),
    );

    if (empty($endpoint)) {
      if (!empty($this->consumer->configuration['authorization_endpoint'])) {
        $endpoint = $this->consumer->configuration['authorization_endpoint'];
      }
      else {
        $endpoint = '/oauth/authorize';
      }
    }

    $endpoint = $this->getAbsolutePath($endpoint);
    $append_query = strpos($endpoint, '?') === FALSE ? '?' : '&';
    return $endpoint . $append_query . http_build_query($options['params'], NULL, '&');
  }

  protected function get($path, $options = array()) {
    $options += array(
        'token' => FALSE,
        'params' => array(),
        'realm' => NULL,
        'get' => FALSE,
      );

    if (empty($options['realm']) && !empty($this->consumer->configuration['authentication_realm'])) {
      $options['realm'] = $this->consumer->configuration['authentication_realm'];
    }

    $token = $options['token'] ? $this->requestToken : NULL;
    $path = $this->getAbsolutePath($path);

    $req = OAuthRequest::from_consumer_and_token($this->consumer, $token,
      $options['get'] ? 'GET' : 'POST', $path, $options['params']);
    $req->sign_request($this->signatureMethod, $this->consumer, $token);

    $url = $req->get_normalized_http_url();
    $params = array();
    foreach ($req->get_parameters() as $param_key => $param_value) {
      if (substr($param_key, 0, 5) != 'oauth') {
        $params[$param_key] = $param_value;
      }
    }
    if (!empty($params) && $options['get']) {
      $url .= '?' . drupal_http_build_query($params);
    }
    $headers = array(
      'Accept: application/x-www-form-urlencoded',
      $req->to_header($options['realm']),
    );

    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    if (!$options['get']) {
      curl_setopt($ch, CURLOPT_POST, 1);
      curl_setopt($ch, CURLOPT_POSTFIELDS, drupal_http_build_query($params)); // required for Google+?
    }
    $oauth_version = _oauth_common_version();
    curl_setopt($ch, CURLOPT_USERAGENT, 'Drupal/' . VERSION . ' OAuth/' . $oauth_version);
    curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
    curl_setopt($ch, CURLOPT_HEADER, 1);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

    $response = curl_exec($ch);
    $error = curl_error($ch);
    curl_close($ch);
    if ($error) {
      throw new Exception($error);
    }

    $result = $this->interpretResponse($response);
    if ($result->responseCode != 200) {
      throw new Exception('Failed to fetch data from url "' . $path . '" (HTTP response code ' . $result->responseCode . ' ' . $result->responseMessage . '): ' . $result->body, $result->responseCode);
    }

    return $result->body;
  }
}
