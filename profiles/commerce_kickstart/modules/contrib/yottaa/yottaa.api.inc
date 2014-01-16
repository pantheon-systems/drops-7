<?php

/**
 * @file
 * Base Yottaa API Class for PHP based plugins.
 */

class YottaaAPI {

  /**
   * The endpoint of the production API.
   */
  const PROD_API = 'https://api.yottaa.com/';

  /**
   * The endpoint of the dev API.
   */
  const DEV_API = 'https://api-dev.yottaa.com/';

  /**
   * The partner id for creating new account.
   */
  const PROD_PARTNER_ID = '4d34f75b74b1553ba500007f';

  /**
   * The partner api key for creating new account.
   */
  const PROD_PARTNER_API_KEY = '455df7500258012f663b12313d145ceb';

  /**
   * The partner id for creating new account on dev.
   */
  const DEV_PARTNER_ID = '51a77cebebe2bb6fb00005a7';

  /**
   * The partner api key for creating new account on dev.
   */
  const DEV_PARTNER_API_KEY = '241e2e90ab730130d161123138106137';

  /**
   * The URL of the production API.
   */
  protected $api;

  /**
   * The partner id for creating new account.
   */
  protected $partnerId;

  /**
   * The partner api key for creating new account.
   */
  protected $partnerApiKey;

  /**
   * The API key.
   */
  protected $key;

  /**
   * The user id.
   */
  protected $uid;

  /**
   * The site id.
   */
  protected $sid;

  /**
   * Constructor.
   *
   * @param string $key
   *   API Key
   * @param string $uid 
   *   User Id
   * @param string $sid 
   *   Site Id
   * @param bool $dev_mode
   *   Use dev api if true
   * @param null $partner
   *   Custom partner id and partner key
   */
  public function __construct($key, $uid, $sid, $dev_mode = FALSE, $partner = NULL) {
    $this->key = $key;
    $this->uid = $uid;
    $this->sid = $sid;

    if ($dev_mode) {
      $this->api = YottaaAPI::DEV_API;
      $this->partner_id = YottaaAPI::DEV_PARTNER_ID;
      $this->partner_api_key = YottaaAPI::DEV_PARTNER_API_KEY;
    }
    else {
      $this->api = YottaaAPI::PROD_API;
      $this->partner_id = YottaaAPI::PROD_PARTNER_ID;
      $this->partner_api_key = YottaaAPI::PROD_PARTNER_API_KEY;
    }

    if (!empty($partner)) {
      if (isset($partner["id"])) {
        $this->partner_id = $partner["id"];
      }
      if (isset($partner["key"])) {
        $this->partner_api_key = $partner["key"];
      }
    }
  }

  /**
   * Checks if tha API class instance has all required parameters.
   *
   * @return bool
   *   True, if one of the required settings is empty.
   */
  public function isEmpty() {
    return empty($this->key) || empty($this->uid) || empty($this->sid);
  }

  /**
   * Creates an account using the partner account.
   *
   * @param string $name
   *   User's name.
   * @param string $email
   *   User's email
   * @param string $phone
   *   User's telephone number
   * @param string $site
   *   Website url.
   *
   * @return mixed
   *   Returns an array or bool.
   */
  public function createAccount($name, $email, $phone, $site) {
    if (is_array($name)) {
      $fname = isset($name['first_name']) ? $name['first_name'] : "";
      $lname = isset($name['last_name']) ? $name['last_name'] : "";
    }
    else {
      list($fname, $lname) = explode(" ", $name);
    }

    return $this->call('partners/' . $this->partner_id . '/accounts', array(
      'first_name' => $fname,
      'last_name' => $lname,
      'email' => $email,
      'phone' => $phone,
      'site' => $site,
    ), 'POST', $this->partner_api_key);
  }

  /**
   * Returns Yottaa site full settings and performances post-processing.
   *
   * @return array
   *   Yottaa site full settings.
   */
  public function getSettings() {
    $result = $this->call('sites/' . $this->sid . '/settings', array(
      'user_id' => $this->uid,
    ), 'GET', $this->key);
    return $this->postProcessingSettings($result);
  }

  /**
   * Returns site status.
   *
   * @return mixed
   *   Site status.
   */
  public function getStatus() {
    return $this->call('sites/' . $this->sid, array(
      'user_id' => $this->uid,
    ), 'GET', $this->key);
  }

  /**
   * Returns site status.
   *
   * @param string $key
   *   API key.
   * @param string $uid
   *   User id.
   * @param string $sid
   *   Site id.
   *
   * @return mixed
   *   Site status.
   */
  public function getRuntimeStatus($key, $uid, $sid) {
    return $this->call('sites/' . $sid, array(
      'user_id' => $uid,
    ), 'GET', $key);
  }

