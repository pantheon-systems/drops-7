<?php

namespace Paymill\Test\Unit\Models\Response;

use Paymill\Models\Response as Response;
use PHPUnit_Framework_TestCase;

/**
 * Paymill\Models\Response\Offer test case.
 */
class OfferTest
        extends PHPUnit_Framework_TestCase
{

    /**
     * @var \Paymill\Models\Response\Offer
     */
    private $_offer;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        parent::setUp();
        $this->_offer = new Response\Offer();
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
        $name = "Testoffer";
        $amount = 4200;
        $currency = "EUR";
        $interval = "1 WEEK";
        $trialPeriodDays = 0;
        $active = 3;
        $inactive = 0;

        $this->_offer->setName($name)->
                setAmount($amount)->
                setCurrency($currency)->
                setInterval($interval)->
                setTrialPeriodDays($trialPeriodDays)->
                setSubscriptionCount($active, $inactive);

        $this->assertEquals($this->_offer->getName(), $name);
        $this->assertEquals($this->_offer->getAmount(), $amount);
        $this->assertEquals($this->_offer->getCurrency(), $currency);
        $this->assertEquals($this->_offer->getInterval(), $interval);
        $this->assertEquals($this->_offer->getTrialPeriodDays(), $trialPeriodDays);
        $this->assertEquals($this->_offer->getSubscriptionCount(), array('active' => $active, 'inactive' => $inactive));
    }

}