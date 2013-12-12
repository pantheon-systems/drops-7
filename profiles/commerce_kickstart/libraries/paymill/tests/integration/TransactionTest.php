<?php

namespace Paymill\Test\Integration;

use Paymill\API\Curl;
use Paymill\Models as Models;
use Paymill\Request;
use PHPUnit_Framework_TestCase;

/**
 * Transaction
 */
class Transaction extends PHPUnit_Framework_TestCase
{
    /**
     * @var \Paymill\Services\Request
     */
    private $_service;

    /**
     * @var \Paymill\Models\Request\Transaction
     */
    private $_model;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        $this->_service = new Request();
        $this->_service->setConnectionClass(new Curl(API_TEST_KEY));
        $this->_model = new Models\Request\Transaction();
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
     * @expectedException \Paymill\Services\PaymillException
     * @expectedExceptionMessage Token, Payment or Preauthorization required
     */
    public function createTransactionWithoutToken()
    {
        $this->_model->setAmount(100)
            ->setCurrency('EUR');
        $this->_service->create($this->_model);
    }

    /**
     * @test
     * @codeCoverageIgnore
     */
    public function createTransactionWithToken()
    {
        $this->_model->setAmount(100)
            ->setCurrency('EUR')
            ->setToken('098f6bcd4621d373cade4e832627b4f6');
        $result = $this->_service->create($this->_model);
        $this->assertInstanceOf('Paymill\Models\Response\Transaction', $result);
        return $result;
    }

    /**
     * @test
     * @codeCoverageIgnore
     * @depends createTransactionWithToken
     */
    public function updateTransaction($model)
    {
        $this->_model->setId($model->getId())
            ->setDescription('TEST');
        $result = $this->_service->update($this->_model);
        $this->assertInstanceOf('Paymill\Models\Response\Transaction', $result, var_export($result, true));
        $this->assertEquals('TEST', $result->getDescription());
    }

    /**
     * @test
     * @codeCoverageIgnore
     * @expectedException \Paymill\Services\PaymillException
     * @expectedExceptionMessage Transaction not found
     */
    public function updateTransactionWithWrongId()
    {
        $this->_model->setId('YouWillNeverFindMe404')
            ->setDescription('TEST');
        $this->_service->update($this->_model);
    }

    /**
     * @test
     * @codeCoverageIgnore
     * @depends createTransactionWithToken
     */
    public function getOneTransaction($model)
    {
        $this->_model->setId($model->getId());
        $result = $this->_service->getOne($this->_model);
        $this->assertInstanceOf('Paymill\Models\Response\Transaction', $result, var_export($result, true));
        $this->assertEquals($model->getId(), $result->getId());
    }

    /**
     * @test
     * @codeCoverageIgnore
     * @depends createTransactionWithToken
     */
    public function getAllTransaction()
    {
        $this->_model;
        $result = $this->_service->getAll($this->_model);
        $this->assertInternalType('array', $result, var_export($result, true));
    }

    /**
     * @test
     * @codeCoverageIgnore
     */
    public function getAllTransactionWithFilter()
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
     * @depends createTransactionWithToken
     * @depends getOneTransaction
     * @depends updateTransaction
     * @expectedException \Paymill\Services\PaymillException
     * @expectedExceptionMessage Method not Found
     */
    public function deleteTransaction($model)
    {
        $this->_model->setId($model->getId());
        $this->_service->delete($this->_model);
    }

}
