<?php

namespace Paymill\Test\Integration;

use Paymill\API\Curl;
use Paymill\Models as Models;
use Paymill\Request;
use PHPUnit_Framework_TestCase;

/**
 * OfferTest
 */
class OfferTest extends PHPUnit_Framework_TestCase
{
    /**
     * @var \Paymill\Services\Request
     */
    private $_service;

    /**
     * @var \Paymill\Models\Request\Offer
     */
    private $_model;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        $this->_service = new Request();
        $this->_service->setConnectionClass(new Curl(API_TEST_KEY));
        $this->_model = new Models\Request\Offer();
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
    public function createOffer()
    {
        $this->_model->setAmount(100)
            ->setCurrency('EUR')
            ->setInterval('2 DAY')
            ->setName('TestOffer');
        $result = $this->_service->create($this->_model);
        $this->assertInstanceOf('Paymill\Models\Response\Offer', $result, var_export($result, true));
        return $result;
    }

    /**
     * @test
     * @codeCoverageIgnore
     * @depends createOffer
     */
    public function updateOffer($model)
    {
        $this->_model->setId($model->getId())
            ->setName('NewName');
        $result = $this->_service->update($this->_model);

        $this->assertInstanceOf('Paymill\Models\Response\Offer', $result, var_export($result, true));
        $this->assertEquals($model->getId(), $result->getId());
    }

    /**
     * @test
     * @codeCoverageIgnore
     * @depends createOffer
     */
    public function getOneOffer($model)
    {
        $this->_model->setId($model->getId());
        $this->assertInstanceOf('Paymill\Models\Response\Offer', $result = $this->_service->getOne($this->_model), var_export($result, true));
        $this->assertEquals($model->getId(), $result->getId());
    }

    /**
     * @test
     * @codeCoverageIgnore
     * @depends createOffer
     */
    public function getAllOffer()
    {
        $this->_model;
        $result = $this->_service->getAll($this->_model);
        $this->assertInternalType('array', $result, var_export($result, true));
    }

    /**
     * @test
     * @codeCoverageIgnore
     */
    public function getAllOfferWithFilter()
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
     * @depends createOffer
     * @depends getOneOffer
     * @depends updateOffer
     */
    public function deleteOffer($model)
    {
        $this->_model->setId($model->getId());
        $result = $this->_service->delete($this->_model);
        $this->assertInternalType('array', $result, var_export($result, true));
    }

}