  /**
   * Flushes cache.
   *
   * @return mixed
   *   Returns an array or bool.
   */
  public function flush() {
    return $this->call('sites/' . $this->sid . '/flush_cache', array(
      'user_id' => $this->uid,
    ), 'PUT', $this->key);
  }

  /**
   * Flushes cache by path(s).
   *
   * @param string $path_configs
   *   A path to flush it's cache.
   *
   * @return mixed
   *   Returns an array or bool.
   */
  public function flushPaths($path_configs) {
    $result = $this->call('sites/' . $this->sid . '/purge_cache?user_id=' . $this->uid, $path_configs, 'POST', $this->key, TRUE);
    return array("config" => $path_configs, "result" => $result);
  }

  /**
   * Pauses optimizer to bypass mode or transparent proxy mode.
   *
   * @param string $mode
   *   The mode.
   *
   * @return mixed
   *   Returns an array or bool.
   */
  public function pause($mode = 'bypass') {
    $action = $mode == 'transparent proxy' ? 'transparent' : 'pause';
    return $this->call('optimizers/' . $this->sid . '/' . $action, array(
      'user_id' => $this->uid,
    ), 'PUT', $this->key);
  }

  /**
   * Transparent optimization.
   */
  public function transparent() {
    return $this->pause('transparent proxy');
  }

  /**
   * Bypass optimization.
   */
  public function bypass() {
    return $this->pause();
  }

  /**
   * Resumes optimization.
   *
   * @return mixed
   *   Array or bool.
   */
  public function resume() {
    return $this->call('optimizers/' . $this->sid . '/resume', array(
      'user_id' => $this->uid,
    ), 'PUT', $this->key);
  }

  /**
   * Makes an api call.
   *
   * @param string $path
   *   The path.
   * @param string $params
   *   The parameters.
   * @param string $method
   *   The method.
   * @param string $key
   *   The key.
   * @param bool $post_json
   *   A flag indicating json settings.
   *
   * @return array|mixed
   *   A json decoded array.
   */
  protected function call($path, $params, $method, $key, $post_json = FALSE) {
    $output = $this->postAsync($this->api . $path, $params, $method, $key, $post_json);
    $json_results = json_decode($this->parseHttpResponse($output), TRUE);
    if (!isset($json_results["error"]) && isset($json_results["error_response"])) {
      $json_results["error"] = $json_results["error_response"];
      unset($json_results["error_response"]);
    }
    return $json_results;
  }

  /**
   * Makes an asynchronous REST call.
   *
   * @param string $url
   *   The url to call.
   * @param string $params
   *   The parameters.
   * @param string $method
   *   The method.
   * @param string $api_key
   *   The api key.
   * @param bool $post_json
   *   A flag indicating json settings.
   *
   * @return string
   *   Data from request.
   */
  protected function postAsync($url, $params, $method, $api_key, $post_json = FALSE) {
    if (!$post_json) {
      foreach ($params as $key => &$val) {
        if (is_array($val)) {
          $val = implode(',', $val);
        }
        $post_params[] = $key . '=' . urlencode($val);
      }
      $post_string = implode('&', $post_params);
    }

    $parts = parse_url($url);

    $port = isset($parts['port']) ? $parts['port'] : 443;

    $fp = fsockopen("ssl://" . $parts['host'], $port, $errno, $errstr, 30);

    // Data goes in the path for a GET request.
    if ($post_json) {
      $parts['path'] .= '?' . $parts['query'];
    }
    else {
      $parts['path'] .= '?' . $post_string;
    }

    $post_data = $post_json ? json_encode($params) : "";

    $out = $method . " " . $parts['path'] . " HTTP/1.1\r\n";
    $out .= "Host: " . $parts['host'] . "\r\n";
    $out .= "Content-Type: application/x-www-form-urlencoded\r\n";
    $out .= "Content-Length: " . strlen($post_data) . "\r\n";
    $out .= "YOTTAA-API-KEY: " . $api_key . "\r\n";
    $out .= "Connection: Close\r\n\r\n";

    if ($post_json) {
      $out .= $post_data;
    }

    if ($this->getEnableLoggingParameter() == 1) {
      $this->log($out);
    }

    fwrite($fp, $out);
    $result = "";
    while (!feof($fp)) {
      $result .= fgets($fp, 128);
    }
    fclose($fp);

    if ($this->getEnableLoggingParameter() == 1) {
      $this->log($result);
    }

    return $result;
  }

