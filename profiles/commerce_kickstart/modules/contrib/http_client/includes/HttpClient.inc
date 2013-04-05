<?php

/**
 * A http client.
 */
class HttpClient {

  protected $authentication = NULL;
  protected $request_alter = NULL;
  protected $formatter = NULL;
  protected $lastError = FALSE;
  protected $delegate = NULL;

  public $rawResponse;
  public $lastResponse;

  /**
   * Allows specification of additional custom options.
   */
  public $options = array();

  /**
   * Creates a Http client.
   *
   * @param HttpClientAuthentication $authentication
   *  Optional. Authentication to use for the request.
   * @param HttpClientFormatter $formatter
   *  Optional. Formatter to use for request and response bodies.
   * @param mixed $request_alter
   *  Optional. Either a callable, a object with a public alterRequest method,
   *  a class that has a public static alterRequestMethod or FALSE.
   * @param HttpClientDelegate $delegate
   *  Optional. The delegate that executes the call for the HttpClient.
   *  Defaults to a HttpClientCurlDelegate if curl is available.
   */
  public function __construct($authentication = NULL, $formatter = NULL, $request_alter = FALSE, $delegate = NULL) {
    $this->authentication = $authentication;
    $this->formatter = $formatter;

    if (!$formatter || in_array('HttpClientFormatter', class_implements($formatter))) {
      $this->formatter = $formatter;
    }
    else {
      throw new Exception(t('The formatter parameter must either be a object implementing HttpClientFormatter, or evaluate to FALSE.'));
    }

    if (is_object($request_alter) && is_callable(array($request_alter, 'alterRequest'))) {
      $request_alter = array($request_alter, 'alterRequest');
    }
    if (!$request_alter || is_callable($request_alter)) {
      $this->request_alter = $request_alter;
    }
    else {
      throw new Exception(t('The request_alter parameter must either be a object or class with a public alterRequest method, callable in itself or evaluate to FALSE.'));
    }

    if (!$delegate && function_exists('curl_init')) {
      $delegate = new HttpClientCurlDelegate();
    }
    if (!$delegate) {
      throw new Exception(t('The HttpClient cannot execute requests without a delegate. This probably means that you don\'t have curl installed on your system.'));
    }
    $this->delegate = $delegate;
  }

  /**
   * Inject authentication class
   *
   * @param HttpClientAuthentication $auth
   *   The class to use for authentication.
   */
  public function setAuthentication(HttpClientAuthentication $auth) {
    $this->authentication = $auth;
  }

  /**
   * Inject formatter class
   *
   * @param HttpClientFormatter $formatter
   *   The class to use for formatting.
   */
  public function setFormatter(HttpClientFormatter $formatter) {
    $this->formatter = $formatter;
  }

  /**
   * Executes a GET request.
   */
  public function get($url, $parameters = array()) {
    return $this->execute(new HttpClientRequest($url, array(
      'method' => 'GET',
      'parameters' => $parameters,
    )));
  }

  /**
   * Executes a POST request.
   */
  public function post($url, $data = NULL, $parameters = array()) {
    return $this->execute(new HttpClientRequest($url, array(
      'method' => 'POST',
      'parameters' => $parameters,
      'data' => $data,
    )));
  }

  /**
   * Executes a PUT request.
   */
  public function put($url, $data = NULL, $parameters = array()) {
    return $this->execute(new HttpClientRequest($url, array(
      'method' => 'PUT',
      'parameters' => $parameters,
      'data' => $data,
    )));
  }

  /**
   * Executes a DELETE request.
   */
  public function delete($url, $parameters = array()) {
    return $this->execute(new HttpClientRequest($url, array(
      'method' => 'DELETE',
      'parameters' => $parameters,
    )));
  }

