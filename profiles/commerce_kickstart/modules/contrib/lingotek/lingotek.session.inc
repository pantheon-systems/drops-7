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

    $response = drupal_http_request(
        $this->url . "/" . $api, array(
      'headers' => $this->headers,
      'method' => 'POST',
      'data' => $query,
      'timeout' => $timeout,
        )
    );

    if ($returnJson) {
      if (isset($response->data) && ($json = json_decode($response->data)) && $json->results == "success") {
        return $json;
      }
      else {
        LingotekLog::error(
            'API @api FAILED', array(
          '@api' => $api,
          'params' => $params,
          'error' => isset($response->error) ? $response->error : "",
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
  public function download($api, $params) {
    $result = LingotekApi::instance()->request($api, $params);

    if (!$result) {
      LingotekLog::error('Unable to download Lingotek Document.');
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
  private function login($version = "3.11", $xml = FALSE) {

    if ($this->key == "") {
      $params = array(
        'userLogin' => $this->login_id,
        'password' => $this->password,
        'version' => $version,
        'community' => $this->community,
      );
      $login = 'login';
    }
    else {
      $arr = array(
        'community' => $this->community,
        'login_id' => $this->login_id,
        'time' => time(),
      );
      $json_str = json_encode($arr);

      $params = array(
        "auth_json" => $json_str,
        "hmac" => LingotekSession::create_mac($json_str),
        "version" => $version,
        "returnXML" => $xml,
      );
      $login = 'keyLogin';
    }

    $data = $this->request($login, $params, NULL, FALSE);

    if (!isset($data->error) && ($response = json_decode($data->data)) && $response->results == "success") {
      $this->headers = array("Cookie" => $data->headers['set-cookie']);
      $this->logged_in = TRUE;
    }
    else {
      $this->logged_in = FALSE;
      if (isset($data->error) && $data->error) {
        LingotekLog::error(
            "Unable to log in", array(
          'params' => $params,
          'error' => $data->error,
          'response' => isset($response) ? $response : "",
            ), 'session'
        );
      }
      else {
        LingotekLog::warning(
            "Unable to log in", array(
          'params' => $params,
          'response' => isset($response) ? $response : "",
            ), 'session'
        );
      }
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
