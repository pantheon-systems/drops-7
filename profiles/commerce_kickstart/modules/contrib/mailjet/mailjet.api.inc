<?php
/**
 * Mailjet Public API / The real-time Cloud Emailing platform
 *
 * Connect your Apps and Make our product yours with our powerful API
 * http://www.mailjet.com/ Mailjet SAS Website
 *
 * @copyright	Copyright (c) 2012-2013, Mailjet SAS, http://www.mailjet.com/Terms-of-use.htm
 * @file
 */

/**
 * Mailjet Public API Main Class
 *
 * This class enables you to connect your Apps and use our powerful API.
 * http://www.mailjet.com/docs/api
 *
 * updated on 2013-08-11
 *
 * @class		Api
 * @version		0.2
 */
class MailJet
{

    /**
     * Mailjet API Key
     * You can edit directly and add here your Mailjet infos
     *
     * @access	private
     * @var		string $_apiKey
     */
    private $_apiKey = '';

    /**
     * Mailjet API Secret Key
     * You can edit directly and add here your Mailjet infos
     *
     * @access	private
     * @var		string $_secretKey
     */
    private $_secretKey = '';

    /**
     * Secure flag to connect through https protocol
     * You can edit directly
     *
     * @access	private
     * @var		boolean $_secure
     */
    private $_secure = TRUE;

    /**
     * Debug flag :
     * 0 none / 1 errors only / 2 all
     * You can edit directly
     *
     * @access	private
     * @var		integer $_debug
     */
    private $_debug = 0;

    /**
     * Echo debug ?
     * If not, you can read and display the html error code block
     * by access the public string $_debugErrorHtml
     * You can edit directly
     *
     * @access	private
     * @var		boolean $_debugEcho
     */
    private $_debugEcho = TRUE;

    /**
     * Default Nb of seconds before updating the cached object
     * If set to 0, Object caching will be disabled
     *
     * @access	private
     * @var		integer $_cache
     */
    private $_cache = 0;

    /**
     * Cache path
     *
     * @access	private
     * @var		string $_cache_path
     */
    private $_cache_path = 'cache/';

    /**
     * API version to use.
     *
     * @access	private
     * @var		string $_version
     */
    private $_version = 'REST';

    /**
     * Output format :
     * php, json, xml, serialize, html, csv
     *
     * @access	private
     * @var		string $_output
     */
    private $_output = 'json';

    /**
     * API URL.
     *
     * @access	private
     * @var		string $_apiUrl
     */
    private $_apiUrl = '';

    /**
     * cURL handle resource
     *
     * @access	private
     * @var		resource $_curl_handle
     */
    private $_curl_handle = NULL;

    /**
     * Singleton pattern : Current instance
     *
     * @access	private
     * @var		resource $_instance
     */
    private static $_instance = NULL;

    /**
     * Response of the API
     *
     * @access	private
     * @var		mixed $_response
     */
    private $_response = NULL;

    /**
     * Response code of the API
     *
     * @access	private
     * @var		integer $_response_code
     */
    private $_response_code = 0;

    /**
     * Boolean FALSE or Array of POST args
     *
     * @access	private
     * @var		mixed $_request_post
     */
    private $_request_post = FALSE;

    /**
     * Full Call URL for debugging purpose
     *
     * @access	private
     * @var		string $_debugCallUrl
     */
    private $_debugCallUrl = '';

    /**
     * Method for debugging purpose
     *
     * @access	private
     * @var		string $_debugMethod
     */
    private $_debugMethod = '';

    /**
     * Request for debugging purpose
     *
     * @access	private
     * @var		string $_debugRequest
     */
    private $_debugRequest = '';

    /**
     * Error as a HTML table
     *
     * @access	private
     * @var		string $_debugErrorHtml
     */
    private $_debugErrorHtml = '';

