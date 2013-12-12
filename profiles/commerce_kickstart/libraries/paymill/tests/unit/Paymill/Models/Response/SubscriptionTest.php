<?php

namespace Paymill\Test\Unit\Models\Response;

use Paymill\Models\Response as Response;
use PHPUnit_Framework_TestCase;

/**
 * Paymill\Models\Response\Subscription test case.
 */
class SubscriptionTest
        extends PHPUnit_Framework_TestCase
{

    /**
     * @var \Paymill\Models\Response\Subscription
     */
    private $_subscription;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        parent::setUp();
        $this->_subscription = new Response\Subscription();
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_subscription = null;
        parent::tearDown();
    }
    
    //Testmethods
    /**
     * Tests the getters and setters of the model
     * @test
     */
    public function setGetTest()
    {
        $offer = new Response\Offer();
        $liveMode = false;
        $cancelAtPeriodEnd = false;
        $trialStart = null;
        $trialEnd = null;
        $nextCaptureAt = 1369563095;
        $canceledAt = null;
        $payment = new Response\Payment();
        $client = new Response\Client();

        $this->_subscription->setOffer($offer)
                ->setLivemode($liveMode)
                ->setCancelAtPeriodEnd($cancelAtPeriodEnd)
                ->setTrialStart($trialStart)
                ->setTrialEnd($trialEnd)
                ->setNextCaptureAt($nextCaptureAt)
                ->setCanceledAt($canceledAt)
                ->setClient($client)
                ->setPayment($payment);

        $this->assertEquals($this->_subscription->getOffer(), $offer);
        $this->assertEquals($this->_subscription->getLivemode(), $liveMode);
        $this->assertEquals($this->_subscription->getCancelAtPeriodEnd(), $cancelAtPeriodEnd);
        $this->assertEquals($this->_subscription->getTrialStart(), $trialStart);
        $this->assertEquals($this->_subscription->getTrialEnd(), $trialEnd);
        $this->assertEquals($this->_subscription->getNextCaptureAt(), $nextCaptureAt);
        $this->assertEquals($this->_subscription->getCanceledAt(), $canceledAt);
        $this->assertEquals($this->_subscription->getClient(), $client);
        $this->assertEquals($this->_subscription->getPayment(), $payment);
    }

}