  /**
   * Executes the given request.
   */
  public function execute(HttpClientRequest $request) {
    // Allow the request to be altered
    if ($this->request_alter) {
      call_user_func($this->request_alter, $request);
    }

    if (isset($request->data)) {
      if ($this->formatter) {
        $request->setHeader('Content-type', $this->formatter->contentType());
        $request->data = $this->formatter->serialize($request->data);
      }
      else {
        $request->data = (string) $request->data;
      }
      if (is_string($request->data)) {
        $request->setHeader('Content-length', strlen($request->data));
      }
    }
    if ($this->formatter) {
      $request->setHeader('Accept', $this->formatter->accepts());
    }

    // Allow the authentication implementation to do it's magic
    if ($this->authentication) {
      $this->authentication->authenticate($request);
    }

    $response = $this->delegate->execute($this, $request);
    $this->lastResponse = $response;

    $result = NULL;

    if ($response->responseCode >= 200 && $response->responseCode <= 299) {
      if ($this->formatter) {
        try {
          $result = $this->formatter->unserialize($response->body);
        }
        catch (Exception $e) {
          throw new HttpClientException('Failed to unserialize response', 0, $response, $e);
        }
      }
      else {
        $result = $response->body;
      }
    }
    // Output any errors set by remote drupal sites.
    elseif (!empty($response->drupalErrors)) {
      throw new HttpClientException(check_plain(implode("\n", $response->drupalErrors)), $response->responseCode, $response);
    }
    // Treat all remaining non-200 responses as errors
    else {
      throw new HttpClientException(check_plain($response->responseMessage), $response->responseCode, $response);
    }

    return $result;
  }

  /**
   * Stolen from OAuth_common
   */
  public static function urlencode_rfc3986($input) {
    if (is_array($input)) {
      return array_map(array('HttpClient', 'urlencode_rfc3986'), $input);
    }
    else if (is_scalar($input)) {
      return str_replace(
        '+',
        ' ',
        str_replace('%7E', '~', rawurlencode($input))
      );
    }
    else {
      return '';
    }
  }
}

/**
 * Abstract base class for Http client delegates.
 */
abstract class HttpClientDelegate {

  /**
   * Executes a request for the HttpClient.
   *
   * @param HttpClient $client
   *  The client we're acting as a delegate for.
   * @param HttpClientRequest $request
   *  The request to execute.
   * @return object
   *  The interpreted response.
   */
  public abstract function execute(HttpClient $client, HttpClientRequest $request);

  /**
   * This function interprets a raw http response.
   *
   * @param HttpClient $client
   * @param string $response
   * @return object
   *  The interpreted response.
   */
  protected function interpretResponse(HttpClient $client, $response) {
    $client->rawResponse = $response;
    $split = preg_split('/\r\n\r\n/', $response, 2);
    if (!isset($split[1])) {
      throw new HttpClientException('Error interpreting response', 0, (object) array(
        'rawResponse' => $response,
      ));
    }
    list($headers, $body) = $split;

    $obj = (object) array(
      'headers' => $headers,
      'body' => $body,
    );

    // Drupal sends errors are via X-Drupal-Assertion-* headers,
    // generated by _drupal_log_error(). Read them to ease debugging.
    if (preg_match_all('/X-Drupal-Assertion-[0-9]+: (.*)\n/', $headers, $matches)) {
      foreach ($matches[1] as $key => $match) {
        $obj->drupalErrors[] = print_r(unserialize(urldecode($match)), 1);
      }
    }
    $matches = array();
    if (preg_match('/HTTP\/1.\d (\d{3}) (.*)/', $headers, $matches)) {
      $obj->responseCode = intVal(trim($matches[1]), 10);
      $obj->responseMessage = trim($matches[2]);

      // Handle HTTP/1.1 100 Continue
      if ($obj->responseCode == 100) {
        return $this->interpretResponse($client, $body);
      }
    }
    return $obj;
  }
}

/**
 * Exception that's used to pass information about the response when
 * a operation fails.
 */
class HttpClientException extends Exception {
  protected $response;

  public function __construct($message, $code = 0, $response = NULL, $exception = NULL) {
    parent::__construct($message, $code);
    $this->response = $response;
  }

  /**
   * Gets the response object, if any.
   */
  public function getResponse() {
    $response = $this->response;
    if (is_object($response)) {
      $response = clone $response;
    }
    return $response;
  }
}

/**
 * A base formatter to format php and json.
 */
class HttpClientBaseFormatter implements HttpClientFormatter {

  const FORMAT_PHP = 'php';
  const FORMAT_JSON = 'json';
  const FORMAT_FORM = 'form';

