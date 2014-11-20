<?php

/**
 * @file
 * MailUp Client classes
 *
 * Wrappers around the mailup service, utilising HttpClient
 */

class ConsoleRecipientItem {

  function __construct($data) {

    $base_fields = array('idRecipient', 'Name', 'Email', 'MobilePrefix', 'MobileNumber');

    foreach ($data as $key => $value) {
      if (in_array($key, $base_fields)) {
        $this->{$key} = $value;
      }
      else {
        $this->Fields[] = array(
          'Description' => NULL,
          'Id' => $key,
          'Value' => $value,
        );
      }
    }
  }

  function getRecipientID() {
    return empty($this->idRecipient) ? NULL : $this->idRecipient;
  }
}

class MailUpHttpClient extends HttpClient {

  public function __construct($authentication = NULL, $formatter = NULL, $request_alter = FALSE, $delegate = NULL) {
    parent::__construct($authentication, $formatter, $request_alter, $delegate);
    $this->options['curlopts'] = array(
      CURLOPT_CAINFO => dirname(__FILE__) . '/cacert.pem'
    );
  }

}

/**
 * One way formatter, for accepting plain html from MailUp API
 * Should only be used as *accept* formatter in HttpClientCompositeFormatter
 */
class HttpClientMailUpHtmlFormatter implements HttpClientFormatter {

  protected $mimeType;

  public function __construct() {
    $this->mimeType = 'text/html';
  }

  // When class is used correctly, this method is redundant
  // Required by HttpClientFormatter interface
  public function serialize($data) {
    return '';
  }
  // When class is used correctly, this method is redundant
  // Required by HttpClientFormatter interface
  public function contentType() {
    return 'text/plain';
  } 

  // Just return the plain data
  public function unserialize($data) {
    return (string)$data;
  }

  public function accepts() {
    return $this->mimeType;
  }

  public function mimeType() {
    return $this->mimeType;
  }

}

/**
 * Authentication class for HTTP Client
 */
class HttpClientMailUpBasicAuth implements HttpClientAuthentication {

  function __construct($client_id, $client_secret) {
    $this->client_id = $client_id;
    $this->client_secret = $client_secret;
  }

  public function authenticate($request) {
    $request->setHeader('Authorization', 'Basic ' . base64_encode($this->client_id . ":" . $this->client_secret));
  }
}


/**
 * Authentication class for HTTP Client
 */
class HttpClientMailUpOAuth2 implements HttpClientAuthentication {

  function __construct($access_token) {
    $this->access_token = $access_token;
  }

  public function authenticate($request) {
    $request->setHeader('Authorization', 'Bearer ' . $this->access_token);
  }
}


class MailUpException extends Exception {

  var $statusCode;

  function __construct($inStatusCode, $inMessage) {
    parent::__construct($inMessage);
    $this->statusCode = $inStatusCode;
  }

  function getStatusCode() {
    return $this->statusCode;
  }

  function setStatusCode($inStatusCode) {
    $this->statusCode = $inStatusCode;
  }
}


class MailUp {

  const MAILUP_CLIENT_ID = '83844ffd-3110-4efa-bb92-87e20c0e305d';
  const MAILUP_CLIENT_SECRET = '983406a4-6a0f-4fb1-b348-f200fd471bee';

  const LOGON_ENDPOINT = 'https://services.mailup.com/Authorization/OAuth/LogOn';
  const TOKEN_ENDPOINT = 'https://services.mailup.com/Authorization/OAuth/Token';
  const AUTH_ENDPOINT = 'https://services.mailup.com/Authorization/OAuth/Authorization';

  const CONSOLE_ENDPOINT = 'https://services.mailup.com/API/v1.1/Rest/ConsoleService.svc';
  const PUBLIC_ENDPOINT = 'https://services.mailup.com/API/v1.1/Rest/PublicService.svc';
  const STATS_ENDPOINT = 'https://services.mailup.com/API/v1.1/Rest/MailStatisticsService.svc';

  var $callbackUri;
  var $accessToken;
  var $refreshToken;
  var $tokenExpires;

