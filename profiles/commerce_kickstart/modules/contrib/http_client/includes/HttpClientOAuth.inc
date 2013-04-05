<?php

class HttpClientOAuth implements HttpClientAuthentication {
  private $consumer;
  private $token;
  private $sign;
  private $signImpl;
  private $hash_body;
  private $header_auth;
  private $realm;

  public function __construct($consumer, $token = NULL, $sign_impl = NULL, $hash_body = TRUE, $header_auth = FALSE, $realm = NULL) {
    $this->consumer = $consumer;
    $this->token = $token;
    $this->signImpl = $sign_impl;
    $this->sign = is_object($sign_impl);
    $this->hash_body = $hash_body;
    $this->header_auth = $header_auth;
    $this->realm = $realm;
  }

  /**
   * Enables and disables request signing.
   * N.B. Signing will always be disabled if no signing implementation was
   * passed to the constructor.
   *
   * @param bool $sign
   *  Set to FALSE to disable signing, TRUE to enable.
   * @return void
   */
  public function signRequests($sign) {
    $this->sign = $sign;
  }

  /**
   * Used by the HttpClient to authenticate requests.
   *
   * @param HttpClientRequest $request
   * @return void
   */
  public function authenticate($request) {
    // Create a OAuth request object.
    $req = OAuthRequest::from_consumer_and_token($this->consumer, $this->token,
      $request->method, $request->url, $request->parameters);

    if ($this->hash_body) {
      // Add a body hash if applicable.
      $content_type = $request->getHeader('Content-type', TRUE);
      if (in_array($request->method, array('POST', 'PUT')) && $content_type !== 'application/x-www-form-urlencoded') {
        $data = $request->data;
        $data || $data = '';
        $req->set_parameter('oauth_body_hash', base64_encode(sha1($data, TRUE)));
      }
    }

    // Sign the request if we can and should.
    if ($this->sign && $this->signImpl) {
      $req->sign_request($this->signImpl, $this->consumer, $this->token);
    }

    // Make sure that we use the normalized url for the request
    $request->url = $req->get_normalized_http_url();

    // Transfer the parameters to the request objects
    foreach ($req->get_parameters() as $key => $val) {
      if (!$this->header_auth || substr($key, 0, 5) != 'oauth') {
        $request->parameters[$key] = $val;
      }
    }

    if ($this->header_auth) {
      $auth_header = explode(':', $req->to_header($this->realm), 2);
      $request->setHeader($auth_header[0], trim($auth_header[1]));
    }
  }
}
