<?php

namespace Paymill\Test\Unit\Models\Request;

use Paymill\Models\Request as Request;
use PHPUnit_Framework_TestCase;

/**
 * Paymill\Models\Request\Refund test case.
 */
class RefundTest extends PHPUnit_Framework_TestCase
{

    /**
     * @var Request\Refund
     */
    private $_refund;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        parent::setUp();
        $this->_refund = new Request\Refund();
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
        $sample = array(
            'amount' => '4200', // e.g. "4200" for 42.00 EUR
            'description' => 'Sample Description'
        );

        $this->_refund->setAmount($sample['amount'])
            ->setDescription($sample['description']);

        $this->assertEquals($this->_refund->getAmount(), $sample['amount']);
        $this->assertEquals($this->_refund->getDescription(), $sample['description']);

        return $this->_refund;
    }

    /**
     * Test the Parameterize function of the model
     * @test
     * @depends setGetTest
     */
    public function parameterizeTest($refund)
    {
        $testId = "refund_88a388d9dd48f86c3136";
        $refund->setId($testId);

        $creationArray = $refund->parameterize("create");
        $getOneArray = $refund->parameterize("getOne");

        $this->assertEquals($creationArray, array('amount' => '4200', 'description' => 'Sample Description'));
        $this->assertEquals($getOneArray, array('count' => 1, 'offset' => 0));
    }

}