  var $allowRefresh;

  function __construct($username, $password) {
    $this->allowRefresh = TRUE;
    $this->loadTokensFromSession();

    $this->username = $username;
    $this->password = $password;
  }

  static function clearSessionTokens() {
    unset($_SESSION['mailup_access_token']);
    unset($_SESSION['mailup_refresh_token']);
    unset($_SESSION['mailup_token_expiry']);
  }

  function storeToken() {
    $_SESSION['mailup_access_token'] = $this->accessToken;
    $_SESSION['mailup_refresh_token'] = $this->refreshToken;
    $_SESSION['mailup_token_expiry'] = $this->tokenExpires;
  }

  function updateTokenFromResponse($response) {
    $this->accessToken = $response['access_token'];
    $this->refreshToken = $response['refresh_token'];
    $this->tokenExpires = time() + $response['expires_in'];

    $this->storeToken();
  }

  function requestTrialActivation($data) {
    return $this->callMethodHttpBasicAuth(self::PUBLIC_ENDPOINT . '/Console/TrialActivation', 'POST', NULL, $data);
  }

  function getTrialActivationStatus($id, $hash) {

    $status = array(
      '0' => "Activation not confirmed",
      '1' => "Your MailUp account is being created. Account credentials will be sent to your email",
      '2' => "Your trial has been activated and you'll receive the account credentials in your inbox",
      '3' => "An error occurred and an alert has been notified to our support team in order to solve this issue",
      '4' => "MailUp platform is in maintenance mode, your request will be processed as soon as the platform will be restored",
      '5' => "The trial activation is in progress, soon you’ll receive the account credentials",
      '6' => "An error occurred and an alert has been notified to our support team in order to solve this issue",
      '7' => "The trial activation is in progress, soon you’ll receive the account credentials"
    );


    $result = $this->callMethodHttpBasicAuth(self::PUBLIC_ENDPOINT . '/Console/TrialActivationStatus', 'POST', NULL, array('Id' => $id, 'Hash' => $hash));
    
    return array(
      'code' => $result['Code'],
      'message' => $status[$result['Code']]
    );
  }

  function retrieveAccessToken() {

    if (empty($this->username) || empty($this->password)) {
      throw new MailUpException(1000, 'Unable to access MailUp REST API without setting credentials.');
    }

    $authentication = new HttpClientMailUpBasicAuth(self::MAILUP_CLIENT_ID, self::MAILUP_CLIENT_SECRET);
    $formatter = new HttpClientBaseFormatter(HttpClientBaseFormatter::FORMAT_JSON);
    $http_client = new MailUpHttpClient($authentication, $formatter);

    $params = array(
      'grant_type' => 'password',
      'username' => $this->username,
      'password' => $this->password
    );

    try {
      $result = $http_client->post(self::TOKEN_ENDPOINT, NULL, $params);
      $this->updateTokenFromResponse( $result );

      return $this->accessToken;
    }
    catch (HttpClientException $e) {
      $this->handleHttpClientException($e);
    }
  }


  function refreshAccessToken() {

    $this->allowRefresh = FALSE;

    $authentication = new HttpClientMailUpBasicAuth(self::MAILUP_CLIENT_ID, self::MAILUP_CLIENT_SECRET);
    $formatter = new HttpClientBaseFormatter(HttpClientBaseFormatter::FORMAT_JSON);
    $http_client = new MailUpHttpClient($authentication, $formatter);

    $params = array(
      'grant_type' => 'refresh_token',
      'refresh_token' => $this->refreshToken
    );

    try {
      $result = $http_client->post(self::TOKEN_ENDPOINT, NULL, $params);
      $this->updateTokenFromResponse( $result );
      return TRUE;
    }
    catch (HttpClientException $e) {
      $this->handleHttpClientException($e);
      return FALSE;
    }
  }