  protected $mimeTypes = array(
    self::FORMAT_PHP => 'application/vnd.php.serialized',
    self::FORMAT_JSON => 'application/json',
    self::FORMAT_FORM => 'application/x-www-form-urlencoded',
  );

  protected $format;

  public function __construct($format = self::FORMAT_PHP) {
    $this->format = $format;
  }

  /**
   * Serializes arbitrary data.
   *
   * @param mixed $data
   *  The data that should be serialized.
   * @return string
   *  The serialized data as a string.
   */
  public function serialize($data) {
    switch ($this->format) {
      case self::FORMAT_PHP:
        return serialize($data);
        break;
      case self::FORMAT_JSON:
        return drupal_json_encode($data);
        break;
      case self::FORMAT_FORM:
        return http_build_query($data, NULL, '&');
        break;
    }
  }

  /**
   * Unserializes data.
   *
   * @param string $data
   *  The data that should be unserialized.
   * @return mixed
   *  The unserialized data.
   */
  public function unserialize($data) {
    switch ($this->format) {
      case self::FORMAT_PHP:
        if (($response = @unserialize($data)) !== FALSE || $data === serialize(FALSE)) {
          return $response;
        }
        else {
          throw new Exception(t('Unserialization of response body failed.'), 1);
        }
        break;
      case self::FORMAT_JSON:
        $response = drupal_json_decode($data);
        if ($response === NULL) {
          throw new Exception(t('Unserialization of response body failed.'), 1);
        }
        return $response;
        break;
      case self::FORMAT_FORM:
        $response = array();
        parse_str($data, $response);
        return $response;
        break;
    }
  }

  /**
   * Returns the mime type to use.
   */
  public function mimeType() {
    return $this->mimeTypes[$this->format];
  }

  /**
   * Return the mime type that the formatter can parse.
   */
  public function accepts() {
    return $this->mimeType();
  }

  /**
   * Return the content type form the data the formatter generates.
   */
  public function contentType() {
    return $this->mimeType();
  }
}

/**
 * A utility formatter to use for creating assymetrical http client formatters.
 */
class HttpClientCompositeFormatter implements HttpClientFormatter {
  private $send = null;
  private $accept = null;

  /**
   * Creates an assymetrical formatter.
   *
   * @param string|HttpClientFormatter $send
   *  Optional. The formatter to use when sending requests. Accepts one of
   *  the HttpClientBaseFormatter::FORMAT_ constants or a HttpClientFormatter
   *  object. Defaults to form encoded.
   * @param string|HttpClientFormatter $accept
   *  Optional. The formatter to use when parsing responses. Accepts one of
   *  the HttpClientBaseFormatter::FORMAT_ constants or a HttpClientFormatter
   *  object. Defaults to json.
   */
  public function __construct($send = HttpClientBaseFormatter::FORMAT_FORM, $accept = HttpClientBaseFormatter::FORMAT_JSON) {
    if (is_string($send)) {
      $send = new HttpClientBaseFormatter($send);
    }
    if (is_string($accept)) {
      $accept = new HttpClientBaseFormatter($accept);
    }

    $this->send = $send;
    $this->accept = $accept;
  }

  /**
   * Serializes arbitrary data to the implemented format.
   *
   * @param mixed $data
   *  The data that should be serialized.
   * @return string
   *  The serialized data as a string.
   */
  public function serialize($data) {
    return $this->send->serialize($data);
  }

  /**
   * Unserializes data in the implemented format.
   *
   * @param string $data
   *  The data that should be unserialized.
   * @return mixed
   *  The unserialized data.
   */
  public function unserialize($data) {
    return $this->accept->unserialize($data);
  }

  /**
   * Return the mime type that the formatter can parse.
   */
  public function accepts() {
    return $this->accept->mimeType();
  }

  /**
   * Return the content type form the data the formatter generates.
   */
  public function contentType() {
    return $this->send->mimeType();
  }
}

/**
 * Interface implemented by formatter implementations for the http client
 */
interface HttpClientFormatter {
  /**
   * Serializes arbitrary data to the implemented format.
   *
   * @param mixed $data
   *  The data that should be serialized.
   * @return string
   *  The serialized data as a string.
   */
  public function serialize($data);

