<?php

/**
 * Services_Paymill_Preauthorizations test case.
 */
class Services_Paymill_PreauthorizationsTest extends PHPUnit_Framework_TestCase
{
    /**
     * @var Services_Paymill_Preauthorizations
     */
    private $_preauthorization;

    /**
     * @var Services_Paymill_Transactions
     */
    private $_transaction;

    /**
     * @var Services_Paymill_Payments
     */
    private $_payments;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        $this->_apiTestKey = API_TEST_KEY;
        $this->_apiUrl = API_HOST;
        $this->_preauthorization = new Services_Paymill_Preauthorizations($this->_apiTestKey,  $this->_apiUrl);
        $this->_transaction = new Services_Paymill_Transactions($this->_apiTestKey,  $this->_apiUrl);
        $this->_payments = new Services_Paymill_Payments($this->_apiTestKey,  $this->_apiUrl);
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_preauthorization = null;
        $this->_transaction = null;
        $this->_payments = null;

        parent::tearDown();
    }
    
    /**
     * Tests Services_Paymill_Preauthorizations->create()
     */
    public function testCreate()
    {
        $params = array('amount' => 30,
                        'currency'=> 'gbp',
                        'token' => TOKEN
                        );

        $preauthorization = $this->_preauthorization->create($params);

        $this->assertInternalType('array', $preauthorization);
        $this->assertArrayHasKey('id', $preauthorization);
        $this->assertNotEmpty($preauthorization['id']);
        $this->assertEquals($preauthorization['amount'], 30);

        $preauthorizationId = $preauthorization['preauthorization']['id'];
        
        return $preauthorizationId;
    }

    /**
     * Tests Services_Paymill_Transactions->create()
     * @depends testCreate
     */
    public function testCreateTransaction($preauthorizationId)
    {
        $params = array('amount' => 30,
                        'currency'=> 'gbp',
                        'description' => 'Lancashire Cheese',
                        'preauthorization' => $preauthorizationId
                        );

        $transaction = $this->_transaction->create($params);

        $this->assertInternalType('array', $transaction);
        $this->assertArrayHasKey('id', $transaction);
        $this->assertNotEmpty($transaction['id']);
        $this->assertEquals($transaction['amount'], 30);
        $this->assertEquals($transaction['description'], 'Lancashire Cheese');

        $transactionId = $transaction['id'];
        
        return $transactionId;
    }

    /**
     * Tests Services_Paymill_Preauthorizations->get()
     * @depends testCreate
     */
    public function testGet()
    {   
        $filters = array('count'=>5,'offset'=>0);
        $preauthorizations = $this->_preauthorization->get($filters);
        
        $this->assertInternalType('array', $preauthorizations);
        $this->assertGreaterThanOrEqual(1, count($preauthorizations));
        $this->assertArrayHasKey('id', $preauthorizations[0]);
    }

    /**
     * Tests Services_Paymill_Preauthorizations->getOne()
     * @depends testCreate
     */
    public function testGetOne($preauthorizationId)
    {
        $preauthorization = $this->_preauthorization->getOne($preauthorizationId);
        $this->assertEquals($preauthorizationId, $preauthorization['id']);
    }
    
    /**
     * Tests Services_Paymill_Preauthorizations->update()
     */
    public function testUpdate()
    {
        try {
            $this->_preauthorization->update();
        } catch (Exception $e) {
            $this->assertInstanceOf('Services_Paymill_Exception', $e);
            $this->assertEquals(404, $e->getCode() );
        }
    }
    
    /**
     * Tests Services_Paymill_Preauthorizations->delete()
     */
    public function testDelete()
    {
        try {
            $this->_preauthorization->delete();
        } catch (Exception $e) {
            $this->assertInstanceOf('Services_Paymill_Exception', $e);
            $this->assertEquals(404, $e->getCode() );
        }
    }
}