  function loadTokensFromSession() {
    if (isset($_SESSION["mailup_access_token"])) $this->accessToken = $_SESSION["mailup_access_token"];
    if (isset($_SESSION["mailup_refresh_token"])) $this->refreshToken = $_SESSION["mailup_refresh_token"];
    if (isset($_SESSION["mailup_token_expiry"])) $this->tokenExpires = $_SESSION["mailup_token_expiry"];
  }


  function logOnWithPassword($username, $password) {
    return $this->retrieveAccessToken($username, $password);
  }

  function _hasValidToken() {

    $properties = array('accessToken', 'refreshToken', 'tokenExpires');

    foreach ($properties as $property) {
      if (empty($this->{$property})) {
        return FALSE;
      }
    }

    if (time() > $this->tokenExpires) {
      return FALSE;
    }

    return TRUE;
  }

  function ensureAccessToken() {
    if ($this->allowRefresh) {
      if (empty($this->accessToken)) {
        $this->retrieveAccessToken();
      }
      else {
        if (time() > $this->tokenExpires) {
          $this->refreshAccessToken();
        }
      }
    }
  }

  /**
   * Helper to parse HttpClientException
   * Re-throws as MailUpException
   */
  function handleHttpClientException(HttpClientException $e) {

    // Default message (useful when unreachable host etc)
    $msg = $e->getMessage();
    // Use a non-http code
    $code = 1000;

    // A response was received
    if ($response = $e->getResponse()) {
      // Use the HTTP code
      $code = $response->responseCode;
      // Static message
      if ($static_msg = $this->lookupResponseCode($response->responseCode)) {
        $msg = $static_msg;
      }
      // Try and better this by parsing the actual error from message body
      if (!empty($response->body) && $parsed_body = json_decode($response->body)) {
        if (!empty($parsed_body->error)) {
          $msg = $parsed_body->error;
          if (!empty($parsed_body->error_description)) {
            $msg = $parsed_body->error_description;
          }
          $msg .= ' (HTTP ' . $response->responseCode . ')';
        }
      }
    }

    // Re-throw
    throw new MailUpException($code, $msg);
  }

  function callMethodFrontend($url, $http_method='GET', $params=array(), $data=array()) {

    $formatter = new HttpClientCompositeFormatter(
      new HttpClientBaseFormatter(HttpClientBaseFormatter::FORMAT_FORM),
      new HttpClientMailUpHtmlFormatter()
    );

    $http_client = new MailUpHttpClient(NULL, $formatter);

    $request = array(
      'method' => $http_method,
      'parameters' => $params
    );

    if (!empty($data)) {
      $request['data'] = $data;
    }
    try {
      $response = $http_client->execute(new HttpClientRequest($url, $request));
      return $response;
    }
    catch (HttpClientException $e) {
      return $this->handleHttpClientException($e);
    }
  }


  function callMethodHTTP($url, $http_method='GET', $params=array(), $data=array()) {
    $http_client = new MailUpHttpClient(NULL, new HttpClientXMLFormatter());

    $request = array(
      'method' => $http_method,
      'parameters' => $params
    );

    if (!empty($data)) {
      $request['data'] = $data;
    }

    try {
      $response = $http_client->execute(new HttpClientRequest($url, $request));
      return $response;
    }
    catch (HttpClientException $e) {
      return $this->handleHttpClientException($e);
    }
  }

  function callMethodHttpBasicAuth($url, $http_method='GET', $params=array(), $data=array()) {

    $authentication = new HttpClientMailUpBasicAuth(self::MAILUP_CLIENT_ID, self::MAILUP_CLIENT_SECRET);
    $formatter = new HttpClientBaseFormatter(HttpClientBaseFormatter::FORMAT_JSON);

    $http_client = new MailUpHttpClient($authentication, $formatter);

    $request = array(
      'method' => $http_method,
      'parameters' => $params
    );

    if (!empty($data)) {
      $request['data'] = $data;
    }

    try {
      return $http_client->execute(new HttpClientRequest($url, $request));  
    }
    catch (HttpClientException $e) {
      return $this->handleHttpClientException($e);
    }
  }

