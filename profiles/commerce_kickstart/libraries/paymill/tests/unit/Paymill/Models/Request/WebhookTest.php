<?php

namespace Paymill\Test\Unit\Models\Request;

use Paymill\Models\Request as Request;
use PHPUnit_Framework_TestCase;

/**
 * Paymill\Models\Request\Webhook test case.
 */
class WebhookTest extends PHPUnit_Framework_TestCase
{

    /**
     * @var Request\Webhook
     */
    private $_webhook;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        parent::setUp();
        $this->_webhook = new Request\Webhook();
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
        $sample = array(
            'url' => 'your-webhook-url',
            'email' => 'your-webhook-email',
            'event_types' => array('transaction.succeeded', 'subscription.created')
        );

        $this->_webhook
            ->setUrl($sample['url'])
            ->setEmail($sample['email'])
            ->setEventTypes($sample['event_types']);

        $this->assertEquals($this->_webhook->getUrl(), $sample['url']);
        $this->assertEquals($this->_webhook->getEmail(), $sample['email']);
        $this->assertEquals($this->_webhook->getEventTypes(), $sample['event_types']);

        return $this->_webhook;
    }

    /**
     * Test the Parameterize function of the model
     * @test
     * @depends setGetTest
     */
    public function parameterizeTest($webhook)
    {
        $testId = "webhook_88a388d9dd48f86c3136";
        $webhook->setId($testId);

        $creationArray = $webhook->parameterize("create");
        $updateArray = $webhook->parameterize("update");
        $getOneArray = $webhook->parameterize("getOne");

        $this->assertEquals($creationArray, array(
            'url' => 'your-webhook-url',
            'event_types' => array('transaction.succeeded', 'subscription.created')
        ));
        $this->assertEquals($updateArray, array(
            'url' => 'your-webhook-url',
            'event_types' => array('transaction.succeeded', 'subscription.created')
        ));
        $this->assertEquals($getOneArray, array('count' => 1, 'offset' => 0));
    }

}