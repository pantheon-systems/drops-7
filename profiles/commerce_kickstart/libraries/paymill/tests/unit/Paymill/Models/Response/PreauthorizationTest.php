<?php

namespace Paymill\Test\Unit\Models\Response;

use Paymill\Models\Response as Response;
use PHPUnit_Framework_TestCase;

/**
 * Paymill\Models\Response\Preauthorization test case.
 */
class PreauthorizationTest
        extends PHPUnit_Framework_TestCase
{

    /**
     * @var \Paymill\Models\Response\Preauthorization
     */
    private $_preauthorization;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        parent::setUp();
        $this->_preauthorization = new Response\Preauthorization();
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_preauthorization = null;
        parent::tearDown();
    }

    //Testmethods
    /**
     * Tests the getters and setters of the model
     * @test
     */
    public function setGetTest()
    {
        $amount = "4200";
        $currency = "EUR";
        $status = "closed";
        $livemode = false;
        $payment = new Response\Payment();
        $client = new Response\Client();

        $this->_preauthorization->setAmount($amount)
                ->setCurrency($currency)
                ->setStatus($status)
                ->setLivemode($livemode)
                ->setPayment($payment)
                ->setClient($client);

        $this->assertEquals($this->_preauthorization->getAmount(), $amount);
        $this->assertEquals($this->_preauthorization->getCurrency(), $currency);
        $this->assertEquals($this->_preauthorization->getStatus(), $status);
        $this->assertEquals($this->_preauthorization->getLivemode(), $livemode);
        $this->assertEquals($this->_preauthorization->getPayment(), $payment);
        $this->assertEquals($this->_preauthorization->getClient(), $client);
    }

}