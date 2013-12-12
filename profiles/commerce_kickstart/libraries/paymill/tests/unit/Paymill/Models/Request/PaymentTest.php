<?php

namespace Paymill\Test\Unit\Models\Request;

use Paymill\Models\Request as Request;
use PHPUnit_Framework_TestCase;

/**
 * Paymill\Models\Request\Payment test case.
 */
class PaymentTest extends PHPUnit_Framework_TestCase
{

    /**
     * @var Request\Payment
     */
    private $_payment;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        parent::setUp();
        $this->_payment = new Request\Payment();
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_payment = null;
        parent::tearDown();
    }

    //Testmethods
    /**
     * Tests the getters and setters of the model
     * @test
     */
    public function setGetTest()
    {
        $sample = array(
            'client' => 'client_88a388d9dd48f86c3136',
            'token' => '098f6bcd4621d373cade4e832627b4f6'
        );

        $this->_payment->setClient($sample['client'])->setToken($sample['token']);

        $this->assertEquals($this->_payment->getClient(), $sample['client']);
        $this->assertEquals($this->_payment->getToken(), $sample['token']);

        return $this->_payment;
    }

    /**
     * Test the Parameterize function of the model
     * @test
     * @depends setGetTest
     */
    public function parameterizeTest($payment)
    {
        $testId = "payment_88a388d9dd48f86c3136";
        $payment->setId($testId);

        $creationArray = $payment->parameterize("create");
        $getOneArray = $payment->parameterize("getOne");

        $this->assertEquals($creationArray, array('client' => 'client_88a388d9dd48f86c3136', 'token' => '098f6bcd4621d373cade4e832627b4f6')
        );
        $this->assertEquals($getOneArray, array(
            'count' => 1,
            'offset' => 0
        ));
    }

}