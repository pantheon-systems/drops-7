<?php

namespace Paymill\Test\Unit\Models\Request;

use Paymill\Models\Request as Request;
use PHPUnit_Framework_TestCase;

/**
 * Paymill\Models\Request\Offer test case.
 */
class OfferTest extends PHPUnit_Framework_TestCase
{

    /**
     * @var Request\Offer
     */
    private $_offer;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        parent::setUp();
        $this->_offer = new Request\Offer();
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_offer = null;
        parent::tearDown();
    }

    //Testmethods
    /**
     * Tests the getters and setters of the model
     * @test
     */
    public function setGetTest()
    {
        $amount = '4200';
        $currency = 'EUR';
        $interval = '1 MONTH';
        $name = 'Test Offer';

        $this->_offer->setAmount($amount)->setCurrency($currency)->setInterval($interval)->setName($name);

        $this->assertEquals($this->_offer->getAmount(), $amount);
        $this->assertEquals($this->_offer->getCurrency(), $currency);
        $this->assertEquals($this->_offer->getInterval(), $interval);
        $this->assertEquals($this->_offer->getName(), $name);

        return $this->_offer;
    }

    /**
     * Test the Parameterize function of the model
     * @test
     * @depends setGetTest
     */
    public function parameterizeTest($offer)
    {
        $testId = "offer_88a388d9dd48f86c3136";
        $offer->setId($testId);

        $creationArray = $offer->parameterize("create");
        $updateArray = $offer->parameterize("update");
        $getOneArray = $offer->parameterize("getOne");

        $this->assertEquals($creationArray, array(
            'amount' => '4200', // E.g. "4200" for 42.00 EUR
            'currency' => 'EUR', // ISO 4217
            'interval' => '1 MONTH',
            'name' => 'Test Offer',
            'trial_period_days' => null
        ));
        $this->assertEquals($updateArray, array('name' => $offer->getName()));
        $this->assertEquals($getOneArray, array(
            'count' => 1,
            'offset' => 0
        ));
    }

}