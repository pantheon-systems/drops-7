<?php

/**
 * Current Supported Class for RC4+
 */
#[\AllowDynamicProperties]
class PantheonApachesolrSearchApiSolrConnection extends SearchApiSolrConnection {

  protected $base_url;
  protected $http_auth;
  protected $method;
  protected $options;

  function __construct(array $options) {

    // Adding in custom settings for Pantheon.
    if (pantheon_apachesolr_get_search_version() == 8) {
      $options['scheme'] = getenv('PANTHEON_INDEX_SCHEME');
      $options['host'] = getenv('PANTHEON_INDEX_HOST');
      $options['path'] = getenv('PANTHEON_INDEX_PATH') . getenv('PANTHEON_INDEX_CORE');
      $options['port'] = intval(getenv('PANTHEON_INDEX_PORT'));
    }
    else {
      $options['scheme'] = 'https';
      $options['host'] = variable_get('pantheon_index_host', 'index.'. variable_get('pantheon_tier', 'live') .'.getpantheon.com');
      $options['path'] = 'sites/self/environments/' . variable_get('pantheon_environment', 'dev') . '/index';
      $options['port'] = variable_get('pantheon_index_port', 449);
    }
    $this->setStreamContext(
      stream_context_create(
        array(
          'ssl' => array(
            'local_cert' => pantheon_apachesolr_client_cert(),
          )
        )
      )
    );

    // Adding in general settings for Search API.
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

  /**
   * {@inheritdoc}
   */
  protected function connect($clean_path = TRUE) {
    if (!$this->solr) {
      $connection_class = $this->getConnectionClass();
      if (!class_exists($connection_class)) {
        throw new SearchApiException(t('Invalid class @class set as Solr connection class.', array('@class' => $connection_class)));
      }
      $options = $this->options + array('server' => $this->server->machine_name);

      // Adding in custom settings for Pantheon SOLR 8.x.
      if (pantheon_apachesolr_get_search_version() == 8) {
        $options['scheme'] = getenv('PANTHEON_INDEX_SCHEME');
        $options['host'] = getenv('PANTHEON_INDEX_HOST');
        $options['path'] = getenv('PANTHEON_INDEX_PATH') . getenv('PANTHEON_INDEX_CORE');
        $options['port'] = intval(getenv('PANTHEON_INDEX_PORT'));
      }

      // If clean path is set then remove (/#/) character patterns.
      if ($clean_path && strpos($options['path'], '/#/')) {
        $alternative_options = $options;
        $alternative_options['path'] = str_replace('/#/', '/', $alternative_options['path']);

        $this->solr = new $connection_class($alternative_options);
        if ($this->solr instanceof SearchApiSolrConnectionInterface) {
          return;
        }
      }

      $this->solr = new $connection_class($options);
      if (!($this->solr instanceof SearchApiSolrConnectionInterface)) {
        $this->solr = NULL;
        throw new SearchApiException(t('Invalid class @class set as Solr connection class.', array('@class' => $connection_class)));
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public function getServerLink() {
    if (pantheon_apachesolr_get_search_version() == 8) {
      $url = getenv('PANTHEON_INDEX_SCHEME') . '://' . getenv('PANTHEON_INDEX_HOST') . ':' . intval(getenv('PANTHEON_INDEX_PORT')) . '/' . getenv('PANTHEON_INDEX_PATH') . getenv('PANTHEON_INDEX_CORE');
    }
    else {
      if (!$this->options) {
        return '';
      }
      $host = $this->options['host'];
      if ($host == 'localhost' && !empty($_SERVER['SERVER_NAME'])) {
        $host = $_SERVER['SERVER_NAME'];
      }
      $url = $this->options['scheme'] . '://' . $host . ':' . $this->options['port'] . $this->options['path'];
    }
    return l($url, $url);
  }

  /**
   * {@inheritdoc}
   */
  public function ping() {
    if (pantheon_apachesolr_get_search_version() == 8) {
      $host = PANTHEON_APACHESOLR_HOST;
      $path = getenv('PANTHEON_INDEX_PATH') . getenv('PANTHEON_INDEX_CORE') . '/admin/ping';
      $pantheon_apachesolr_schema = variable_get('pantheon_apachesolr_schema');

      if (strpos($pantheon_apachesolr_schema, 'search_api_solr') !== FALSE) {
        $path .= '?q=id:1';
      }

      $url = 'https://'. $host . '/' . $path;

      $ch = curl_init();
      curl_setopt($ch, CURLOPT_SSLCERT, pantheon_apachesolr_client_cert());

      $opts = pantheon_apachesolr_curlopts();
      $opts[CURLOPT_URL] = $url;
      $opts[CURLOPT_PORT] = PANTHEON_APACHESOLR_PORT;
      $opts[CURLOPT_CONNECTTIMEOUT] = 5;
      $opts[CURLOPT_RETURNTRANSFER] = 1;

      curl_setopt_array($ch, $opts);
      $response = curl_exec($ch);
      $info = curl_getinfo($ch);
      curl_close($ch);
      return ($info['http_code'] == 200);
    }
    else {
      $this->connect(FALSE);
      return $this->solr->ping();
    }
  }

}

/**
 * Legacy Supported Class for RC2.
 */
class PantheonSearchApiSolrService extends SearchApiSolrConnection {
  /**
   * Constructor.
   */
  public function __construct(array $options) {
    if (pantheon_apachesolr_get_search_version() == 8) {
      $host = getenv('PANTHEON_INDEX_HOST');
      $path = getenv('PANTHEON_INDEX_PATH') . getenv('PANTHEON_INDEX_CORE');
      $port = intval(getenv('PANTHEON_INDEX_PORT'));
    }
    else {
      $host = variable_get('pantheon_index_host', 'index.'. variable_get('pantheon_tier', 'live') .'.getpantheon.com');
      $path = 'sites/self/environments/'. variable_get('pantheon_environment', 'dev') .'/index';
      $port = variable_get('pantheon_index_port', 449);
    }
    $options = array(
      'host' => $host,
      'path' => $path,
      'port' => $port,
      'default_field' => 'id',
    );
    parent::__construct($options);
    // Since /ping otherwise complains about missing default field.
    $this->_pingUrl .= '?q=' . $options['default_field'] . ':1';

    // As of July 2011, the newest release is r60, with Service.php having
    // revision 59. Revision 40 is just anything between 22 (old) and that.
    $this->newClient = trim(parent::SVN_REVISION, '$ :A..Za..z') > 40;
    if ($this->newClient) {
      $this->_httpTransport = new PanteheonSearchApiSolrHttpTransport();
    }
  }
}

/**
 * Pantheon implementation of the HTTP transport interface.
 *
 * Uses curl() for sending the request with certificate auth
 */

if (class_exists('Apache_Solr_HttpTransport_Abstract')) {
  class PanteheonSearchApiSolrHttpTransport extends Apache_Solr_HttpTransport_Abstract {
    public function __construct() {}

    /**
     * Perform a GET HTTP operation with an optional timeout and return the response
     * contents, use getLastResponseHeaders to retrieve HTTP headers
     *
     * @param string $url
     * @param float $timeout
     * @return Apache_Solr_HttpTransport_Response HTTP response
     */
    public function performGetRequest($url, $timeout = FALSE) {
      return $this->performHttpRequest('GET', $url, $timeout);
    }

    /**
     * Perform a HEAD HTTP operation with an optional timeout and return the response
     * headers - NOTE: head requests have no response body
     *
     * @param string $url
     * @param float $timeout
     * @return Apache_Solr_HttpTransport_Response HTTP response
     */
    public function performHeadRequest($url, $timeout = FALSE) {
      return $this->performHttpRequest('HEAD', $url, $timeout);
    }

    /**
     * Perform a POST HTTP operation with an optional timeout and return the response
     * contents, use getLastResponseHeaders to retrieve HTTP headers
     *
     * @param string $url
     * @param string $rawPost
     * @param string $contentType
     * @param float $timeout
     * @return Apache_Solr_HttpTransport_Response HTTP response
     */
    public function performPostRequest($url, $rawPost, $contentType, $timeout = FALSE) {
      return $this->performHttpRequest('POST', $url, $timeout, $rawPost, $contentType);
    }

    /**
     * Helper method for making an HTTP request.
     */
    protected function performHttpRequest($method, $url, $timeout, $rawPost = NULL, $contentType = NULL) {
      // The _constructUrl() in Apache_Solr_Service hard codes http like a boss.
      $url = str_replace('http://', 'https://', $url);
      // Kludgy workaround of double-get-arging.
      // https://index.live.getpantheon.com:449/sites/self/environments/dev/index/admin/ping?q=id:1?q=id:1
      // WHY ARG WHY!?!?!
      $parts = explode('?', $url);
      $url = $parts[0] .'?'. $parts[1];
      $client_cert = pantheon_apachesolr_client_cert();
      $port = variable_get('pantheon_index_port', 449);
      $ch = curl_init();
      curl_setopt($ch, CURLOPT_SSLCERT, $client_cert);

      $opts = pantheon_apachesolr_curlopts();
      $opts[CURLOPT_URL] = $url;
      $opts[CURLOPT_PORT] = $port;

      if ($timeout) {
        $opts[CURLOPT_CONNECTTIMEOUT] = $timeout;
      }
      curl_setopt_array($ch, $opts);

      // If we are doing a delete request...
      if ($method == 'DELETE') {
        curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "DELETE");
      }
      // If we are doing a put request...
      if ($method == 'PUT') {
        curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "PUT");
      }
      // If we are doing a put request...
      if ($method == 'POST') {
        curl_setopt($ch, CURLOPT_POST, 1);
      }
      if ($rawPost) {
        curl_setopt($ch, CURLOPT_POSTFIELDS, $rawPost);
      }

      $response = curl_exec($ch);

      if ($response == NULL) {
        // TODO; better error handling.
        watchdog('pantheon_apachesolr', "Error !error connecting to !url on port !port", array('!error' => curl_error($ch), '!url' => $url, '!port' => $port), WATCHDOG_ERROR);
      }
      else {
        // mimick the $result object from drupal_http_request()
        // TODO; better error handling
        $result = new stdClass();
        list($split, $result->data) = explode("\r\n\r\n", $response, 2);
        $split = preg_split("/\r\n|\n|\r/", $split);
        list($result->protocol, $result->code, $result->status_message) = explode(' ', trim(array_shift($split)), 3);
        // Parse headers.
        $result->headers = array();
        while (!empty($split) && $line = trim(array_shift($split))) {
          list($header, $value) = explode(':', $line, 2);
          if (isset($result->headers[$header]) && $result->header == 'Set-Cookie') {
            // RFC 2109: the Set-Cookie response header comprises the token Set-
            // Cookie:, followed by a comma-separated list of one or more cookies.
            $result->headers[$header] .= ',' . trim($value);
          }
          else {
            $result->headers[$header] = trim($value);
          }
        }
      }

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

      // drupal_set_message("$url: $result->code");

      $type = isset($result->headers['content-type']) ? $result->headers['content-type'] : 'text/xml';
      $body = isset($result->data) ? $result->data : NULL;
      return new Apache_Solr_HttpTransport_Response($result->code, $type, $body);
    }

  }
}
