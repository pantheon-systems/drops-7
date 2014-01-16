<?php

/**
 * Services_Paymill_Webhooks test case.
 */
class Services_Paymill_WebhooksTest extends PHPUnit_Framework_TestCase
{
    /**
     * @var Services_Paymill_Webhooks
     */
    private $_webhook;
    private $_email;
    private $_url;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        $this->_webhook     = new Services_Paymill_Webhooks(API_TEST_KEY,  API_HOST);
        $this->_email       = 'dummy@example.com';
        $this->_url         = 'http://example.com/dummyCallback';
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_webhook = null;
    }

    /**
     * Tests Services_Paymill_Webhooks->create()
     */
    public function testCreateUrlWebhook()
    {
        $events = array('transaction.succeeded', 'subscription.created');
        $params = array(
            'url'        => $this->_url,
            'event_types'=> $events
        );

        $webhook = $this->_webhook->create($params);

        $this->assertInternalType('array', $webhook);
        $this->assertArrayHasKey('id', $webhook);
        $this->assertNotEmpty($webhook['id']);
        $this->assertEquals($this->_url, $webhook['url']);
        $this->assertContains('subscription.created', $webhook['event_types']);
        $this->assertContains('transaction.succeeded', $webhook['event_types']);

        $webhookId = $webhook['id'];

        return $webhookId;
    }

    /**
     * Tests Services_Paymill_Webhooks->create()
     */
    public function testCreateEmailWebhook()
    {
        $events = array('transaction.succeeded', 'subscription.created');
        $params = array(
            'email'      => $this->_email,
            'event_types'=> $events
        );

        $webhook = $this->_webhook->create($params);

        $this->assertInternalType('array', $webhook);
        $this->assertArrayHasKey('id', $webhook);
        $this->assertNotEmpty($webhook['id']);
        $this->assertEquals($this->_email, $webhook['email']);
        $this->assertContains('subscription.created', $webhook['event_types']);
        $this->assertContains('transaction.succeeded', $webhook['event_types']);

        $webhookId = $webhook['id'];

        return $webhookId;
    }


    /**
     * Tests Services_Paymill_Webhooks->get()
     */
    public function testGet()
    {
        $filters = array('count'=>5,'offset'=>0);
        $webhooks = $this->_webhook->get($filters);

        $this->assertInternalType('array', $webhooks);
        $this->assertGreaterThanOrEqual(1, count($webhooks));
        $this->assertArrayHasKey('id', $webhooks[0]);
    }

    /**
     * Tests Services_Paymill_Webhooks->getOne()
     * @depends testCreateUrlWebhook
     */
    public function testGetOne($webhookId)
    {
        $webhook = $this->_webhook->getOne($webhookId);
        $this->assertEquals($webhookId, $webhook['id']);

        return $webhook;
    }

    /**
     * Tests Services_Paymill_Webhooks->update()
     * @depends testCreateUrlWebhook
     */
    public function testUpdate($webhookId)
    {
        $events = array('transaction.failed', 'subscription.failed');
        $params = array(
            'id'         => $webhookId,
            'url'        => $this->_url,
            'event_types'=> $events
        );

        $webhook = $this->_webhook->update($params);
        $this->assertInternalType('array', $webhook);
        $this->assertArrayHasKey('id', $webhook);
        $this->assertNotEmpty($webhook['id']);
        $this->assertContains('subscription.failed', $webhook['event_types']);
        $this->assertContains('transaction.failed', $webhook['event_types']);
    }

    /**
     * Tests Services_Paymill_Webhooks->delete()
     * and cleans up the test web hooks
     */
    public function testDelete()
    {
        $webhooks = $this->_webhook->get();

        foreach ($webhooks as $webhook) {
            if(    (isset($webhook['email']) && $webhook['email'] == $this->_email)
                || (isset($webhook['url'])   && $webhook['url'] == $this->_url)
            ) {
                $webhook = $this->_webhook->delete($webhook['id']);
                $this->assertEquals(null, $webhook);
            }
        }
     }
}