  /**
   * Unserializes data in the implemented format.
   *
   * @param string $data
   *  The data that should be unserialized.
   * @return mixed
   *  The unserialized data.
   */
  public function unserialize($data);

  /**
   * Return the mime type that the formatter can parse.
   */
  public function accepts();

  /**
   * Return the content type form the data the formatter generates.
   */
  public function contentType();
}

/**
 * Interface that should be implemented by classes that provides a
 * authentication method for the http client.
 */
interface HttpClientAuthentication {
  /**
   * Used by the HttpClient to authenticate requests.
   *
   * @param HttpClientRequest $request
   * @return void
   */
  public function authenticate($request);
}

/**
 * This is a convenience class that allows the manipulation of a http request
 * before it's handed over to curl.
 */
class HttpClientRequest {

  const METHOD_GET = 'GET';
  const METHOD_POST = 'POST';
  const METHOD_PUT = 'PUT';
  const METHOD_DELETE = 'DELETE';

  public $method = self::METHOD_GET;
  public $url = '';
  public $parameters = array();
  public $headers = array();
  public $data = NULL;

  /**
   * Allows specification of additional custom options.
   */
  public $options = array();

  /**
   * Construct a new client request.
   *
   * @param $url
   *   The url to send the request to.
   * @param $values
   *   An array of values for the object properties to set for the request.
   */
  public function __construct($url, $values = array()) {
    $this->url = $url;
    foreach (get_object_vars($this) as $key => $value) {
      if (isset($values[$key])) {
        $this->$key = $values[$key];
      }
    }
  }

  /**
   * Gets the values of a header, or the value of the header if
   * $treat_as_single is set to true.
   *
   * @param string $name
   * @param string $treat_as_single
   *  Optional. If set to FALSE an array of values will be returned. Otherwise
   *  The first value of the header will be returned.
   * @return string|array
   */
  public function getHeader($name, $treat_as_single = TRUE) {
    $value = NULL;

    if (!empty($this->headers[$name])) {
      if ($treat_as_single) {
        $value = reset($this->headers[$name]);
      }
      else {
        $value = $this->headers[$name];
      }
    }

    return $value;
  }

  /**
   * Returns the headers as a array. Multiple valued headers will have their
   * values concatenated and separated by a comma as per
   * http://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html#sec4.2
   *
   * @return array
   */
  public function getHeaders() {
    $headers = array();
    foreach ($this->headers as $name => $values) {
      $headers[] = $name . ': ' . join($values, ', ');
    }
    return $headers;
  }

  /**
   * Appends a header value. Use HttpClientRequest::setHeader() it you want to
   * set the value of a header.
   *
   * @param string $name
   * @param string $value
   * @return void
   */
  public function addHeader($name, $value) {
    if (!is_array($value)) {
      $this->headers[$name][] = $value;
    }
    else {
      $values = isset($this->headers[$name]) ? $this->headers[$name] : array();
      $this->headers[$name] = $values + $value;
    }
  }

  /**
   * Sets a header value.
   *
   * @param string $name
   * @param string $value
   * @return void
   */
  public function setHeader($name, $value) {
    if (!is_array($value)) {
      $this->headers[$name][] = $value;
    }
    else {
      $this->headers[$name] = $value;
    }
  }

  /**
   * Removes a header.
   *
   * @param string $name
   * @return void
   */
  public function removeHeader($name) {
    unset($this->headers[$name]);
  }

  /**
   * Returns the url taken the parameters into account.
   */
  public function url() {
    if (empty($this->parameters)) {
      return $this->url;
    }
    $total = array();
    foreach ($this->parameters as $k => $v) {
      if (is_array($v)) {
        foreach ($v as $va) {
          $total[] = HttpClient::urlencode_rfc3986($k) . "[]=" . HttpClient::urlencode_rfc3986($va);
        }
      }
      else {
        $total[] = HttpClient::urlencode_rfc3986($k) . "=" . HttpClient::urlencode_rfc3986($v);
      }
    }
    $out = implode("&", $total);
    return $this->url . '?' . $out;
  }
}