  function callMethodREST($url, $http_method='GET', $params=array(), $data=array()) {

    // Will request or refresh token
    // only when required.
    $this->ensureAccessToken();

    $authentication = new HttpClientMailUpOAuth2($this->accessToken);
    $formatter = new HttpClientBaseFormatter(HttpClientBaseFormatter::FORMAT_JSON);

    $http_client = new MailUpHttpClient($authentication, $formatter);

    try {
      $request = array(
        'method' => $http_method,
        'parameters' => $params
      );
      if (!empty($data)) {
        $request['data'] = $data;
      }
      return $http_client->execute(new HttpClientRequest($url, $request));
    }
    catch (HttpClientException $e) {

      // Get object containing HTTP response
      if ($response = $e->getResponse()) {

        // DELETE seems to
        if ($response->responseCode == 200) {
          return TRUE;
        }

        // Unauthorized
        if ($response->responseCode == 401) {
          // Try refreshing the access token if we can
          if ($this->allowRefresh && $this->refreshAccessToken()) {
            // And try again
            return $this->callMethodREST($url);
          }
        }
      }

      // Something else went wrong
      return $this->handleHttpClientException($e);
    }
  }

  function callBasicAuthMethod($url) {

  }

  function lookupResponseCode($code=NULL) {

    $response_codes = array(
      '200' => t('200: OK.'),
      '201' => t('201: The request has been accepted for processing, but the processing has not been completed.'),
      '203' => t('203: The server successfully processed the request, but is returning information that may be from another source.'),
      '204' => t('204: The server successfully processed the request, but is not returning any content.'),
      '205' => t('205: The server successfully processed the request, but is not returning any content. Requires the requester to reset the document view.'),
      '206' => t('206: The server is delivering only part of the resource due to a range header sent by the client.'),
      '207' => t('207: The message body that follows is an XML message and can contain a number of separate response codes.'),
      '208' => t('208: The members of a DAV binding have already been enumerated in a previous reply to this request, and are not being included again.'),
      '226' => t('226: The server has fulfilled a GET request for the resource, and the response is a representation of the result of one or more instance-manipulations applied to the current instance.'),
      '300' => t('300: Indicates multiple options for the resource that the client may follow.'),
      '301' => t('301: This and all future requests should be directed to the given URI.'),
      '302' => t('302: Required the client to perform a temporary redirect'),
      '303' => t('303: The response to the request can be found under another URI using a GET method.'),
      '304' => t('304: Indicates that the resource has not been modified since the version specified by the request headers If-Modified-Since or If-Match.'),
      '305' => t('305: The requested resource is only available through a proxy, whose address is provided in the response.'),
      '307' => t('307: The request should be repeated with another URI'),
      '308' => t('308: The request, and all future requests should be repeated using another URI.'),
      '400' => t('400: The request cannot be fulfilled due to bad syntax.'),
      '401' => t('401: Authentication faild.'),
      '403' => t('403: The request was a valid request, but the server is refusing to respond to it.'),
      '404' => t('404: The requested resource could not be found but may be available again in the future.'),
      '405' => t('405: A request was made of a resource using a request method not supported by that resource.'),
      '406' => t('406: The requested resource is only capable of generating content not acceptable according to the Accept headers sent in the request'),
      '407' => t('407: The client must first authenticate itself with the proxy.'),
      '408' => t('408: The server timed out waiting for the request.'),
      '409' => t('409: Indicates that the request could not be processed because of conflict in the request, such as an edit conflict in the case of multiple updates.'),
      '410' => t('410: Indicates that the resource requested is no longer available and will not be available again.'),
      '411' => t('411: The request did not specify the length of its content, which is required by the requested resource.'),
      '412' => t('412: The server does not meet one of the preconditions that the requester put on the request.'),
      '413' => t('413: The request is larger than the server is willing or able to process.'),
      '414' => t('414: The URI provided was too long for the server to process'),
      '415' => t('415: The request entity has a media type which the server or resource does not support.'),
      '416' => t('416: The client has asked for a portion of the file, but the server cannot supply that portion.'),
      '417' => t('417: The server cannot meet the requirements of the Expect request-header field.'),
      '419' => t('419: Authentication Timeout denotes that previously valid authentication has expired.'),
      '422' => t('422: The request was well-formed but was unable to be followed due to semantic errors'),
      '423' => t('423: The resource that is being accessed is locked.'),
      '424' => t('424: The request failed due to failure of a previous request'),
      '426' => t('426: The client should switch to a different protocol such as TLS/1.0'),
      '428' => t('428: The origin server requires the request to be conditional.'),
      '429' => t('429: The user has sent too many requests in a given amount of time.'),
      '431' => t('431: The server is unwilling to process the request because either an individual header field, or all the header fields collectively, are too large.'),
      '440' => t('440: Your session has expired.'),
      '444' => t('444: The server did not return any information to the client and close the connection.'),
      '449' => t('449: The request should be retried after performing the appropriate action.'),
      '450' => t('450: Windows Parental Controls are turned on and are blocking access to the given webpage.'),
      '451' => t('451: If there either is a more efficient server to use or the server cannot access the users\' mailbox.'),
      '500' => t('500: Internal Server Error.'),
      '501' => t('501: The server either does not recognize the request method, or it lacks the ability to fulfill the request.'),
      '502' => t('502: The server was acting as a gateway or proxy and received an invalid response from the upstream server.'),
      '503' => t('503: The server is currently unavailable.'),
      '504' => t('504: The server was acting as a gateway or proxy and did not receive a timely response from the upstream server.'),
      '505' => t('505: The server does not support the HTTP protocol version used in the request.'),
      '507' => t('507: The server is unable to store the representation needed to complete the request.'),
      '508' => t('508: The server detected an infinite loop while processing the request.'),
      '509' => t('509: Bandwidth Limit Exceeded.'),
      '510' => t('510: Further extensions to the request are required for the server to fulfill it.'),
      '511' => t('511: The client needs to authenticate to gain network access.'),
      '598' => t('598: The network read timeout behind the proxy to a client in front of the proxy.'),
      '599' => t('599: Network connect timeout behind the proxy to a client in front of the proxy.')
    );

    if (!empty($response_codes[$code])) {
      return $response_codes[$code];
    }

    return FALSE;
  }

}


