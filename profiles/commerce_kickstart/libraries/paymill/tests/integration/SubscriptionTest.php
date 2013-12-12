<?php

namespace Paymill\Test\Integration;

use Paymill\API\Curl;
use Paymill\Models as Models;
use Paymill\Request;
use PHPUnit_Framework_TestCase;

/**
 * SubscriptionTest
 */
class SubscriptionTest extends PHPUnit_Framework_TestCase
{
    /**
     * @var \Paymill\Services\Request
     */
    private $_service;

    /**
     * @var \Paymill\Models\Request\Subscription
     */
    private $_model;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        $this->_service = new Request();
        $this->_service->setConnectionClass(new Curl(API_TEST_KEY));
        $this->_model = new Models\Request\Subscription();
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
    public function createSubscription()
    {
        $this->markTestIncomplete(
            'Needs to be clarified with Paymill. Creation crashes with Message "currently there exists subscriptions, please delete them first"'
        );

        $OfferModel = new Models\Request\Offer();
        $OfferModel->setAmount(100)
            ->setCurrency('EUR')
            ->setInterval('2 DAY')
            ->setName('TestOffer');
        $OfferModelResponse = $this->_service->create($OfferModel);
        $this->assertInstanceOf('Paymill\Models\Response\Offer', $OfferModelResponse, var_export($OfferModelResponse, true));

        $PaymentModel = new Models\Request\Payment();
        $PaymentModel->setToken("098f6bcd4621d373cade4e832627b4f6");
        $PaymentModelResponse = $this->_service->create($PaymentModel);
        $this->assertInstanceOf('Paymill\Models\Response\Payment', $PaymentModelResponse, var_export($PaymentModelResponse, true));

        $this->_model->setClient($PaymentModelResponse->getClient())
            ->setOffer($OfferModelResponse->getId())
            ->setPayment($PaymentModelResponse->getId())
            ->setCancelAtPeriodEnd(false);
        $result = $this->_service->create($this->_model);
        $this->assertInstanceOf('Paymill\Models\Response\Subscription', $result, var_export($result, true));
        $this->_service->delete($OfferModel->setId($OfferModelResponse->getId()));
        $this->_service->delete($PaymentModel->setId($PaymentModelResponse->getId()));
        return $result;
    }

    /**
     * @test
     * @codeCoverageIgnore
     * @depends createSubscription
     */
    public function updateSubscription($model)
    {
        $this->_model->setId($model->getId())
            ->setCancelAtPeriodEnd(true);
        $result = $this->_service->update($this->_model);

        $this->assertInstanceOf('Paymill\Models\Response\Subscription', $result, var_export($result, true));
        $this->assertTrue($result->getCancelAtPeriodEnd());
    }

    /**
     * @test
     * @codeCoverageIgnore
     * @depends createSubscription
     */
    public function getOneSubscription($model)
    {
        $this->_model->setId($model->getId());
        $this->assertInstanceOf('Paymill\Models\Response\Subscription', $result = $this->_service->getOne($this->_model), var_export($result, true));
        $this->assertEquals($model->getId(), $result->getId());
    }

    /**
     * @test
     * @codeCoverageIgnore
     * @depends createSubscription
     */
    public function getAllSubscription()
    {
        $this->_model;
        $result = $this->_service->getAll($this->_model);
        $this->assertInternalType('array', $result, var_export($result, true));
    }

    /**
     * @test
     * @codeCoverageIgnore
     */
    public function getAllSubscriptionWithFilter()
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
     * @depends createSubscription
     * @depends getOneSubscription
     * @depends updateSubscription
     */
    public function deleteSubscription($model)
    {
        $this->_model->setId($model->getId());
        $this->markTestIncomplete('Subscription does not return a empty array like the other resources.');
        $result = $this->_service->delete($this->_model);
        $this->assertInternalType('array', $result, var_export($result, true));
    }

}
