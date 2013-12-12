<?php

namespace Paymill\Test\Unit\API;

use PHPUnit_Framework_TestCase;

/**
 * Paymill\API\Curl test case.
 */
class CurlTest
    extends PHPUnit_Framework_TestCase
{

    /**
     * @var \Paymill\API\Curl
     */
    private $_curlObject;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        $this->_curlObject = $this->getMock('Paymill\API\Curl', array('_curlExec', '_curlInfo', '_curlError'), array("TestToken"));
        parent::setUp();
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_curlObject = null;
        parent::tearDown();
    }


    /**
     * Prepares the mocked curl object to return any desired value for the given method
     * @param string $method
     * @param mixed $response
     */
    private function _setMockProperties($method, $response)
    {
        $this->_curlObject->expects($this->any())->method($method)->will($this->returnValue($response));
    }

    //Testmethods


    /**
     * Tests the requestApi function using GET as the httpMethod
     * @test
     */
    public function requestApiTestGET()
    {
        //Desired Results
        $responseBody = array('test' => true);
        $responseInfo = array('http_code' => 200, 'content_type' => 'test');

        $this->_setMockProperties('_curlExec', $responseBody);
        $this->_setMockProperties('_curlInfo', $responseInfo);

        //Testing method using GET
        $result = $this->_curlObject->requestApi("", array(), 'GET');
        $this->assertEquals(
            array(
                'header' => array(
                    'status' => $responseInfo['http_code'],
                    'reason' => null,
                ),
                'body' => $responseBody)
            ,$result);
    }

    /**
     * Tests the requestApi function using POST as the httpMethod
     * @test
     */
    public function requestApiTestPost()
    {
        //Desired Results
        $responseBody = array('test' => true);
        $responseInfo = array('http_code' => 200, 'content_type' => 'test');

        $this->_setMockProperties('_curlExec', $responseBody);
        $this->_setMockProperties('_curlInfo', $responseInfo);

        //using POST to test the else case
        $result = $this->_curlObject->requestApi("", array(), 'POST');
        $this->assertEquals(
            array(
                'header' => array(
                    'status' => $responseInfo['http_code'],
                    'reason' => null,
                ),
                'body' => $responseBody)
            ,$result);
    }

    /**
     * Tests the requestApi function triggering the error case
     * @test
     */
    public function requestApiTestError()
    {
        //Desired Results
        $responseBody = false;
        $responseInfo = array('http_code' => 666, 'content_type' => 'test');
        $responseError = array('test' => true);

        $this->_setMockProperties('_curlExec', $responseBody);
        $this->_setMockProperties('_curlInfo', $responseInfo);
        $this->_setMockProperties('_curlError', $responseError);

        //using POST to test the else case
        $result = $this->_curlObject->requestApi("", array(), 'POST');
        $this->assertEquals(
            array(
                'header' => array(
                    'status' => $responseInfo['http_code'],
                    'reason' => null,
                ),
                'body' => array('error' => $responseError))
            ,$result);
    }

}