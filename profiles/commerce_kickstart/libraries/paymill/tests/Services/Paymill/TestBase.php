<?php

require_once ('PHPUnit/Framework/TestCase.php');

class Services_Paymill_TestBase extends PHPUnit_Framework_TestCase
{
    protected $_apiTestKey = '';
    protected $_publicTestKey = '';
    protected $_apiUrl;

    protected function setUp()
    {   
        if (API_TEST_KEY !== '') {
            $this->_apiTestKey = API_TEST_KEY;
        } else {
            throw new Services_Paymill_Exception('Please provide the ApiTestKey in bootstrap.php or via environment', '401');
        }
        
        if (API_HOST !== '') {
            $this->_apiUrl = API_HOST;
        } else {
            throw new Services_Paymill_Exception('Please provide the API_HOST in bootstrap.php or via environment','401');
        }
    }

    /**
     * @return string
     */
    protected function getToken()
    {
        return "098f6bcd4621d373cade4e832627b4f6";
    }

    protected function getMessages($response)
    {
        $info = array(
            "endpoint" => $this->_apiUrl,
            "api_key" => $this->_apiTestKey,
            "public_key" => $this->_publicTestKey,
            "response" => var_export($response, true),
            "raw_response" => var_export(Services_Paymill_Apiclient_Curl::$lastRawResponse, true),
            "curl_options" => var_export(Services_Paymill_Apiclient_Curl::$lastRawCurlOptions, true)
        );

        return var_export($info, true);
    }
}