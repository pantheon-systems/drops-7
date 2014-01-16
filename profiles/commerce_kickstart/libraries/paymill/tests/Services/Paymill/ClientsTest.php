<?php

/**
 * Services_Paymill_Customer test case.
 */
class Services_Paymill_ClientsTest extends PHPUnit_Framework_TestCase
{
    /**
     * @var Services_Paymill_Clients
     */
    private $_clients;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        $this->_clients = new Services_Paymill_Clients(API_TEST_KEY,  API_HOST);
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_clients = null;
    }

    /**
     * Tests Services_Paymill_Clients->create()
     */
    public function testCreate()
    {
        $email = 'john.bigboote@example.org';
        $client = $this->_clients->create(array('email' => $email));

        $this->assertArrayHasKey('email', $client);
        $this->assertEquals($email, $client['email']);

        return $client['id'];
    }

    /**
     * Tests Services_Paymill_Clients->get()
     * @depends testCreate
     */
    public function testGet()
    {
        $filters = array('count'=>10,'offset'=>0,);
        $clients = $this->_clients->get($filters);
        $this->assertInternalType('array', $clients);
        $this->assertGreaterThanOrEqual(1, count($clients));
        $this->assertArrayHasKey('id', $clients[0]);
    }

    /**
     * Tests Services_Paymill_Clients->getOne()
     * @depends testCreate
     */
    public function testGetOne($clientId)
    {
        $client = $this->_clients->getOne($clientId);
        
        $this->assertNotNull($client);
        $this->assertArrayHasKey('id', $client);
        $this->assertEquals($clientId, $client['id']);

        return $client['id'];
    }
    
    /**
     * Tests Services_Paymill_Clients->getOne()
     */
    public function testGetOneNull()
    {
        $client = $this->_clients->getOne(null);
        
        $this->assertNull($client);
    }
    
    /**
     * Tests Services_Paymill_Clients->getOne()
     */
    public function testGetOneUnknownId()
    {
        try {
            $client = $this->_clients->getOne("client_9285c809b6744paymill");
        } catch (Exception $e) {
            $this->assertInstanceOf('Services_Paymill_Exception', $e);
            $this->assertEquals(404, $e->getCode() );
        }        
    }

    /**
     * Tests Services_Paymill_Clients->update()
     * @depends testGetOne
     */
    public function testUpdate($clientId)
    {
        $email = 'john.emdall@example.org';
        $client = $this->_clients->update(array('id' => $clientId,  'email' => $email) );
        
        $this->assertInternalType('array', $client);
        $this->assertArrayHasKey('email', $client);
        $this->assertEquals($email, $client['email']);

        return $client['id'];
    }
    
    /**
     * Tests Services_Paymill_Clients->update()
     */
    public function testUpdateUnknownId() {
        $clientId = 'UNKNOWNID';
        $email = 'john.emdall@example.org';

        try {
            $client = $this->_clients->update(array('id' => $clientId,  'email' => $email) );
        } catch (Exception $e) {
            $this->assertInstanceOf('Services_Paymill_Exception', $e);
            $this->assertEquals(404, $e->getCode() );
        }
    }

    /**
     * Tests Services_Paymill_Clients->delete()
     * @depends testUpdate
     */
    public function testDeleteNull()
    {
        try {
            $response = $this->_clients->delete(null);
        } catch (Exception $e) {
            $this->assertInstanceOf('Services_Paymill_Exception', $e);
            $this->assertEquals(412, $e->getCode() );
        }
    }

    /**
     * Tests Services_Paymill_Clients->delete()
     * @depends testUpdate
     */
    public function testDelete($clientId)
    {
        $client = $this->_clients->delete($clientId);
        $this->assertInternalType('array', $client);
        $this->assertEquals($clientId, $client["id"]);

        $client = $this->_clients->getOne($clientId);
        $this->assertEquals("Client not found", $client["error"]);
    }
}
