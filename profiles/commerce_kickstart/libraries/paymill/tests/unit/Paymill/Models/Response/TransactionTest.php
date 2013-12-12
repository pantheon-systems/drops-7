<?php

namespace Paymill\Test\Unit\Models\Response;

use Paymill\Models\Response as Response;
use PHPUnit_Framework_TestCase;

/**
 * Paymill\Models\Response\Transaction test case.
 */
class TransactionTest
        extends PHPUnit_Framework_TestCase
{

    /**
     * @var \Paymill\Models\Response\Transaction
     */
    private $_transaction;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        parent::setUp();
        $this->_transaction = new Response\Transaction();
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_transaction = null;
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
        $originAmount = 4200;
        $status = "closed";
        $description = "Test Desc";
        $livemode = false;
        $refunds = null;
        $currency = "EUR";
        $responseCode = 200000;
        $shortId = "This is a short string?!";
        $invoices = array();
        $payment = new Response\Payment();
        $client = new Response\Client();
        $preAuth = new Response\Preauthorization();
        $fees = array();

        $this->_transaction->setAmount($amount)
                ->setOriginAmount($originAmount)
                ->setStatus($status)
                ->setDescription($description)
                ->setLivemode($livemode)
                ->setRefunds($refunds)
                ->setCurrency($currency)
                ->setResponseCode($responseCode)
                ->setShortId($shortId)
                ->setInvoices($invoices)
                ->setPayment($payment)
                ->setClient($client)
                ->setPreauthorization($preAuth)
                ->setFees($fees);

        $this->assertEquals($this->_transaction->getAmount(), $amount);
        $this->assertEquals($this->_transaction->getOriginAmount(), $originAmount);
        $this->assertEquals($this->_transaction->getStatus(), $status);
        $this->assertEquals($this->_transaction->getDescription(), $description);
        $this->assertEquals($this->_transaction->getLivemode(), $livemode);
        $this->assertEquals($this->_transaction->getRefunds(), $refunds);
        $this->assertEquals($this->_transaction->getCurrency(), $currency);
        $this->assertEquals($this->_transaction->getResponseCode(), $responseCode);
        $this->assertEquals($this->_transaction->getShortId(), $shortId);
        $this->assertEquals($this->_transaction->getInvoices(), $invoices);
        $this->assertEquals($this->_transaction->getPayment(), $payment);
        $this->assertEquals($this->_transaction->getClient(), $client);
        $this->assertEquals($this->_transaction->getPreauthorization(), $preAuth);
        $this->assertEquals($this->_transaction->getFees(), $fees);
    }

}