    /**
     * Constructor
     *
     * Set $_apiKey and $_secretKey if provided & Update $_apiUrl with protocol
     *
     * @access	public
     * @uses	Mailjet::Api::$_apiKey
     * @uses	Mailjet::Api::$_secretKey
     * @uses	Mailjet::Api::$_version
     * @param string $apiKey    Mailjet API Key
     * @param string $secretKey Mailjet API Secret Key
     */
    public function __construct($apiKey = FALSE, $secretKey = FALSE)
    {
        if ( $apiKey )		$this->_apiKey = $apiKey;
        if ( $secretKey )	$this->_secretKey = $secretKey;
        $this->_apiUrl = (($this->_secure) ? 'https' : 'http').'://api.mailjet.com/v3/'.$this->_version;
    }

    /**
     * Singleton pattern :
     * Get the instance of the object if it already exists
     * or create a new one.
     *
     * @access	public
     * @uses	Mailjet::Api::$_instance
     *
     * @return resource instance
     */
    public static function getInstance()
    {
        if (!(self::$_instance instanceof self))
            self::$_instance = new self();

        return self::$_instance;
    }

    /**
     *
     * @param unknown_type $version
     */
    public function data($method, $id, $type = 'HTML', $contType = 'text:html', $params = array(), $request = 'GET', $lastID = null)
    {
    	$is_json_put = (isset($params['ID']) && !empty($params['ID']));
    	if ($this->_debug != 0) {
    		$this->_debugMethod = $method;
    		$this->_debugRequest = $request;
    	}

    	$this->_debugCallUrl = $this->_apiUrl = $url = (($this->_secure) ? 'https' : 'http').'://api.mailjet.com/v3/DATA/' . $method .'/'.$id.'/' .$type
    	. '/' . $contType;

    	if(is_null($this->_curl_handle))
    		$this->_curl_handle = curl_init();

    	curl_setopt($this->_curl_handle, CURLOPT_URL, $url);
    	curl_setopt($this->_curl_handle, CURLOPT_RETURNTRANSFER, 1);
    	curl_setopt($this->_curl_handle, CURLOPT_HTTPHEADER, array("Content-Type: ".$contType));
    	curl_setopt($this->_curl_handle, CURLOPT_SSL_VERIFYPEER, FALSE);
    	curl_setopt($this->_curl_handle, CURLOPT_SSL_VERIFYHOST, 2);
    	if (drupal_get_profile() == 'standard') {
          curl_setopt($this->_curl_handle, CURLOPT_USERAGENT, 'drupal-3.0');
    	}
    	elseif (drupal_get_profile() == 'commerce_kickstart') {
    	  curl_setopt($this->_curl_handle, CURLOPT_USERAGENT, 'kickstart');
    	}
    	curl_setopt($this->_curl_handle, CURLOPT_USERPWD, $this->_apiKey.':'.$this->_secretKey);

    	if ($lastID) {
    		$this->_debugCallUrl = $this->_apiUrl = $this->_apiUrl . '/' . $lastID;
    	}

    	switch ($request) {
            case 'GET' :
                curl_setopt($this->_curl_handle, CURLOPT_CUSTOMREQUEST, 'GET');
                curl_setopt($this->_curl_handle, CURLOPT_HTTPGET, TRUE);
                curl_setopt($this->_curl_handle, CURLOPT_POSTFIELDS, NULL);
                $this->_request_post = FALSE;
                break;
            case 'POST':
                curl_setopt($this->_curl_handle, CURLOPT_CUSTOMREQUEST, 'POST');
                curl_setopt($this->_curl_handle, CURLOPT_POST, count($params));
                curl_setopt($this->_curl_handle, CURLOPT_POSTFIELDS, $params);
                $this->_request_post = $params;
                break;
            case 'PUT':
                curl_setopt($this->_curl_handle, CURLOPT_CUSTOMREQUEST, 'PUT');
                curl_setopt($this->_curl_handle, CURLOPT_POSTFIELDS, $params);
            break;
            case 'DELETE':
                curl_setopt($this->_curl_handle, CURLOPT_CUSTOMREQUEST, 'DELETE');
                /*curl_setopt($this->_curl_handle, CURLOPT_POSTFIELDS, http_build_query($params, '', '&'));*/
                $this->_request_post = $params;
                curl_setopt($this->_curl_handle, CURLOPT_POSTFIELDS, json_encode($this->_request_post));
                curl_setopt($this->_curl_handle, CURLOPT_RETURNTRANSFER, TRUE);
                curl_setopt($this->_curl_handle, CURLOPT_HTTPHEADER, array(
                        'Content-Type: application/json',
                        'Content-Length: ' . strlen(json_encode($this->_request_post)))
                );
            break;
            case 'JSON':
                if($is_json_put)
                    curl_setopt($this->_curl_handle, CURLOPT_CUSTOMREQUEST, "PUT");
                else
                    curl_setopt($this->_curl_handle, CURLOPT_CUSTOMREQUEST, "POST");

                $this->_request_post = $params;
                curl_setopt($this->_curl_handle, CURLOPT_POSTFIELDS, json_encode($this->_request_post));
                curl_setopt($this->_curl_handle, CURLOPT_RETURNTRANSFER, TRUE);
                curl_setopt($this->_curl_handle, CURLOPT_HTTPHEADER, array(
                        'Content-Type: application/json',
                        'Content-Length: ' . strlen(json_encode($this->_request_post)))
                );
                break;
        }
    	curl_setopt($this->_curl_handle, CURLOPT_URL, $this->_apiUrl);

    	$buffer = curl_exec($this->_curl_handle);

    	if ($this->_debug > 2)
    		var_dump($buffer);

    	$this->_response_code 	= curl_getinfo($this->_curl_handle,CURLINFO_HTTP_CODE);
    	$this->_response 		= $buffer;

    	if ($this->_debug > 0) {
    		$this->debug();
    	}

    	return ($this->_response_code == 200) ? $this : FALSE;

    }