class MailUpClient extends MailUp {

  var $consoleURL;

  function __construct($username, $password, $consoleURL=NULL) {
    $this->consoleURL = $consoleURL;
    parent::__construct($username, $password);
  }

  function checkAuth($check_console_url=FALSE) {
    if ($check_console_url) {
      if (empty($this->consoleURL)) {
        throw new MailUpException(1000, 'No Console URL supplied');
      }
      else {
        try {
          $this->callMethodFrontend($this->consoleURL . 'frontend/xmlSubscribe.aspx');
        }
        catch (MailUpException $e) {
          throw new MailUpException(1000, 'Invalid Console URL (' . $e->getMessage() . ')');
        }
      }
    }
    
    return $this->callMethodREST(self::CONSOLE_ENDPOINT . '/Console/Authentication/Info', 'GET');
  }

  function getLists() {

    $params = array(
      'orderby' => 'idList+asc'
    );

    return $this->callMethodREST(self::CONSOLE_ENDPOINT . '/Console/User/Lists', 'GET', $params);
  }

  function getListGroups($list_id) {
    return $this->callMethodREST(self::CONSOLE_ENDPOINT . '/Console/List/' . $list_id . '/Groups', 'GET');
  }

  function getRecipientByEmail($email) {
    $url = self::CONSOLE_ENDPOINT . '/Console/Recipient/' . $email;

    return $this->callMethodREST($url, 'GET');
  }

  function updateRecipient($recipient) {
    $url = self::CONSOLE_ENDPOINT . '/Console/Recipient/Detail';
    return $this->callMethodREST($url, 'PUT', NULL, $recipient);
  }

  function getRecipientListGroups($recipient_id, $list_id) {
    $url = self::CONSOLE_ENDPOINT . '/Console/List/' . $list_id . '/Recipient/' . $recipient_id . '/Groups';
    return $this->callMethodREST($url, 'GET');
  }

