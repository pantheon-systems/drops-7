<?php

namespace Paymill\Test\Unit\Services;

use Paymill\API\Curl;
use Paymill\Models\Request;
use Paymill\Models\Response;
use Paymill\Request as RequestService;
use PHPUnit_Framework_MockObject_MockObject;
use PHPUnit_Framework_TestCase;

/**
 * Paymill\Request test case.
 */
class RequestTest
        extends PHPUnit_Framework_TestCase
{

    /**
     * @var RequestService
     */
    private $_request;

    /**
     * @var Request\Client
     */
    private $_client;

    /**
     * @var PHPUnit_Framework_MockObject_MockObject
     */
    private $_curlObjectMock;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        parent::setUp();
        $this->_request = new RequestService();
        $this->_client = new Request\Client();
        $this->_curlObjectMock = $this->getMock('Paymill\API\Curl', array('requestApi'), array("TestToken"));
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_request = null;
        $this->_client = null;
        parent::tearDown();
    }

    /**
     * Test the setter for the connection class
     * @test
     */
    public function setConnectionClassTest()
    {
        $connector = new Curl("This connector will never be used.");
        $request = $this->_request->setConnectionClass($connector);
        $this->assertEquals($request, $this->_request);
    }

    /**
     * Test the setter for the connection class
     * @test
     */
    public function setConnectionClassWithinConstructorTest()
    {
        $this->_request = new RequestService("TestToken");
        $this->assertInstanceOf('Paymill\Request', $this->_request);
    }

    /**
     * Test the setter for the connection class
     * @test
     * @expectedException \Paymill\Services\PaymillException
     * @expectedExceptionMessage The connenction class is missing!
     */
    public function missingConnectionClassTest()
    {
        $this->_request = new RequestService();
        $this->_request->getOne($this->_client);
    }

    /**
     * Tests the create request method
     * @test
     */
    public function createTest()
    {
        $inputArray = array(
            'email'       => "max.mustermann@example.com",
            'description' => "Lovely Client"
        );
        $inputModel = $this->_client->setEmail($inputArray['email'])->setDescription($inputArray['description']);
        $outputArray = array();
        $outputArray['header']['status'] = 200;
        $outputArray['body']['data'] = array(
            "id"           => "client_88a388d9dd48f86c3136",
            "email"        => "lovely-client@example.com",
            "description"  => "Lovely Client",
            "created_at"   => 1342438695,
            "updated_at"   => 1342438695,
            "payment"      => array(
                "id"           => "pay_3af44644dd6d25c820a8",
                "type"         => "creditcard",
                "client"       => "client_88a388d9dd48f86c3136",
                "card_type"    => "visa",
                "country"      => null,
                "expire_month" => 10,
                "expire_year"  => 2013,
                "card_holder"  => null,
                "last4"        => "1111",
                "created_at"   => 1349942085,
                "updated_at"   => 1349942085,
                "app_id"       => null
            ),
            "subscription" => null,
            "app_id"       => null
        );
        $paymentModel = new Response\Payment();
        $paymentModel->setId($outputArray['body']['data']['payment']['id'])
                ->setType($outputArray['body']['data']['payment']['type'])
                ->setClient($outputArray['body']['data']['payment']['client'])
                ->setCardType($outputArray['body']['data']['payment']['card_type'])
                ->setCountry($outputArray['body']['data']['payment']['country'])
                ->setExpireMonth($outputArray['body']['data']['payment']['expire_month'])
                ->setExpireYear($outputArray['body']['data']['payment']['expire_year'])
                ->setCardHolder($outputArray['body']['data']['payment']['card_holder'])
                ->setLastFour($outputArray['body']['data']['payment']['last4'])
                ->setCreatedAt($outputArray['body']['data']['payment']['created_at'])
                ->setUpdatedAt($outputArray['body']['data']['payment']['updated_at'])
                ->setAppId($outputArray['body']['data']['payment']['app_id']);
        $outputModel = new Response\Client();
        $outputModel->setId($outputArray['body']['data']['id'])
                ->setEmail($outputArray['body']['data']['email'])
                ->setDescription($outputArray['body']['data']['description'])
                ->setCreatedAt($outputArray['body']['data']['created_at'])
                ->setUpdatedAt($outputArray['body']['data']['updated_at'])
                ->setPayment($paymentModel)
                ->setSubscription($outputArray['body']['data']['subscription'])
                ->setAppId($outputArray['body']['data']['app_id']);

        $this->_getCurlMock(
                $this->_client->getServiceResource() . $this->_client->getId(), $inputArray, "POST", $outputArray
        );

        $this->assertEquals($outputModel, $this->_request->create($inputModel));
        return $this->_client;
    }

    /**
     * Tests the update request method
     * @test
     */
    public function updateTest()
    {
        $outputArray = array();
        $outputArray['header']['status'] = 200;
        $outputArray['body']['data'] = array(
            "id"           => "client_88a388d9dd48f86c3136",
            "email"        => null,
            "description"  => "Lovely Client",
            "created_at"   => 1342438695,
            "updated_at"   => 1342438695,
            "payment"      => array(
                "id"           => "pay_3af44644dd6d25c820a8",
                "type"         => "creditcard",
                "client"       => "client_88a388d9dd48f86c3136",
                "card_type"    => "visa",
                "country"      => null,
                "expire_month" => 10,
                "expire_year"  => 2013,
                "card_holder"  => null,
                "last4"        => "1111",
                "created_at"   => 1349942085,
                "updated_at"   => 1349942085,
                "app_id"       => null
            ),
            "subscription" => null,
            "app_id"       => null
        );
        $paymentModel = new Response\Payment();
        $paymentModel->setId($outputArray['body']['data']['payment']['id'])
                ->setType($outputArray['body']['data']['payment']['type'])
                ->setClient($outputArray['body']['data']['payment']['client'])
                ->setCardType($outputArray['body']['data']['payment']['card_type'])
                ->setCountry($outputArray['body']['data']['payment']['country'])
                ->setExpireMonth($outputArray['body']['data']['payment']['expire_month'])
                ->setExpireYear($outputArray['body']['data']['payment']['expire_year'])
                ->setCardHolder($outputArray['body']['data']['payment']['card_holder'])
                ->setLastFour($outputArray['body']['data']['payment']['last4'])
                ->setCreatedAt($outputArray['body']['data']['payment']['created_at'])
                ->setUpdatedAt($outputArray['body']['data']['payment']['updated_at'])
                ->setAppId($outputArray['body']['data']['payment']['app_id']);
        $outputModel = new Response\Client();
        $outputModel->setId($outputArray['body']['data']['id'])
                ->setEmail($outputArray['body']['data']['email'])
                ->setDescription($outputArray['body']['data']['description'])
                ->setCreatedAt($outputArray['body']['data']['created_at'])
                ->setUpdatedAt($outputArray['body']['data']['updated_at'])
                ->setPayment($paymentModel)
                ->setSubscription($outputArray['body']['data']['subscription'])
                ->setAppId($outputArray['body']['data']['app_id']);

        $this->_getCurlMock(
                $this->_client->getServiceResource() . $this->_client->getId(), $this->_client->parameterize("update"), "PUT", $outputArray
        );

        $this->_client->setEmail(null);

        $this->_client = $this->_request->update($this->_client);
        $this->assertEquals($outputModel, $this->_client);
        return $this->_client;
    }

    /**
     * Tests the delete request method
     * @test
     * @depends updateTest
     */
    public function deleteTest($client)
    {

        $inputModel = new Request\Client();
        $inputModel->setId($client->getId());
        $outputArray = array();
        $outputArray['header']['status'] = 200;
        $outputArray['body']['data'] = array(
            "id"           => "client_88a388d9dd48f86c3136",
            "email"        => null,
            "description"  => "Lovely Client",
            "created_at"   => 1342438695,
            "updated_at"   => 1342438695,
            "payment"      => array(
                "id"           => "pay_3af44644dd6d25c820a8",
                "type"         => "creditcard",
                "client"       => "client_88a388d9dd48f86c3136",
                "card_type"    => "visa",
                "country"      => null,
                "expire_month" => 10,
                "expire_year"  => 2013,
                "card_holder"  => null,
                "last4"        => "1111",
                "created_at"   => 1349942085,
                "updated_at"   => 1349942085,
                "app_id"       => null
            ),
            "subscription" => null,
            "app_id"       => null
        );

        $paymentModel = new Response\Payment();
        $paymentModel->setId($outputArray['body']['data']['payment']['id'])
                ->setType($outputArray['body']['data']['payment']['type'])
                ->setClient($outputArray['body']['data']['payment']['client'])
                ->setCardType($outputArray['body']['data']['payment']['card_type'])
                ->setCountry($outputArray['body']['data']['payment']['country'])
                ->setExpireMonth($outputArray['body']['data']['payment']['expire_month'])
                ->setExpireYear($outputArray['body']['data']['payment']['expire_year'])
                ->setCardHolder($outputArray['body']['data']['payment']['card_holder'])
                ->setLastFour($outputArray['body']['data']['payment']['last4'])
                ->setCreatedAt($outputArray['body']['data']['payment']['created_at'])
                ->setUpdatedAt($outputArray['body']['data']['payment']['updated_at'])
                ->setAppId($outputArray['body']['data']['payment']['app_id']);
        $outputModel = new Response\Client();
        $outputModel->setId($outputArray['body']['data']['id'])
                ->setEmail($outputArray['body']['data']['email'])
                ->setDescription($outputArray['body']['data']['description'])
                ->setCreatedAt($outputArray['body']['data']['created_at'])
                ->setUpdatedAt($outputArray['body']['data']['updated_at'])
                ->setPayment($paymentModel)
                ->setSubscription($outputArray['body']['data']['subscription'])
                ->setAppId($outputArray['body']['data']['app_id']);

        $this->_getCurlMock(
                $this->_client->getServiceResource() . $this->_client->getId(), $this->_client->parameterize("delete"), "DELETE", $outputArray
        );

        $this->_client = $this->_request->delete($inputModel);
        $this->assertEquals($outputModel, $this->_client);
        return $this->_client;
    }

    /**
     * Tests the getAll request method
     * @test
     */
    public function getAllTest()
    {
        $outputArray['header']['status'] = 200;
        $outputArray['body']['data'] = null;
        $this->_getCurlMock(
                $this->_client->getServiceResource() . $this->_client->getId(), $this->_client->parameterize("getAll"), "GET", $outputArray
        );
        $result = $this->_request->getAll($this->_client);
        $this->assertEquals($result, null);
    }

    /**
     * Tests the getOne request method
     * @test
     * @depends createTest
     */
    public function getOneTest($client)
    {

        $outputArray = array();
        $outputArray['header']['status'] = 200;
        $outputArray['body']['data'] = array(
            "id"           => "client_88a388d9dd48f86c3136",
            "email"        => "lovely-client@example.com",
            "description"  => "Lovely Client",
            "created_at"   => 1342438695,
            "updated_at"   => 1342438695,
            "payment"      => array(
                "id"           => "pay_3af44644dd6d25c820a8",
                "type"         => "creditcard",
                "client"       => "client_88a388d9dd48f86c3136",
                "card_type"    => "visa",
                "country"      => null,
                "expire_month" => 10,
                "expire_year"  => 2013,
                "card_holder"  => null,
                "last4"        => "1111",
                "created_at"   => 1349942085,
                "updated_at"   => 1349942085,
                "app_id"       => null
            ),
            "subscription" => null,
            "app_id"       => null
        );

        $paymentModel = new Response\Payment();
        $paymentModel->setId($outputArray['body']['data']['payment']['id'])
                ->setType($outputArray['body']['data']['payment']['type'])
                ->setClient($outputArray['body']['data']['payment']['client'])
                ->setCardType($outputArray['body']['data']['payment']['card_type'])
                ->setCountry($outputArray['body']['data']['payment']['country'])
                ->setExpireMonth($outputArray['body']['data']['payment']['expire_month'])
                ->setExpireYear($outputArray['body']['data']['payment']['expire_year'])
                ->setCardHolder($outputArray['body']['data']['payment']['card_holder'])
                ->setLastFour($outputArray['body']['data']['payment']['last4'])
                ->setCreatedAt($outputArray['body']['data']['payment']['created_at'])
                ->setUpdatedAt($outputArray['body']['data']['payment']['updated_at'])
                ->setAppId($outputArray['body']['data']['payment']['app_id']);
        $outputModel = new Response\Client();
        $outputModel->setId($outputArray['body']['data']['id'])
                ->setEmail($outputArray['body']['data']['email'])
                ->setDescription($outputArray['body']['data']['description'])
                ->setCreatedAt($outputArray['body']['data']['created_at'])
                ->setUpdatedAt($outputArray['body']['data']['updated_at'])
                ->setPayment($paymentModel)
                ->setSubscription($outputArray['body']['data']['subscription'])
                ->setAppId($outputArray['body']['data']['app_id']);

        $this->_getCurlMock(
                $client->getServiceResource() . $client->getId(), $client->parameterize("getOne"), "GET", $outputArray
        );

        $this->_client = $this->_request->getOne($client);
        $this->assertEquals($outputModel, $this->_client);
        return $this->_request;
    }

    /**
     * Tests the exception trigger in the create request method
     * @test
     * @expectedException \Paymill\Services\PaymillException
     * @expectedExceptionMessage Undefined Error. This should not happen!
     */
    public function createExceptionTest()
    {
        $outputArray = array();
        $outputArray['header']['status'] = 500;
        $this->_getCurlMock(
                $this->_client->getServiceResource() . $this->_client->getId(), $this->_client->parameterize("create"), "POST", $outputArray
        );

        $this->_client = $this->_request->create($this->_client);
    }

    /**
     * Tests the exception trigger in the update request method
     * @test
     * @expectedException \Paymill\Services\PaymillException
     * @expectedExceptionMessage Undefined Error. This should not happen!
     */
    public function updateExceptionTest()
    {
        $outputArray = array();
        $outputArray['header']['status'] = 500;
        $this->_getCurlMock(
                $this->_client->getServiceResource() . $this->_client->getId(), $this->_client->parameterize("update"), "PUT", $outputArray
        );

        $this->_client = $this->_request->update($this->_client);
    }

    /**
     * Tests the exception trigger in the delete request method
     * @test
     * @expectedException \Paymill\Services\PaymillException
     * @expectedExceptionMessage Undefined Error. This should not happen!
     */
    public function deleteExceptionTest()
    {
        $outputArray = array();
        $outputArray['header']['status'] = 500;
        $this->_getCurlMock(
                $this->_client->getServiceResource() . $this->_client->getId(), $this->_client->parameterize("delete"), "DELETE", $outputArray
        );

        $this->_client = $this->_request->delete($this->_client);
    }

    /**
     * Tests the exception trigger in the getAll request method
     * @test
     * @expectedException \Paymill\Services\PaymillException
     * @expectedExceptionMessage Undefined Error. This should not happen!
     */
    public function getAllExceptionTest()
    {
        $outputArray = array();
        $outputArray['header']['status'] = 666;
        $this->_getCurlMock(
                $this->_client->getServiceResource() . $this->_client->getId(), $this->_client->parameterize("getAll"), "GET", $outputArray
        );

        $this->_client = $this->_request->getAll($this->_client);
    }


    /**
     * Tests the exception trigger in the getOne request method
     * @test
     * @expectedException \Paymill\Services\PaymillException
     * @expectedExceptionMessage Undefined Error. This should not happen!
     */
    public function getOneExceptionTest()
    {
       $outputArray = array();
        $outputArray['header']['status'] = 500;
        $this->_getCurlMock(
                $this->_client->getServiceResource() . $this->_client->getId(), $this->_client->parameterize("getOne"), "GET", $outputArray
        );

        $this->_client = $this->_request->getOne($this->_client);
    }

    /**
     * Tests the getter for the last response array
     * @test
     * @param Request $request
     * @depends getOneTest
     */
    public function getLastResponseTest($request)
    {
        $this->assertInternalType("array", $request->getLastResponse());
    }

    /**
     * Returns a mocked Curl Object
     * @param string $action Api Action
     * @param array $params Param Array for the action call
     * @param string $method httpMethod
     * @param array $response Array returned by the function
     * @return array Response Array
     */
    private function _getCurlMock($action, $params, $method, $response)
    {
        $this->_curlObjectMock->expects($this->any())
                ->method('requestApi')
                ->with(
                        $this->stringContains($action), $this->equalTo($params), $this->matches($method)
                )
                ->will($this->returnValue($response));
        $this->_request->setConnectionClass($this->_curlObjectMock);
        return $this->_curlObjectMock;
    }

}