    /**
     * Destructor
     *
     * Close the cURL handle resource
     *
     * @access	public
     * @uses	Mailjet::Api::$_curl_handle
     */
    public function __destruct()
    {
        if(!is_null($this->_curl_handle))
            curl_close($this->_curl_handle);
        $this->_curl_handle = NULL;
    }

    /**
     * Update or set consumer keys
     *
     * @access	public
     * @uses	Mailjet::Api::$_apiKey
     * @uses	Mailjet::Api::$_secretKey
     * @param string $apiKey    Mailjet API Key
     * @param string $secretKey Mailjet API Secret Key
     */
    public function setKeys($apiKey, $secretKey)
    {
        $this->_apiKey = $apiKey;
        $this->_secretKey = $secretKey;
    }

    /**
     * Get the API Key
     *
     * @access	public
     * @uses	Mailjet::Api::$_apiKey
     *
     * @return string Api Key
     */
    public function getAPIKey()
    {
        return ($this->_apiKey);
    }

    /**
     * Secure or not the transaction through https
     *
     * @access	public
     * @uses	Mailjet::Api::$_apiUrl
     * @param boolean $secure TRUE to secure the transaction, FALSE otherwise
     */
    public function secure($secure = TRUE)
    {
        $this->_secure = $secure;
        $protocol = 'http';
        if ($secure)
            $protocol = 'https';
        $this->_apiUrl = preg_replace('/http(s)?:\/\//', $protocol.'://', $this->_apiUrl);
    }

    /**
     * Get the last Response HTTP Code
     *
     * @access	public
     * @uses	Mailjet::Api::$_response_code
     * @return integer last Response HTTP Code
     */
    public function getHTTPCode()
    {
        return ($this->_response_code);
    }

    /**
     * Get the response from the last call
     *
     * @access	public
     * @uses	Mailjet::Api::$_response
     * @return mixed Response from the last call
     */
    public function getResponse()
    {
        return ($this->_response);
    }

    /**
     * Get the last error as a HTML table
     *
     * @access	public
     * @uses	Mailjet::Api::$_debugErrorHtml
     * @return string last Error as a HTML table
     */
    public function getErrorHtml()
    {
        return ($this->_debugErrorHtml);
    }

