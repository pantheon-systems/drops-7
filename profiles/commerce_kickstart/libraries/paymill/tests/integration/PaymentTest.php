<?php

namespace Paymill\Test\Integration;

use Paymill\API\Curl;
use Paymill\Models as Models;
use Paymill\Request;
use PHPUnit_Framework_TestCase;

/**
 * PaymentTest
 */
class PaymentTest extends PHPUnit_Framework_TestCase
{
    /**
     * @var \Paymill\Services\Request
     */
    private $_service;

    /**
     * @var \Paymill\Models\Request\Payment
     */
    private $_model;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        $this->_service = new Request();
        $this->_service->setConnectionClass(new Curl(API_TEST_KEY));
        $this->_model = new Models\Request\Payment();
        parent::setUp();
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_service = null;
        $this->_model = null;
        parent::tearDown();
    }

    /**
     * @test
     * @codeCoverageIgnore
     */
    public function createPayment()
    {
        $this->_model->setToken("098f6bcd4621d373cade4e832627b4f6");
        $result = $this->_service->create($this->_model);
        $this->assertInstanceOf('Paymill\Models\Response\Payment', $result);
        return $result;
    }

    /**
     * @test
     * @codeCoverageIgnore
     * @depends createPayment
     * @expectedException \Paymill\Services\PaymillException
     * @expectedExceptionMessage Method not Found
     */
    public function updatePayment($model)
    {
        $this->_model->setId($model->getId());
        $this->_service->update($this->_model);
    }

    /**
     * @test
     * @codeCoverageIgnore
     * @depends createPayment
     */
    public function getOnePayment($model)
    {
        $this->_model->setId($model->getId());
        $result = $this->_service->getOne($this->_model);
        $this->assertInstanceOf('Paymill\Models\Response\Payment', $result, var_export($result, true));
        $this->assertEquals($model->getId(), $result->getId());
    }

    /**
     * @test
     * @codeCoverageIgnore
     * @depends createPayment
     */
    public function getAllPayment()
    {
        $this->_model;
        $result = $this->_service->getAll($this->_model);
        $this->assertInternalType('array', $result, var_export($result, true));
    }

    /**
     * @test
     * @codeCoverageIgnore
     */
    public function getAllPaymentWithFilter()
    {
        $this->_model->setFilter(array(
            'count' => 2,
            'offset' => 0
            )
        );
        $result = $this->_service->getAll($this->_model);
        $this->assertEquals(2, count($result), var_export($result, true));
    }
    /**
     * @test
     * @codeCoverageIgnore
     * @depends createPayment
     * @depends getOnePayment
     * @depends updatePayment
     */
    public function deletePayment($model)
    {
        $this->_model->setId($model->getId());
        $result = $this->_service->delete($this->_model);
        $this->assertInternalType('array', $result, var_export($result, true));
    }
}
