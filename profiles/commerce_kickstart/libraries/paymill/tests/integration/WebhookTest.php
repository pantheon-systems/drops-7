<?php

namespace Paymill\Test\Integration;

use Paymill\API\Curl;
use Paymill\Models as Models;
use Paymill\Request;
use PHPUnit_Framework_TestCase;

/**
 * WebhookTest
 */
class WebhookTest extends PHPUnit_Framework_TestCase
{
    /**
     * @var \Paymill\Services\Request
     */
    private $_service;

    /**
     * @var \Paymill\Models\Request\Webhook
     */
    private $_model;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        $this->_service = new Request();
        $this->_service->setConnectionClass(new Curl(API_TEST_KEY));
        $this->_model = new Models\Request\Webhook();
        $this->_email       = 'dummy@example.com';
        $this->_url         = 'http://example.com/dummyCallback';
        parent::setUp();
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_service = null;
        $this->_model = null;
        parent::tearDown();
    }

    /**
     * @test
     * @codeCoverageIgnore
     */
    public function createWebhookWithUrl()
    {
        $this->_model->setUrl('http://example.com/dummyCallback')
            ->setEventTypes(array(
                'transaction.succeeded', 'subscription.created'
            ));
        $result = $this->_service->create($this->_model);
        $this->assertInstanceOf('Paymill\Models\Response\Webhook', $result, var_export($result, true));
        return $result;
    }

    /**
     * @test
     * @codeCoverageIgnore
     */
    public function createWebhookWithEmail()
    {
        $this->_model->setEmail('dummy@example.com')
            ->setEventTypes(array(
                'transaction.succeeded', 'subscription.created'
            ));
        $result = $this->_service->create($this->_model);
        $this->assertInstanceOf('Paymill\Models\Response\Webhook', $result, var_export($result, true));
        $this->deleteWebhook($result);
    }

    /**
     * @test
     * @codeCoverageIgnore
     * @depends createWebhookWithUrl
     */
    public function updateWebhook($model)
    {
        $this->_model->setId($model->getId())
            ->setUrl('http://example.com/dummyCallbackUpdate');
        $result = $this->_service->update($this->_model);

        $this->assertInstanceOf('Paymill\Models\Response\Webhook', $result, var_export($result, true));
        $this->assertEquals($model->getId(), $result->getId());
    }

    /**
     * @test
     * @codeCoverageIgnore
     * @depends createWebhookWithUrl
     */
    public function getOneWebhook($model)
    {
        $this->_model->setId($model->getId());
        $this->assertInstanceOf('Paymill\Models\Response\Webhook', $result = $this->_service->getOne($this->_model), var_export($result, true));
        $this->assertEquals($model->getId(), $result->getId());
    }

    /**
     * @test
     * @codeCoverageIgnore
     * @depends createWebhookWithUrl
     */
    public function getAllWebhook()
    {
        $this->_model;
        $result = $this->_service->getAll($this->_model);
        $this->assertInternalType('array', $result, var_export($result, true));
    }

    /**
     * @test
     * @codeCoverageIgnore
     */
    public function getAllWebhookWithFilter()
    {
        $this->_model->setFilter(array(
            'count' => 2,
            'offset' => 0
            )
        );
        $result = $this->_service->getAll($this->_model);
        $this->assertEquals(2, count($result), var_export($result, true));
    }

    /**
     * @test
     * @codeCoverageIgnore
     * @depends createWebhookWithUrl
     * @depends getOneWebhook
     * @depends updateWebhook
     */
    public function deleteWebhook($model)
    {
        $this->_model->setId($model->getId());
        $result = $this->_service->delete($this->_model);
        $this->markTestIncomplete('Webhook does not return a empty array like the other resources. Returns Null instead!');
        $this->assertInternalType('array', $result, var_export($result, true));
    }

}
