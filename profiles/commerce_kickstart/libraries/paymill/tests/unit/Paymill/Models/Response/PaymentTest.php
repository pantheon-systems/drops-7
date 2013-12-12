<?php

namespace Paymill\Test\Unit\Models\Response;

use Paymill\Models\Response as Response;
use PHPUnit_Framework_TestCase;

/**
 * Paymill\Models\Response\Payment test case.
 */
class PaymentTest
        extends PHPUnit_Framework_TestCase
{

    /**
     * @var \Paymill\Models\Response\Payment
     */
    private $_payment;

   /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        parent::setUp();
        $this->_payment = new Response\Payment();
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
        $type = "Test";
        $client = "Test";
        $cardType = "Test";
        $country = "Test";
        $expireMonth = 1;
        $expireYear = 2;
        $cardHolder = "Test";
        $lastFour = "Test";
        $code = "Test";
        $account = "Test";
        $holder = "Test";
        
        
        $this->_payment->setType($type)
                ->setClient($client)
                ->setCardType($cardType)
                ->setCountry($country)
                ->setExpireMonth($expireMonth)
                ->setExpireYear($expireYear)
                ->setCardHolder($cardHolder)
                ->setLastFour($lastFour)
                ->setCode($code)
                ->setAccount($account)
                ->setHolder($holder);
        
        
        $this->assertEquals($this->_payment->getType(),$type);
        $this->assertEquals($this->_payment->getClient(),$client);
        $this->assertEquals($this->_payment->getCardType(),$cardType);
        $this->assertEquals($this->_payment->getCountry(),$country);
        $this->assertEquals($this->_payment->getExpireMonth(),$expireMonth);
        $this->assertEquals($this->_payment->getExpireYear(),$expireYear);
        $this->assertEquals($this->_payment->getCardHolder(),$cardHolder);
        $this->assertEquals($this->_payment->getLastFour(),$lastFour);
        $this->assertEquals($this->_payment->getCode(),$code);
        $this->assertEquals($this->_payment->getAccount(),$account);
        $this->assertEquals($this->_payment->getHolder(),$holder);
        
    }

}