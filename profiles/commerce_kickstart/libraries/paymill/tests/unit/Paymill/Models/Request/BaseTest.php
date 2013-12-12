<?php

namespace Paymill\Test\Unit\Models\Request;

use Paymill\Models\Request as Request;
use PHPUnit_Framework_TestCase;

/**
 * Paymill\Models\Request\Base test case.
 */
class BaseTest
        extends PHPUnit_Framework_TestCase
{

    /**
     * Payment Model object to test inherited methods
     * @var \Paymill\Models\Request\Client
     */
    private $_model;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {

        parent::setUp();
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_model = null;
        parent::tearDown();
    }

    //Testmethods
    /**
     * Tests the getters and setters of the model
     * @test
     */
    public function setGetTest()
    {
        $id = "This a weird test";

        $this->_model = new Request\Client();
        $this->_model->setId($id);
        $this->_model->setFilter(array('count' => 1));

        $this->assertEquals($this->_model->getId(), $id);
        $this->assertEquals($this->_model->getServiceResource(), "clients/");
        $this->assertEquals($this->_model->getFilter(), array('count' => 1));
        return $this->_model;
    }

    /**
     * Tests the parameter return from parameterize('getAll')
     * @param \Paymill\Models\Request\Client $model
     * @test
     * @depends setGetTest
     */
    public function parameterizeGetAll($model){
        $this->assertEquals($model->parameterize('getAll'), array('count' => 1));
    }
}