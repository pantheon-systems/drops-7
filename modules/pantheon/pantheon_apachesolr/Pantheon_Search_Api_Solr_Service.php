<?php

/**
 * Current Supported Class for RC4+
 */
class PantheonApachesolrSearchApiSolrConnection extends SearchApiSolrConnection {

  /**
   * @var string
   */
  protected $method;

  function __construct(array $options) {

    // Adding in custom settings for Pantheon
    $options['scheme'] = 'https';
    $options['host'] = variable_get('pantheon_index_host', 'index.'. variable_get('pantheon_tier', 'live') .'.getpantheon.com');
    $options['path'] = 'sites/self/environments/' . variable_get('pantheon_environment', 'dev') . '/index';
    $options['port'] = variable_get('pantheon_index_port', 449);
    
    // pantheon_curl_setup will determine whether binding.pem needed and return
    // path in options if so.
    list($ch, $opts) = pantheon_apachesolr_curl_setup("", $options['port']);
    $sslContext = array();

    // Use ssl cert if provided
    if (isset($opts[CURLOPT_SSLCERT])) {
        $sslContext['local_cert'] = $opts[CURLOPT_SSLCERT];
    } else {
        // Add peer verification settings if available.
        if (isset($opts[CURLOPT_SSL_VERIFYPEER])) {
            $sslContext['verify_peer'] = (bool) $opts[CURLOPT_SSL_VERIFYPEER];
        }
        if (isset($opts[CURLOPT_SSL_VERIFYHOST])) {
            $sslContext['verify_peer_name'] = (bool) $opts[CURLOPT_SSL_VERIFYHOST];
        }
    }
    
    if ($sslContext) {
        $this->setStreamContext(stream_context_create(array('ssl' => $sslContext)));
    }

    // Adding in general settings for Search API
    $options += array(
      'scheme' => 'http',
      'host' => 'localhost',
      'port' => 8983,
      'path' => 'solr',
      'http_user' => NULL,
      'http_pass' => NULL,
      'http_method' => 'POST',
      'local_cert'=> NULL,
    );
    $this->options = $options;

    $path = '/' . trim($options['path'], '/') . '/';
    $this->base_url = $options['scheme'] . '://' . $options['host'] . ':' . $options['port'] . $path;

    // Make sure we always have a valid method set, default to POST.
    $this->method = $options['http_method'] == 'GET' ? 'GET' : 'POST';

    // Set HTTP Basic Authentication parameter, if login data was set.
    if (strlen($options['http_user']) && strlen($options['http_pass'])) {
      $this->http_auth = 'Basic ' . base64_encode($options['http_user'] . ':' . $options['http_pass']);
    }
  }

  /**
   * Sends an HTTP request to Solr.
   *
   * This is just a wrapper around drupal_http_request().
   *
   * Overridden by Pantheon to set a timeout and possibly other improvements.
   */
  protected function makeHttpRequest($url, array $options = array()) {
    if (empty($options['method']) || $options['method'] == 'GET' || $options['method'] == 'HEAD') {
      // Make sure we are not sending a request body.
      $options['data'] = NULL;
    }
    if ($this->http_auth) {
      $options['headers']['Authorization'] = $this->http_auth;
    }
    if ($this->stream_context) {
      $options['context'] = $this->stream_context;
    }
    // Specify timeout.
    $options['timeout'] = 5;

    $result = drupal_http_request($url, $options);

    if (!isset($result->code) || $result->code < 0) {
      $result->code = 0;
      $result->status_message = 'Request failed';
      $result->protocol = 'HTTP/1.0';
    }
    // Additional information may be in the error property.
    if (isset($result->error)) {
      $result->status_message .= ': ' . check_plain($result->error);
    }

    if (!isset($result->data)) {
      $result->data = '';
      $result->response = NULL;
    }
    else {
      $response = json_decode($result->data);
      if (is_object($response)) {
        foreach ($response as $key => $value) {
          $result->$key = $value;
        }
      }
    }

    return $result;
  }

}

class PantheonApachesolrSearchApiSolrService extends SearchApiSolrService {
  protected $connection_class = 'PantheonApachesolrSearchApiSolrConnection';
}