    /**
     * Set the current API output format
     *
     * @access	public
     * @param string $output API output format
     */
    public function setOutput($output)
    {
        $this->_output = $output;
    }

    /**
     * Get the current API output format
     *
     * @access	public
     *
     * @return string API output format
     */
    public function getOutput()
    {
        return ($this->_output);
    }

    /**
     * Set the debug flag :
     * 0 none / 1 errors only / 2 all
     *
     * @access	public
     * @param integer $debug Debug flag
     */
    public function setDebugFlag($debug)
    {
        $this->_debug = $debug;
    }

    /**
     * Get the debug flag :
     * 0 none / 1 errors only / 2 all
     *
     * @access	public
     *
     * @return integer Debug flag
     */
    public function getDebugFlag()
    {
        return ($this->_debug);
    }

    /**
     * Set the default nb of seconds before updating the cached object
     * If set to 0, Object caching will be disabled
     *
     * @access	public
     * @uses	Mailjet::Api::$_cache
     * @param integer $cache Cache to set in seconds
     */
    public function setCachePeriod($cache)
    {
        $this->_cache = $cache;
    }

    /**
     * Get the default nb of seconds before updating the cached object
     * If set to 0, Object caching will be disabled
     *
     * @access	public
     * @uses	Mailjet::Api::$_cache
     *
     * @return integer Cache in seconds
     */
    public function getCachePeriod()
    {
        return ($this->_cache);
    }

    /**
     * Set the Cache path
     *
     * @access	public
     * @uses	Mailjet::Api::$_cache_path
     * @param string $cache_path path to the cached objects
     *
     * @return boolean TRUE if the path is successfully set, FALSE otherwise
     */
    public function setCachePath($cache_path)
    {
        @mkdir($cache_path);
        if (is_dir($cache_path)) {
            $this->_cache_path = rtrim($cache_path, '/').'/';

            return (TRUE);
        }

        return (FALSE);
    }

    /**
     * Get the cache path
     *
     * @access	public
     * @uses	Mailjet::Api::$_cache_path
     *
     * @return string path to the cached objects
     */
    public function getCachePath()
    {
        return ($this->_cache_path);
    }

    public function resetRequest()
    {
    	$this->_apiUrl = (($this->_secure) ? 'https' : 'http').'://api.mailjet.com/v3/'.$this->_version;
    	$this->_request_post = false;
    }

    /**
     * Read object from cache if available and not outdated
     *
     * @access	private
     * @uses	Mailjet::Api::$_cache
     * @uses	Mailjet::Api::$_cache_path
     * @param string $object  Object or collection of resources you want to access
     * @param array  $params  Additional parameters for the request
     * @param string $request cURL request method (GET | POST)
     *
     * @return mixed Cached object, NULL otherwise
     */
    private function readCache($object, $params, $request)
    {
        if (isset($params['cache'])) {
            $cache = $params['cache'];
            unset($params['cache']);
        } else
            $cache = $this->_cache;
        if ($request == 'GET' && $cache != 0) {
            sort($params);
            $file = $object.'.'.hash('md5', $this->_apiKey.http_build_query($params, '', '')).'.'.$this->_output;
            if (file_exists($this->_cache_path.$file)) {
                $data = unserialize(file_get_contents($this->_cache_path.$file));
                if ($data['timestamp'] > time() - $cache)
                    return ($data['result']);
            }
        }

        return (NULL);
    }

    /**
     * Write object to cache
     *
     * @access	private
     * @uses	Mailjet::Api::$_cache
     * @uses	Mailjet::Api::$_cache_path
     * @param string $object  Object or collection of resources you want to access
     * @param array  $params  Additional parameters for the request
     * @param string $request cURL request method (GET | POST)
     * @param string $result  Result of the cURL request
     */
    private function writeCache($object, $params, $request, $result)
    {
        if (isset($params['cache'])) {
            $cache = $params['cache'];
            unset($params['cache']);
        } else
            $cache = $this->_cache;
        if ($request == 'GET' && $cache != 0) {
            sort($params);
            $file = $object.'.'.hash('md5', $this->_apiKey.http_build_query($params, '', '')).'.'.$this->_output;
            $data = array('timestamp' => time(), 'result' => $result);
            file_put_contents($this->_cache_path.$file, serialize($data));
        }
    }

