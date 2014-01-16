<?php

require_once 'Interface.php';

require_once realpath(dirname(__FILE__)) . '/../Exception.php';

/**
 * It's incorrect to test for the function itself. Since we know exactly when the
 * json_decode function was introduced. So we test the PHP version instead.
 */
if (version_compare(PHP_VERSION, '5.2.0', '<')) {
    throw new Exception('Your PHP version is too old: install the PECL JSON extension');
}
else if (!function_exists('json_decode')) {
    throw new Exception('The JSON extension is missing: install it.');
}

/**
 * Check if the cURL extension is enabled.
 *
 */
if (!extension_loaded('curl')) {
    throw new Exception('Please install the PHP cURL extension');
}


/**
 *   Services_Paymill cURL HTTP client
 */
class Services_Paymill_Apiclient_Curl implements Services_Paymill_Apiclient_Interface
{

    /**
     * Paymill API merchant key
     *
     * @var string
     */
    private $_apiKey = null;
    private $_responseArray = null;

    /**
     *  Paymill API base url
     *
     *  @var string
     */
    private $_apiUrl = '/';

    const USER_AGENT = 'Paymill-php/0.0.2';

    public static $lastRawResponse;
    public static $lastRawCurlOptions;

    /**
     * cURL HTTP client constructor
     *
     * @param string $apiKey
     * @param string $apiEndpoint
     * @param array $extracURL
     *   Extra cURL options. The array is keyed by the name of the cURL
     *   options.
     */
    public function __construct($apiKey, $apiEndpoint, $extracURL = array())
    {
        $this->_apiKey = $apiKey;
        $this->_apiUrl = $apiEndpoint;
        /**
         * Proxy support. The proxy can be SOCKS5 or HTTP.
         * Also the connection could be tunneled through.
         */
        if (!empty($extracURL)) {
            $this->_extraOptions = $extracURL;
        }
    }

    /**
     * Perform API and handle exceptions
     *
     * @param $action
     * @param array $params
     * @param string $method
     * @return mixed
     * @throws Services_Paymill_Exception
     * @throws Exception
     */
    public function request($action, $params = array(), $method = 'POST')
    {
        if (!is_array($params))
            $params = array();

        try {
            $this->_responseArray = $this->_requestApi($action, $params, $method);
            $httpStatusCode = $this->_responseArray['header']['status'];
            if ($httpStatusCode != 200) {
                $errorMessage = 'Client returned HTTP status code ' . $httpStatusCode;
                if (isset($this->_responseArray['body']['error'])) {
                    $errorMessage = $this->_responseArray['body']['error'];
                }
                $responseCode = '';
                if (isset($this->_responseArray['body']['response_code'])) {
                    $responseCode = $this->_responseArray['body']['response_code'];
                }
                if ($responseCode === '' && isset($this->_responseArray['body']['data']['response_code'])) {
                    $responseCode = $this->_responseArray['body']['data']['response_code'];
                }

                return array('data' => array(
                                 'error' => $errorMessage,
                                 'response_code' => $responseCode,
                                 'http_status_code' => $httpStatusCode
                             ));
            }

            return $this->_responseArray['body'];
        } catch (Exception $e) {
            return array("data" => array("error" => $e->getMessage()));
        }
    }

    /**
     * Perform HTTP request to REST endpoint
     *
     * @param string $action
     * @param array $params
     * @param string $method
     * @return array
     */
    protected function _requestApi($action = '', $params = array(), $method = 'POST')
    {
        $curlOpts = array(
            CURLOPT_URL => $this->_apiUrl . $action,
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_CUSTOMREQUEST => $method,
            CURLOPT_USERAGENT => self::USER_AGENT,
            CURLOPT_SSL_VERIFYPEER => true,
            CURLOPT_CAINFO => realpath(dirname(__FILE__)) . DIRECTORY_SEPARATOR . 'paymill.crt',
        );

        // Add extra options to cURL if defined.
        if (!empty($this->_extraOptions)) {
            $curlOpts += $this->_extraOptions;
        }

        if (Services_Paymill_Apiclient_Interface::HTTP_GET === $method) {
            if (0 !== count($params)) {
                $curlOpts[CURLOPT_URL] .= false === strpos($curlOpts[CURLOPT_URL], '?') ? '?' : '&';
                $curlOpts[CURLOPT_URL] .= http_build_query($params, null, '&');
            }
        } else {
            $curlOpts[CURLOPT_POSTFIELDS] = http_build_query($params, null, '&');
        }

        if ($this->_apiKey) {
            $curlOpts[CURLOPT_USERPWD] = $this->_apiKey . ':';
        }

        $curl = curl_init();
        curl_setopt_array($curl, $curlOpts);
        $responseBody = curl_exec($curl);
        self::$lastRawCurlOptions = $curlOpts;
        self::$lastRawResponse = $responseBody;
        $responseInfo = curl_getinfo($curl);
        if ($responseBody === false) {
            $responseBody = array('error' => curl_error($curl));
        }
        curl_close($curl);

        if ('application/json' === $responseInfo['content_type']) {
            $responseBody = json_decode($responseBody, true);
        }

        return array(
            'header' => array(
                'status' => $responseInfo['http_code'],
                'reason' => null,
            ),
            'body' => $responseBody
        );
    }

    /**
     * Returns the response of the request as an array.
     * @return mixed Response
     * @todo Create Unit Test
     */
    public function getResponse()
    {
        return $this->_responseArray;
    }

}