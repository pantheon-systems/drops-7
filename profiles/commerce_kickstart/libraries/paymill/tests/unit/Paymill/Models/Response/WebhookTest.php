<?php

namespace Paymill\Test\Unit\Models\Response;

use Paymill\Models\Response as Response;
use PHPUnit_Framework_TestCase;

/**
 * Paymill\Models\Response\Webhook test case.
 */
class WebhookTest
        extends PHPUnit_Framework_TestCase
{

    /**
     * @var \Paymill\Models\Response\Webhook
     */
    private $_webhook;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        parent::setUp();
        $this->_webhook = new Response\Webhook();
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_webhook = null;
        parent::tearDown();
    }

    //Testmethods
    /**
     * Tests the getters and setters of the model
     * @test
     */
    public function setGetTest()
    {
        $url = "www.test.ing";
        $email = "test@test.ing";
        $livemode = false;
        $eventTypes = array(
            "transaction.succeeded",
            "transaction.failed"
        );


        $this->_webhook->setUrl($url)
                ->setEmail($email)
                ->setLivemode($livemode)
                ->setEventTypes($eventTypes);

        $this->assertEquals($this->_webhook->getUrl(), $url);
        $this->assertEquals($this->_webhook->getEmail(), $email);
        $this->assertEquals($this->_webhook->getLivemode(), $livemode);
        $this->assertEquals($this->_webhook->getEventTypes(), $eventTypes);
    }

}