<?php

namespace Paymill\Test\Unit\Models\Response;

use Paymill\Models\Response as Response;
use PHPUnit_Framework_TestCase;

/**
 * Paymill\Models\Response\Refund test case.
 */
class RefundTest
        extends PHPUnit_Framework_TestCase
{

    /**
     * @var \Paymill\Models\Response\Refund
     */
    private $_refund;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        parent::setUp();
        $this->_refund = new Response\Refund();
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_refund = null;
        parent::tearDown();
    }

    //Testmethods
    /**
     * Tests the getters and setters of the model
     * @test
     */
    public function setGetTest()
    {
        $amount = "010";
        $status = "closed";
        $description = "Test Description";
        $livemode = false;
        $responseCode = 20000;
        $transaction = new Response\Transaction();

        $this->_refund->setAmount($amount)
                ->setStatus($status)
                ->setDescription($description)
                ->setLivemode($livemode)
                ->setResponseCode($responseCode)
                ->setTransaction($transaction);

        $this->assertEquals($this->_refund->getAmount(), $amount);
        $this->assertEquals($this->_refund->getStatus(), $status);
        $this->assertEquals($this->_refund->getDescription(), $description);
        $this->assertEquals($this->_refund->getLivemode(), $livemode);
        $this->assertEquals($this->_refund->getResponseCode(), $responseCode);
        $this->assertEquals($this->_refund->getTransaction(), $transaction);
    }

}