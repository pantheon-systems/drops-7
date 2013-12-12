<?php

namespace Paymill\Test\Unit\Models\Request;

use Paymill\Models\Request as Request;
use PHPUnit_Framework_TestCase;

/**
 * Paymill\Models\Request\Subscription test case.
 */
class SubscriptionTest extends PHPUnit_Framework_TestCase
{

    /**
     * @var Request\Subscription
     */
    private $_subscription;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        parent::setUp();
        $this->_subscription = new Request\Subscription();
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
        $sample = array(
            'client' => 'client_88a388d9dd48f86c3136',
            'offer' => 'offer_40237e20a7d5a231d99b',
            'payment' => 'pay_95ba26ba2c613ebb0ca8'
        );

        $this->_subscription->setPayment($sample['payment'])->setOffer($sample['offer'])->setClient($sample['client']);

        $this->assertEquals($this->_subscription->getClient(), $sample['client']);
        $this->assertEquals($this->_subscription->getOffer(), $sample['offer']);
        $this->assertEquals($this->_subscription->getPayment(), $sample['payment']);


        return $this->_subscription;
    }

    /**
     * Test the Parameterize function of the model
     * @test
     * @depends setGetTest
     */
    public function parameterizeTest($subscription)
    {
        $testId = "subscription_88a388d9dd48f86c3136";
        $cancelAtPeriodEnd = true;
        $subscription->setId($testId);
        $subscription->setCancelAtPeriodEnd($cancelAtPeriodEnd);

        $creationArray = $subscription->parameterize("create");
        $updateArray = $subscription->parameterize("update");
        $getOneArray = $subscription->parameterize("getOne");

        $this->assertEquals($creationArray, array(
            'client' => 'client_88a388d9dd48f86c3136',
            'offer' => 'offer_40237e20a7d5a231d99b',
            'payment' => 'pay_95ba26ba2c613ebb0ca8',
            'start_at' => null
        ));

        $this->assertEquals($getOneArray, array(
            'count' => 1,
            'offset' => 0
        ));
        $this->assertEquals($updateArray, array(
            'cancel_at_period_end' => true,
            'offer' => 'offer_40237e20a7d5a231d99b',
            'payment' => 'pay_95ba26ba2c613ebb0ca8'
        ));
    }

}