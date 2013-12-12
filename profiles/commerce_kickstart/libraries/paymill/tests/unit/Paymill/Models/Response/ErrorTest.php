<?php

namespace Paymill\Test\Unit\Models\Response;

use Paymill\Models\Response\Error;
use PHPUnit_Framework_TestCase;

/**
 *
 */
class ErrorTest extends PHPUnit_Framework_TestCase
{

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
        parent::tearDown();
    }

    /**
     * Tests the setter and getter of this model
     * @test
     */
    public function setGet()
    {
        $errorMessage = "This is a error";
        $httpStatusCode = 200;
        $responseCode = 20000;

        $errorModel = new Error();
        $errorModel->setErrorMessage($errorMessage);
        $errorModel->setHttpStatusCode($httpStatusCode);
        $errorModel->setResponseCode($responseCode);

        $this->assertEquals($errorMessage, $errorModel->getErrorMessage());
        $this->assertEquals($httpStatusCode, $errorModel->getHttpStatusCode());
        $this->assertEquals($responseCode, $errorModel->getResponseCode());
    }

}