    /**
     * Make the magic call ;)
     *
     * Check for arguments and order them before sending the request.
     *
     * @access	public
     * @uses	Mailjet::Api::$_debug
     * @uses	Mailjet::Api::debug() to display the debug output
     * @uses	Mailjet::Api::sendRequest() to send the request
     * @param string $method Method to call
     * @param array  $args   Array of parameters
     *
     * @return mixed array with the status of the response
     * and the result of the request OR FALSE on failure.
     */
    public function __call($method, $args)
    {
        $params = (sizeof($args) > 0) ? $args[0] : array();
        $request = isset($params["method"]) ? strtoupper($params["method"]) : 'GET';
        if (isset($params["method"])) unset($params["method"]);
        $result = $this->readCache($method,$params,$request);
        if (is_null($result)) {
            if ($result = $this->sendRequest($method,$params,$request))
                $this->writeCache($method,$params,$request, $this->_response);
        } else

            return ($this);

        $return = ($result === TRUE) ? $this->_response : FALSE;
        if ( $this->_debug == 2 || ( $this->_debug == 1 && $return == FALSE ) )
            $this->debug();

        return $this;
    }

    /**
     * Build the full Url for the request
     *
     * @access	private
     * @uses	Mailjet::Api::$_apiUrl
     * @uses	Mailjet::Api::$_debugCallUrl
     * @param string $method  Method to call
     * @param array  $params  Additional parameters for the request
     * @param string $request Request method
     *
     * @return string Full built Url for the request
     */
    private function requestUrlBuilder($method, $params, $request)
    {
        $query_string = array();
        foreach ($params as $key => $value) {
            if ($request == "GET" || in_array($key, array('apikey','output')))
                $query_string[$key] = $key.'='.urlencode($value);
            if ($key == "output")
                $this->_output = $value;
        }
        $query_string['output'] = 'output='.urlencode($this->_output);

        if (isset($params['ID']) && $params['ID']) {
        	$id = $params['ID'];
        	unset($params['ID']);
        	$this->_debugCallUrl = $this->_apiUrl.'/'.$method.'/'.$id.'?'.join('&',$query_string);
        } else {
        	$this->_debugCallUrl = $this->_apiUrl.'/'.$method.'/?'.join('&',$query_string);
        }

        return $this->_debugCallUrl;
    }

