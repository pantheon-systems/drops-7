<?php

namespace Paymill\Test\Unit\Models\Response;

use Paymill\Models\Response as Response;
use PHPUnit_Framework_TestCase;

/**
 * Paymill\Models\Response\Base test case.
 */
class BaseTest
        extends PHPUnit_Framework_TestCase
{

    /**
     * Payment Model object to test inherited methods
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
        $id = "This a weird test";
        $createdAt = 1;
        $updatedAt = 2;
        $appId = 1337;
        
        $this->_payment->setId($id)->setCreatedAt($createdAt)->setUpdatedAt($updatedAt)->setAppId($appId);
        
        $this->assertEquals($this->_payment->getId(), $id);
        $this->assertEquals($this->_payment->getCreatedAt(), $createdAt);
        $this->assertEquals($this->_payment->getUpdatedAt(), $updatedAt);
        $this->assertEquals($this->_payment->getAppId(), $appId);
    }
}