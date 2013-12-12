<?php

namespace Paymill\Test\Unit\Models\Response;

use Paymill\Models\Response as Response;
use PHPUnit_Framework_TestCase;

/**
 * Paymill\Models\Response\Client test case.
 */
class ClientTest
        extends PHPUnit_Framework_TestCase
{

    /**
     * @var Response\Client
     */
    private $_client;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        parent::setUp();
        $this->_client = new Response\Client();
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_client = null;
        parent::tearDown();
    }

    //Testmethods
    /**
     * Tests the getters and setters of the model
     * @test
     */
    public function setGetTest()
    {
        $paymentModel = new Response\Payment();
        $email = "lovely-client@example.com";
        $descriptionValue = "TestDesc";

        $this->_client->setEmail($email)
                ->setDescription($descriptionValue)
                ->setPayment($paymentModel);

        $this->assertEquals($this->_client->getEmail(), $email);
        $this->assertEquals($this->_client->getDescription(), $descriptionValue);
        $this->assertEquals($this->_client->getPayment(), $paymentModel);
    }

}