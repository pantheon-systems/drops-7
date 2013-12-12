<?php

namespace Paymill\Test\Unit\Models\Request;

use Paymill\Models\Request as Request;
use PHPUnit_Framework_TestCase;

/**
 * Paymill\Models\Request\Preauthorization test case.
 */
class PreauthorizationTest extends PHPUnit_Framework_TestCase
{

    /**
     * @var Request\Preauthorization
     */
    private $_preauthorization;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        parent::setUp();
        $this->_preauthorization = new Request\Preauthorization();
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
        $sample = array(
            'token' => '098f6bcd4621d373cade4e832627b4f6',
            'payment' => 'pay_d43cf0ee969d9847512b',
            'amount' => '4200',
            'currency' => 'EUR'
        );

        $this->_preauthorization->setPayment($sample['payment'])->setToken($sample['token'])->setAmount($sample['amount'])->setCurrency($sample['currency']);

        $this->assertEquals($this->_preauthorization->getToken(), $sample['token']);
        $this->assertEquals($this->_preauthorization->getPayment(), $sample['payment']);
        $this->assertEquals($this->_preauthorization->getAmount(), $sample['amount']);
        $this->assertEquals($this->_preauthorization->getCurrency(), $sample['currency']);

        return $this->_preauthorization;
    }

    /**
     * Test the Parameterize function of the model
     * @test
     * @depends setGetTest
     */
    public function parameterizeTest($preauthorization)
    {
        $testId = "preauthorization_88a388d9dd48f86c3136";
        $preauthorization->setId($testId);

        $creationArray = $preauthorization->parameterize("create");
        $getOneArray = $preauthorization->parameterize("getOne");

        $this->assertEquals($creationArray, array(
            'payment' => 'pay_d43cf0ee969d9847512b',
            'amount' => '4200',
            'currency' => 'EUR'
            )
        );
        $this->assertEquals($getOneArray, array(
            'count' => 1,
            'offset' => 0
            )
        );
    }

}