    /**
     * Send Request
     *
     * Send the request to the Mailjet API server and get back the result
     * Basically, setup and execute the curl process
     *
     * @access	private
     * @uses	Mailjet::Api::$_debug
     * @uses	Mailjet::Api::$_apiKey
     * @uses	Mailjet::Api::$_secretKey
     * @uses	Mailjet::Api::$_curl_handle
     * @uses	Mailjet::Api::requestUrlBuilder() to build the full Url for the request
     * @param string $method  Method to call
     * @param array  $params  Additional parameters for the request
     * @param string $request Request method
     *
     * @return string the result of the request
     */
    private function sendRequest($method = FALSE,$params=array(),$request="GET", $url = false)
    {
    	$is_json_put = (isset($params['ID']) && !empty($params['ID']));

        if ($this->_debug != 0) {
            $this->_debugMethod = $method;
            $this->_debugRequest = $request;
        }

        if ($url == false) {
        	$url = $this->requestUrlBuilder($method, $params, $request);
        }

        if(is_null($this->_curl_handle))
            $this->_curl_handle = curl_init();

        curl_setopt($this->_curl_handle, CURLOPT_URL, $url);
        curl_setopt($this->_curl_handle, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($this->_curl_handle, CURLOPT_SSL_VERIFYPEER, FALSE);
        curl_setopt($this->_curl_handle, CURLOPT_SSL_VERIFYHOST, 2);
        curl_setopt($this->_curl_handle, CURLOPT_TIMEOUT, 10); //timeout in seconds
        curl_setopt($this->_curl_handle, CURLOPT_USERPWD, $this->_apiKey.':'.$this->_secretKey);
        if (drupal_get_profile() == 'standard') {
          curl_setopt($this->_curl_handle, CURLOPT_USERAGENT, 'drupal-3.0');
    	}
    	elseif (drupal_get_profile() == 'commerce_kickstart') {
    	  curl_setopt($this->_curl_handle, CURLOPT_USERAGENT, 'kickstart');
    	}

        switch ($request) {
            case 'GET' :
                curl_setopt($this->_curl_handle, CURLOPT_CUSTOMREQUEST, 'GET');
                curl_setopt($this->_curl_handle, CURLOPT_HTTPGET, TRUE);
                curl_setopt($this->_curl_handle, CURLOPT_POSTFIELDS, NULL);
                $this->_request_post = FALSE;
                break;
            case 'POST':
                if( isset($params['Action']) && isset($params['ListID']) ){
                    curl_setopt($this->_curl_handle, CURLOPT_CUSTOMREQUEST, 'POST');
                }
                else{
                    curl_setopt($this->_curl_handle, CURLOPT_CUSTOMREQUEST, 'JSON');
                }

                curl_setopt($this->_curl_handle, CURLOPT_POST, count($params));
                if( isset($params['Action']) && isset($params['ListID']) ){
                    curl_setopt($this->_curl_handle, CURLOPT_POSTFIELDS, json_encode($params));
                    curl_setopt($this->_curl_handle, CURLOPT_HTTPHEADER, array('Content-Type: application/json'));
                }
                else{
                    curl_setopt($this->_curl_handle, CURLOPT_POSTFIELDS, http_build_query($params, '', '&'));
                }

                $this->_request_post = $params;
                break;
            case 'PUT':
                curl_setopt($this->_curl_handle, CURLOPT_CUSTOMREQUEST, 'PUT');
                curl_setopt($this->_curl_handle, CURLOPT_POSTFIELDS, http_build_query($params, '', '&'));
            break;
            case 'DELETE':
                curl_setopt($this->_curl_handle, CURLOPT_CUSTOMREQUEST, 'DELETE');
                /*curl_setopt($this->_curl_handle, CURLOPT_POSTFIELDS, http_build_query($params, '', '&'));*/
                $this->_request_post = $params;
                curl_setopt($this->_curl_handle, CURLOPT_POSTFIELDS, json_encode($this->_request_post));
                curl_setopt($this->_curl_handle, CURLOPT_RETURNTRANSFER, TRUE);
                curl_setopt($this->_curl_handle, CURLOPT_HTTPHEADER, array(
                        'Content-Type: application/json',
                        'Content-Length: ' . strlen(json_encode($this->_request_post)))
                );
            break;
            case 'JSON':
            	if($is_json_put)
            		curl_setopt($this->_curl_handle, CURLOPT_CUSTOMREQUEST, "PUT");
            	else
            		curl_setopt($this->_curl_handle, CURLOPT_CUSTOMREQUEST, "POST");

            	$this->_request_post = $params;
            	curl_setopt($this->_curl_handle, CURLOPT_POSTFIELDS, json_encode($this->_request_post));
            	curl_setopt($this->_curl_handle, CURLOPT_RETURNTRANSFER, TRUE);
            	curl_setopt($this->_curl_handle, CURLOPT_HTTPHEADER, array(
            			'Content-Type: application/json',
            			'Content-Length: ' . strlen(json_encode($this->_request_post)))
            	);
            	break;
        }

        $buffer = curl_exec($this->_curl_handle);

        if ($this->_debug > 2)
            var_dump($buffer);

        $this->_response_code = curl_getinfo($this->_curl_handle,CURLINFO_HTTP_CODE);
        $this->_response = ($this->_output == 'json') ? json_decode($buffer) : $buffer;

        return ($this->_response_code == 200) ? TRUE : FALSE;
    }

   /**
     * General request.
     *
     * @param string $method  Method to call
     * @param array  $params  Additional parameters for the request
     * @param string $request Request method
     * @param string $url Url to target the request
     */
    public function generalRequest($method = FALSE, $params = array(),$request = "GET", $url)
    {
    	$is_json_put = (isset($params['ID']) && !empty($params['ID']));

        if ($this->_debug != 0) {
            $this->_debugMethod = $method;
            $this->_debugRequest = $request;
        }

        if(is_null($this->_curl_handle))
            $this->_curl_handle = curl_init();

        curl_setopt($this->_curl_handle, CURLOPT_URL, $url);
        curl_setopt($this->_curl_handle, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($this->_curl_handle, CURLOPT_SSL_VERIFYPEER, FALSE);
        curl_setopt($this->_curl_handle, CURLOPT_SSL_VERIFYHOST, 2);
        curl_setopt($this->_curl_handle, CURLOPT_TIMEOUT, 10); //timeout in seconds
        curl_setopt($this->_curl_handle, CURLOPT_USERPWD, $this->_apiKey.':'.$this->_secretKey);

        switch ($request) {
            case 'GET' :
                curl_setopt($this->_curl_handle, CURLOPT_CUSTOMREQUEST, 'GET');
                curl_setopt($this->_curl_handle, CURLOPT_HTTPGET, TRUE);
                curl_setopt($this->_curl_handle, CURLOPT_POSTFIELDS, NULL);
                $this->_request_post = FALSE;
                break;
            case 'JSON':
            	if($is_json_put)
            		curl_setopt($this->_curl_handle, CURLOPT_CUSTOMREQUEST, "PUT");
            	else
            		curl_setopt($this->_curl_handle, CURLOPT_CUSTOMREQUEST, "POST");

            	$this->_request_post = $params;
            	curl_setopt($this->_curl_handle, CURLOPT_POSTFIELDS, json_encode($this->_request_post));
            	curl_setopt($this->_curl_handle, CURLOPT_RETURNTRANSFER, TRUE);
            	curl_setopt($this->_curl_handle, CURLOPT_HTTPHEADER, array(
            			'Content-Type: application/json',
            			'Content-Length: ' . strlen(json_encode($this->_request_post)))
            	);
            	break;
        }

    	$buffer = curl_exec($this->_curl_handle);
    	if ($this->_debug > 2)
    		var_dump($buffer);

    	$this->_response_code 	= curl_getinfo($this->_curl_handle,CURLINFO_HTTP_CODE);
    	$this->_response 		= $buffer;

    	if ($this->_debug > 0) {
    		$this->debug();
    	}

    	return ($this->_response_code == 200) ? $buffer : FALSE;

    }


    /**
     * Display debugging information
     *
     * @access	private
     * @uses	Mailjet::Api::$_response
     * @uses	Mailjet::Api::$_response_code
     * @uses	Mailjet::Api::$_debugCallUrl
     * @uses	Mailjet::Api::$_debugMethod
     * @uses	Mailjet::Api::$_debugRequest
     * @uses	Mailjet::Api::$_request_post
     */
    private function debug()
    {
        $this->_debugErrorHtml = '<style type="text/css">';
        $this->_debugErrorHtml .= '

        #debugger {width: 100%; font-family: arial;}
        #debugger table {padding: 0; margin: 0 0 20px; width: 100%; font-size: 11px; text-align: left;border-collapse: collapse;}
        #debugger th, #debugger td {padding: 2px 4px;}
        #debugger tr.h {background: #999; color: #fff;}
        #debugger tr.Success {background:#90c306; color: #fff;}
        #debugger tr.Error {background:#c30029 ; color: #fff;}
        #debugger tr.Not-modified {background:orange ; color: #fff;}
        #debugger th {width: 20%; vertical-align:top; padding-bottom: 8px;}

        ';
        $this->_debugErrorHtml .= '</style>';

        $this->_debugErrorHtml .= '<div id="debugger">';

        if (isset($this->_response_code)) {
            if ($this->_response_code == 200) {
                $this->_debugErrorHtml .= '<table>';
                $this->_debugErrorHtml .= '<tr class="Success"><th>Success</th><td></td></tr>';
                $this->_debugErrorHtml .= '<tr><th>Status code</th><td>'.$this->_response_code.'</td></tr>';
                if (isset($this->_response))
                    $this->_debugErrorHtml .= '<tr><th>Response</th><td><pre>'.utf8_decode(print_r($this->_response,1)).'</pre></td></tr>';
                $this->_debugErrorHtml .= '</table>';
            } elseif ($this->_response_code == 304) {
                $this->_debugErrorHtml .= '<table>';
                $this->_debugErrorHtml .= '<tr class="Not-modified"><th>Error</th><td></td></tr>';
                $this->_debugErrorHtml .= '<tr><th>Error no</th><td>'.$this->_response_code.'</td></tr>';
                $this->_debugErrorHtml .= '<tr><th>Message</th><td>Not Modified</td></tr>';
                $this->_debugErrorHtml .= '</table>';
            } else {
                $this->_debugErrorHtml .= '<table>';
                $this->_debugErrorHtml .= '<tr class="Error"><th>Error</th><td></td></tr>';
                $this->_debugErrorHtml .= '<tr><th>Error no</th><td>'.$this->_response_code.'</td></tr>';
                if (isset($this->_response)) {
                    if ( is_array($this->_response) OR  is_object($this->_response) ) {
                        $this->_debugErrorHtml .= '<tr><th>Status</th><td><pre>'.print_r($this->_response,TRUE).'</pre></td></tr>';
                    } else {
                        $this->_debugErrorHtml .= '<tr><th>Status</th><td><pre>'.$this->_response.'</pre></td></tr>';
                    }
                }
                $this->_debugErrorHtml .= '</table>';
            }
        }

        $call_url = parse_url($this->_debugCallUrl);

        $this->_debugErrorHtml .= '<table>';
        $this->_debugErrorHtml .= '<tr class="h"><th>API config</th><td></td></tr>';
        $this->_debugErrorHtml .= '<tr><th>Protocole</th><td>'.$call_url['scheme'].'</td></tr>';
        $this->_debugErrorHtml .= '<tr><th>Host</th><td>'.$call_url['host'].'</td></tr>';
        $this->_debugErrorHtml .= '<tr><th>Version</th><td>'.$this->_version.'</td></tr>';
        $this->_debugErrorHtml .= '</table>';

        $this->_debugErrorHtml .= '<table>';
        $this->_debugErrorHtml .= '<tr class="h"><th>Call infos</th><td></td></tr>';
        $this->_debugErrorHtml .= '<tr><th>Method</th><td>'.$this->_debugMethod.'</td></tr>';
        $this->_debugErrorHtml .= '<tr><th>Request type</th><td>'.$this->_debugRequest.'</td></tr>';
        $this->_debugErrorHtml .= '<tr><th>Get Arguments</th><td>';

        $args = array();
        if(isset($call_url['query']))
            $args = explode("&",$call_url['query']);

        if(sizeof($args)>0){
            foreach ($args as $arg) {
                $arg = explode("=",$arg);
                $this->_debugErrorHtml .= ''.$arg[0].' = <span style="color:#ff6e56;">'.$arg[1].'</span><br/>';
            }
        }

        $this->_debugErrorHtml .= '</td></tr>';

        if ($this->_request_post && sizeof($this->_request_post)>0) {
            $this->_debugErrorHtml .= '<tr><th>Post Arguments</th><td>';

            foreach ($this->_request_post as $k=>$v) {
                $this->_debugErrorHtml .= $k.' = <span style="color:#ff6e56;">'.$v.'</span><br/>';
            }

            $this->_debugErrorHtml .= '</td></tr>';
        }

        $this->_debugErrorHtml .= '<tr><th>Call url</th><td>'.$this->_debugCallUrl.'</td></tr>';
        $this->_debugErrorHtml .= '</table>';

        $this->_debugErrorHtml .= '</div>';

        if ($this->_debugEcho)
            echo $this->_debugErrorHtml;
    }

}
