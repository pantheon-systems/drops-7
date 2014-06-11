<?php

/**
 * @file
 * Handles api calls, logging in and logging out of LingoTek
 */
/*
 * Session handler for api calls and logging in to the Lingotek platform
 *
 * Note:
 *  This was origionally designed to do API Version 3 calls.  And it should proably merge with the V4 API object at some point.
  `*  With the addition of Basic Auth to the Version 4 API, it can now also be used to make API V4 calls.
 *  This is done by creating a new LingotekSession object and manually setting the URL property to a V4 address.
 */
class LingotekSession {

  const DEFAULT_API_TIMEOUT = 90.0;

  public $url;
  public $community;
  public $login_id;
  public $key;
  public $password;
  private $logged_in = FALSE;
  private $last_login_msg = '';
  private $headers = array();
  //API calls that don't need to log in.
  private $sessionless = array('login', 'keyLogin');

  /*
   * Constructor
   */

  function __construct() {
    $this->url = LINGOTEK_API_SERVER . "/lingopoint/api";
    $this->community = variable_get('lingotek_community', '');
    $this->login_id = variable_get('lingotek_login_id', '');
    $this->key = variable_get('lingotek_login_key', '');
    $this->password = variable_get('lingotek_password', '');
  }

  /*
   * Destructor which logs out of the lingotek platform
   */

  function __destruct() {
    if ($this->isLoggedIn()) {
      $this->logout();
    }
  }

  //--------------------
  //Public Functions

  /*
   * Determines if the user is currently logged in to the lingotek platform
   */
  public function isLoggedIn() {
    return $this->logged_in;
  }

  /*
   * Returns message received from last login attempt
   */
  public function getLastLoginMsg() {
    return $this->last_login_msg;
  }

  /*
   * Determines if the user can log in and if so, logs in
   */

  public function canLogIn() {
    if (!$this->isLoggedIn()) {
      $this->login();
    }
    return $this->isLoggedIn();
  }

  /*
   * Makes a request to a Lingotek API
   *
   * @param $api
   *  name of the api to call
   * @param $params
   *  key/value pairs to call as parameters in the api
   * @param $data
   *  associative array with further information to use if there is an error
   * @param $returnJson
   *  default TRUE, return json response or the all of the data?
   *
   * @return
   *  object representing the json returned by the api
   */

  public function request($api, $params = NULL, $data = NULL, $returnJson = TRUE) {
    if (!$this->isLoggedIn() && !in_array($api, $this->sessionless)) {
      $this->login();
    }

    $query = "";
    if (isset($params)) {
      $query = http_build_query($params, '', '&');
    }

    $this->headers["Content-Type"] = "application/x-www-form-urlencoded;";


    $timeout = variable_get('lingotek_api_timeout', self::DEFAULT_API_TIMEOUT);
    if (!is_numeric($timeout)) {
      // The site is configured with some non-numeric timeout value, go back to the default.
      LingotekLog::error("The current value for the 'lingotek_api_timeout' system variable ('@value') should be a number. Defaulting to @timeout seconds", array('@value' => $timeout, '@timeout' => self::DEFAULT_API_TIMEOUT));
      $timeout = self::DEFAULT_API_TIMEOUT;
    }

    $api_url = $this->url . "/" . $api;
    $params = array(
      'headers' => $this->headers,
      'method' => 'POST',
      'data' => $query,
      'timeout' => $timeout,
        );

    $response = drupal_http_request($api_url, $params);
    
    $message_params = array(
      '@url' => $api_url,
      'api' => 'api-session',
      '@method' => $api,
      '!params' => $params,
      '!request' => array(),
      '!response' => $response
    );
    
    LingotekLog::api('<h1>@method</h1> <strong>API URL:</strong> @url
        <br /><strong>Request Params</strong>: !params<br /><strong>Response:</strong> !response', $message_params);

    if (isset($response->error)) {
      $this->last_login_msg = $response->error;
    }
    else {
      $this->last_login_msg = "";
    }
    if ($returnJson) {
      if (isset($response->data) && ($json = json_decode($response->data)) && $json->results == "success") {
        return $json;
      }
      else {
        LingotekLog::error(
            'API @api FAILED', array(
          '@api' => $api,
          'params' => $params,
          'error' => $this->last_login_msg,
          'response' => isset($json) ? $json : "",
          'logged_in' => $this->logged_in,
            ), 'api');
        $json = new stdClass();
        $json->results = 'fail';
        return $json;
      }
    }
    else {
      return $response;
    }
  }

  /**
   * Downloads a document from Lingotek.
   *
   * @param $api
   *  Name of the api to call
   * @param $params
   *  Key/value pairs to call as parameters in the api
   *
   * @return
   *  Location of the file, or FALSE on error.
   */
  public function downloadTriggered($api, $params) {
    $result = LingotekApi::instance()->request($api, $params);
    $result_decoded = json_decode($result);// non-zipped will be decoded

    if (!$result) {
      LingotekLog::error('Unable to download Lingotek Document.', array());
      return FALSE;
    }
    elseif (isset($result_decoded->results)) {
      LingotekLog::error('Download Lingotek Document Failed. !error ', array('!error' => $result));
      return FALSE;
    }

    $tmpFile = tempnam(file_directory_temp(), "lingotek");
    $fp = fopen($tmpFile, 'w');

    fwrite($fp, $result);
    fclose($fp);

    $text = "";
    $file = FALSE;

    //downloadDocument zips the file up
    if ($api == 'downloadDocument') {
      $zip = new ZipArchive;
      $zip->open($tmpFile);
      $name = $zip->getNameIndex(0);
      $file = $zip->getStream($name);
    }
    else {
      $file = fopen($tmpFile, "r");
    }

    if ($file) {
      while (!feof($file)) {
        $text .= fread($file, 2);
      }
    }
    fclose($file);
    unlink($tmpFile);

    return $text;
  }

  //--------------------
  //Static Functions

  /*
   * Hash the key for logging in for this user.
   */
  public static function create_mac($json_msg) {
    $resp = base64_encode(hash_hmac('sha256', $json_msg, pack('H*', variable_get('lingotek_login_key', '')), TRUE));
    return ($resp);
  }

  //--------------------
  //Private Functions

  /*
   * login to the lingotek platform
   */
  private function login() {

    if ($this->key == "") {
      $params = array(
        'loginId' => $this->login_id,
        'password' => $this->password,
        'community' => $this->community,
      );
      $login = 'login';
    }

    $error = 'unknown error';
    $success = FALSE;
    $response = array();
    $data = $this->request($login, $params, NULL, FALSE);
    if (isset($data->data)) {
      $response = json_decode($data->data);
      if (isset($response->results) && $response->results == "success") {
        $success = TRUE;
        $error = '';
      }
      elseif (isset($response->results) && $response->results == "fail") {
        $error = $response->error;
      }
    }
    elseif (isset($data->error)) {
      $error = $data->error;
    }

    if ($success) {
      $this->headers = array("Cookie" => $data->headers['set-cookie']);
      $this->logged_in = TRUE;
      $this->last_login_msg = '';
    }
    else {
      $this->logged_in = FALSE;
      $this->last_login_msg = $error;
      LingotekLog::error(
          "Unable to log in", array(
        'params' => $params,
        'error' => $error,
        'response' => json_encode($response),
          ), 'session'
      );
    }
  }

  /*
   * logout from the lingotek platform
   */

  public function logout() {
    $json = $this->request('logout');
    if ($json->results == "success") {
      $this->logged_in = FALSE;
    }
    else {
      $this->logged_in = TRUE;
      LingotekLog::error('Unable to log out.');
    }
  }

}