  function getFields($page_size = 100) {

    $url = self::CONSOLE_ENDPOINT . '/Console/Recipient/DynamicFields';
    
    $params = array(
      'pageSize' => $page_size,
      'orderby' => 'Id'
    );

    return $this->callMethodREST($url, 'GET', $params);
  }

  function subscribeToListNew($list_id, $recipients) {
    $url = self::CONSOLE_ENDPOINT . '/Console/List/' . $list_id . '/Recipients';
    return $this->callMethodREST($url, 'POST', array(), $recipients);
  }

  function listUnsubscribe($list_id, $recipient_id) {
    $url = self::CONSOLE_ENDPOINT . '/Console/List/' . $list_id . '/Unsubscribe/' . $recipient_id;
    return $this->callMethodREST($url, 'DELETE');
  }

  function checkSubscriptionStatusFrontEnd($list_guid, $list_id, $email) {
    $url = $this->consoleURL . 'frontend/Xmlchksubscriber.aspx';

    $params['listguid'] = $list_guid;
    $params['list'] = $list_id;
    $params['email'] = $email;

    return $this->callMethodFrontend($url, 'POST', $params);
  }

  function unsubscribeFromListFrontEnd($list_guid, $list_id, $email) {
    $url = $this->consoleURL . 'frontend/xmlunsubscribe.aspx';

    $params['listguid'] = $list_guid;
    $params['list'] = $list_id;
    $params['email'] = $email;

    return $this->callMethodFrontend($url, 'POST', $params);
  }

  function subscribeToListFrontEnd($email, $list_ids, $params) {
    $url = $this->consoleURL . 'frontend/xmlSubscribe.aspx';

    $params['retcode'] = '1'; // Ensure numeric response, not text
    $params['list'] = is_array($list_ids) ? implode(',', $list_ids) : $list_ids;
    $params['email'] = $email;

    return $this->callMethodFrontend($url, 'POST', $params);
  }

  function groupSubscribe($group_id, $recipient_id) {
    $url = self::CONSOLE_ENDPOINT . '/Console/Group/' . $group_id . '/Subscribe/' . $recipient_id;
    return $this->callMethodREST($url, 'POST');
  }

  function groupUnsubscribe($group_id, $recipient_id) {
    $url = self::CONSOLE_ENDPOINT . '/Console/Group/' . $group_id . '/Unsubscribe/' . $recipient_id;
    return $this->callMethodREST($url, 'DELETE');
  }

  function getListRecipientsPaged($list_id, $page_size=100, $page_number=0, $type = 'Subscribed') {

    $params = array(
      'pageSize' => $page_size,
      'pageNumber' => $page_number
    );

    return $this->getListRecipients($list_id, $type, $params);
  }

  function getListRecipients($list_id, $type = 'Subscribed', $params=array()) {
    $url = self::CONSOLE_ENDPOINT . '/Console/List/' . $list_id . '/Recipients/' . $type;

    // if ($filter) {
    //   $params['filterby'] = $filter;
    // }
    // if ($sort) {
    //   $params['orderby'] = $sort;
    // }

    return $this->callMethodREST($url, 'GET', $params);
  }

  function enableWebServiceForCurrentIP($console_url) {

    // Extract host, for 'nl_url' param
    $parsed_url = parse_url($console_url);
    $console_hostname = empty($parsed_url['host']) ? $console_url : $parsed_url['host'];

    $params = array(
      'usr' => $this->username,
      'pwd' => $this->password,
      'nl_url' => $console_hostname,
      'ws_name' => 'FrontEnd'
    );

    // Construct endpoint URL from console url
    $endpoint_url = 'http://' . $console_hostname . '/frontend/WSActivation.aspx';
    
    $xml = $this->callMethodHTTP($endpoint_url, 'GET', $params);

    if (isset($xml->mailupBody)) {
      $code = (int)$xml->mailupBody->ReturnCode;
      // Success
      if ($code == 0) {
        return TRUE;
      }
    }
    return FALSE;
  }



}