  /**
   * Checks for a valid http response, unchunks if needed.
   *
   * @param NULL $content
   *   The provided http content.
   *
   * @return bool|string
   *   Http content without headers on success, FALSE on any errors.
   */
  protected function parseHttpResponse($content = NULL) {
    if (empty($content)) {
      return '{"error" : "Empty Response"}';
    }
    // Split into array, headers and content.
    $hunks = explode("\r\n\r\n", trim($content));
    if (!is_array($hunks)) {
      return json_encode(array("error" => trim($content)));
    }
    // Add support for empty body.
    if (count($hunks) < 2) {
      $header = $hunks[count($hunks) - 1];
      $body = "";
    }
    else {
      $header = $hunks[count($hunks) - 2];
      $body = $hunks[count($hunks) - 1];
    }
    $headers = explode("\n", $header);
    unset($hunks);
    unset($header);
    if (!$this->validateHttpResponse($headers)) {
      return json_encode(array("error" => trim($content)));
    }
    if (in_array('Transfer-Coding: chunked', $headers)) {
      return trim($this->unchunkHttpResponse($body));
    }
    else {
      return trim($body);
    }
  }

  /**
   * Validates http responses by checking header.
   *
   * @param NULL $headers
   *   Expects array of headers as argument.
   *
   * @return bool
   *   True if headers are valid.
   */
  protected function validateHttpResponse($headers = NULL) {
    if (!is_array($headers) or count($headers) < 1) {
      return FALSE;
    }
    $return = FALSE;
    switch (trim(strtolower($headers[0]))) {
      case 'http/1.0 100 ok':
      case 'http/1.0 200 ok':
      case 'http/1.1 100 ok':
      case 'http/1.1 200 ok':
        $return = TRUE;
        break;

    }
    return $return;
  }

  /**
   * Un-chunks http content.
   *
   * @param NULL $str
   *   A string.
   *
   * @return bool|NULL|string
   *   Unchunked content on success, FALSE on any errors.
   */
  protected function unchunkHttpResponse($str = NULL) {
    if (!is_string($str) or strlen($str) < 1) {
      return FALSE;
    }
    $eol = "\r\n";
    $add = strlen($eol);
    $tmp = $str;
    $str = '';
    do {
      $tmp = ltrim($tmp);
      $pos = strpos($tmp, $eol);
      if ($pos === FALSE) {
        return FALSE;
      }
      $len = hexdec(substr($tmp, 0, $pos));
      if (!is_numeric($len) or $len < 0) {
        return FALSE;
      }
      $str .= substr($tmp, ($pos + $add), $len);
      $tmp = substr($tmp, ($len + $pos + $add));
      $check = trim($tmp);
    } while (!empty($check));
    unset($tmp);
    return $str;
  }

  /**
   * Updates all Yottaa parameters.
   *
   * @param string $key
   *   The API key.
   * @param string $uid
   *   The user id.
   * @param string $sid
   *   The site id.
   *
   * @return void
   *   Nothing to return.
   */
  public function updateParameters($key, $uid, $sid) {
    $this->key = $key;
    $this->uid = $uid;
    $this->sid = $sid;
  }

  /**
   * Deletes all Yottaa parameters.
   *
   * @return void
   *   Nothing to return.
   */
  public function deleteParameters() {
    $this->key = NULL;
    $this->uid = NULL;
    $this->sid = NULL;
  }

  /**
   * Post-processes Yottaa site settings.
   *
   * @param string $json_output
   *   Json output.
   *
   * @return array
   *   Returns the parameter. What a useful function.
   */
  protected function postProcessingSettings($json_output) {
    return $json_output;
  }

  /**
   * Logs a message.
   *
   * @param string $message
   *   A message to log.
   *
   * @return void
   *   Silence is golden.
   */
  public function log($message) {
  }

  /**
   * Returns auto clear cache parameter.
   *
   * @return int
   *   Logging disabled by default.
   */
  public function getEnableLoggingParameter() {
    return 0;
  }

  /**
   * Returns true for live status codes.
   *
   * @param string $status
   *   The status.
   *
   * @return bool
   *   True if "live".
   */
  public function isLive($status) {
    return "live" == $status;
  }

  /**
   * Returns true for paused status codes.
   *
   * @param string $status
   *   The status.
   *
   * @return bool
   *   True if "bypass".
   */
  public function isBypass($status) {
    return "bypass" == $status;
  }

  /**
   * Returns true for paused status codes.
   *
   * @param string $status
   *   The status.
   *
   * @return bool
   *   True if "transparent proxy".
   */
  public function isTransparent($status) {
    return "transparent proxy" == $status;
  }

  /**
   * Returns true for preview status codes.
   *
   * @param string $status
   *   The status. Again.
   *
   * @return bool
   *   True if "preview".
   */
  public function isPreview($status) {
    return "preview" == $status;
  }

  /**
   * Returns true for paused status codes.
   *
   * @param string $status
   *   The status.
   *
   * @return bool
   *   True if "bypass" or "transparent proxy".
   */
  public function isPaused($status) {
    return $this->isBypass($status) || $this->isTransparent($status);
  }

  /**
   * Returns true for valid status codes for both live and paused.
   *
   * @param string $status
   *   The status.
   *
   * @return bool
   *   True if "live" or "paused".
   */
  public function isValidStatus($status) {
    return $this->isLive($status) || $this->isPaused($status);
  }
}
