<?php

namespace Paymill\Test\Unit\Models\Request;

use Paymill\Models\Request as Request;
use PHPUnit_Framework_TestCase;

/**
 * Paymill\Models\Request\Transaction test case.
 */
class TransactionTest extends PHPUnit_Framework_TestCase
{

    /**
     * @var Request\Transaction
     */
    private $_transaction;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        parent::setUp();
        $this->_transaction = new Request\Transaction();
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
        $sample = array(
            'amount' => '4200', // e.g. "4200" for 42.00 EUR
            'currency' => 'EUR', // ISO 4217
            'payment' => 'pay_2f82a672574647cd911d',
            'token' => '098f6bcd4621d373cade4e832627b4f6',
            'client' => 'client_c781b1d2f7f0f664b4d9',
            'preauthorization' => 'preauth_ec54f67e52e92051bd65',
            'fee_amount' => '420', // e.g. "420" for 4.20 EUR
            'fee_payment' => 'pay_098f6bcd4621d373cade4e832627b4f6',
            'description' => 'Test Transaction'
        );

        $this->_transaction
            ->setAmount($sample['amount'])
            ->setCurrency($sample['currency'])
            ->setPayment($sample['payment'])
            ->setToken($sample['token'])
            ->setClient($sample['client'])
            ->setPreauthorization($sample['preauthorization'])
            ->setFeeAmount($sample['fee_amount'])
            ->setFeePayment($sample['fee_payment'])
            ->setDescription($sample['description']);

        $this->assertEquals($this->_transaction->getAmount(), $sample['amount']);
        $this->assertEquals($this->_transaction->getCurrency(), $sample['currency']);
        $this->assertEquals($this->_transaction->getPayment(), $sample['payment']);
        $this->assertEquals($this->_transaction->getToken(), $sample['token']);
        $this->assertEquals($this->_transaction->getClient(), $sample['client']);
        $this->assertEquals($this->_transaction->getPreauthorization(), $sample['preauthorization']);
        $this->assertEquals($this->_transaction->getFeeAmount(), $sample['fee_amount']);
        $this->assertEquals($this->_transaction->getFeePayment(), $sample['fee_payment']);
        $this->assertEquals($this->_transaction->getDescription(), $sample['description']);

        return $this->_transaction;
    }

    /**
     * Test the Parameterize function of the model
     * @test
     * @depends setGetTest
     */
    public function parameterizeTest($transaction)
    {
        $testId = "transaction_88a388d9dd48f86c3136";
        $transaction->setId($testId);

        $creationArray = $transaction->parameterize("create");
        $updateArray = $transaction->parameterize("update");
        $getOneArray = $transaction->parameterize("getOne");

        $this->assertEquals($creationArray, array(
            'amount' => '4200', // e.g. "4200" for 42.00 EUR
            'currency' => 'EUR', // ISO 4217
            'client' => 'client_c781b1d2f7f0f664b4d9',
            'preauthorization' => 'preauth_ec54f67e52e92051bd65',
            'fee_amount' => '420', // e.g. "420" for 4.20 EUR
            'fee_payment' => 'pay_098f6bcd4621d373cade4e832627b4f6',
            'description' => 'Test Transaction'
        ));
        $this->assertEquals($updateArray, array(
            'description' => 'Test Transaction'
        ));
        $this->assertEquals($getOneArray, array(
            'count' => 1,
            'offset' => 0
            )
        );
    }

}