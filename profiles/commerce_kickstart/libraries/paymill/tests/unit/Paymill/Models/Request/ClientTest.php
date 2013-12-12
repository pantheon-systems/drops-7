<?php

namespace Paymill\Test\Unit\Models\Request;

use Paymill\Models\Request as Request;
use PHPUnit_Framework_TestCase;

/**
 * Paymill\Models\Request\Client test case.
 */
class ClientTest
        extends PHPUnit_Framework_TestCase
{

    /**
     * @var Request\Client
     */
    private $_client;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        parent::setUp();
        $this->_client = new Request\Client();
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_client = null;
        parent::tearDown();
    }

    //Testmethods
    /**
     * Tests the getters and setters of the model
     * @test
     */
    public function setGetTest()
    {
        $email = "lovely-client@example.com";
        $description = "Lovely Client";

        $this->_client->setEmail($email)->setDescription($description);

        $this->assertEquals($this->_client->getEmail(), $email);
        $this->assertEquals($this->_client->getDescription(), $description);
        return $this->_client;
    }

    /**
     * Test the Parameterize function of the model
     * @test
     * @depends setGetTest
     * @param \Paymill\Models\Request\Client $client
     */
    public function parameterizeTest($client)
    {
        $testId = "client_88a388d9dd48f86c3136";
        $client->setId($testId);

        $creationArray = $client->parameterize("create");
        $updateArray = $client->parameterize("update");
        $getOneArray = $client->parameterize("getOne");

        $this->assertEquals($creationArray,
                array('email'       => "lovely-client@example.com", 'description' => "Lovely Client"));
        $this->assertEquals($updateArray,
                array(
            'email'       => 'lovely-client@example.com',
            'description' => 'Lovely Client'
        ));
        $this->assertEquals($getOneArray, array(
            'count' => 1,
            'offset' => 0
        ));
    }

}