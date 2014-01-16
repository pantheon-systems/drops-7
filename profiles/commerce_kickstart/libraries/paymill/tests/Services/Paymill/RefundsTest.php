<?php

/**
 * Services_Paymill_Transaction test case.
 */
class Services_Paymill_RefundsTest extends PHPUnit_Framework_TestCase
{
    /**
     * @var Services_Paymill_Refunds
     */
    private $_refunds;

    /**
     * @var Services_Paymill_Transactions
     */
    private $_transactions;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        $this->_apiTestKey = API_TEST_KEY;
        $this->_apiUrl = API_HOST;
        $this->_refunds = new Services_Paymill_Refunds($this->_apiTestKey,  $this->_apiUrl);
        $this->_transactions = new Services_Paymill_Transactions($this->_apiTestKey,  $this->_apiUrl);
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_refunds = null;
        $this->_transactions = null;
    }

    /**
     * Tests Services_Paymill_Refunds->create()
     */
    public function testCreateWithWrongTransactionid()
    {
        try {
            $params = array(
                'transactionId' => 'wrong_test_transactionid',
                'params'        => array('amount' => 4200)
            );

            $refund = $this->_refunds->create($params);
        } catch (Exception $e) {
            $this->assertInstanceOf('Services_Paymill_Exception', $e);
            $this->assertEquals(404, $e->getCode() );
        }
    }

    /**
     * Tests Services_Paymill_Refunds->create()
     */
    public function testCreateWithoutNullAmount()
    {
        try {
            $transactionParams = array(
                'amount'      => 4200,
                'currency'    => 'eur',
                'description' => 'Deuterium Cartridge',
                'token'       => TOKEN
            );
            $transaction = $this->_transactions->create($transactionParams);

            $params = array(
                'transactionId' => $transaction['id'],
                'params'        => null
            );
            $refund = $this->_refunds->create($params);
        } catch (Exception $e) {
            $this->assertInstanceOf('Services_Paymill_Exception', $e);
            $this->assertEquals(412, $e->getCode() );
        }
    }

    /**
     * Tests Services_Paymill_Refunds->create()
     */
    public function testCreate()
    {
        $transactionParams = array(
            'amount'      => 4200,
            'currency'    => 'eur',
            'description' => 'Deuterium Cartridge',
            'token'       => TOKEN
        );
        $transaction = $this->_transactions->create($transactionParams);

        $params = array(
            'transactionId' => $transaction['id'],
            'params'        => array('amount' => 4200)
        );

        $refund = $this->_refunds->create($params);

        $this->assertInternalType('array', $refund);
        $this->assertArrayHasKey('id', $refund);
        $this->assertEquals($refund['amount'], 4200);

        $refundId = $refund['id'];

        return $refundId;
    }

    /**
     * Tests Services_Paymill_Refunds->create()
     */
    public function testRefundWithLessAmount()
    {
        $transactionParams = array(
            'amount' => 4200,
            'currency'=> 'eur',
            'description' => 'Deuterium Cartridge',
            'token' => TOKEN
        );
        $transaction = $this->_transactions->create($transactionParams);

        $params = array(
            'transactionId' => $transaction['id'],
            'params'        => array('amount' => 3200)
        );

        $refund = $this->_refunds->create($params);

        $this->assertEquals($refund['transaction']['id'], $transaction['id']);
        $this->assertEquals($refund['transaction']['amount'], 1000);
        $this->assertEquals($refund['transaction']['status'], 'partial_refunded');

        $transactionId = $transaction['id'];

        return $transactionId;
    }

    /**
     * Tests Services_Paymill_Refunds->create()
     * @depends testRefundWithLessAmount
     */
    public function testRefundRestfromLastTest($transactionId)
    {
        $params = array(
            'transactionId' => $transactionId,
            'params'        => array('amount'=>1000)
        );

        $refund = $this->_refunds->create($params);

        $this->assertEquals($refund['transaction']['id'], $transactionId);
        $this->assertEquals($refund['transaction']['amount'], 0);
        $this->assertEquals($refund['transaction']['status'], 'refunded');
    }

    /**
     * Tests Services_Paymill_Refunds->get()
     * @depends testCreate
     */
    public function testGet()
    {
        $filters = array('count'=>10,'offset'=>0,);
        $refund = $this->_refunds->get($filters);
        $this->assertInternalType('array', $refund);
        $this->assertGreaterThanOrEqual(1, count($refund));
        $this->assertArrayHasKey('id', $refund[0]);
    }

    /**
     * Tests Services_Paymill_Refunds->getOne()
     * @depends testCreate
     */
    public function testGetOne($refundId)
    {
        $refund = $this->_refunds->getOne($refundId);

        $this->assertInternalType('array', $refund);
        $this->assertArrayHasKey('id', $refund);
        $this->assertEquals($refund['id'],$refundId);
    }

    /**
     * Tests Services_Paymill_Refunds->getOne()
     */
    public function testGetUnknownOne()
    {
        try {
            $refund = $this->_refunds->getOne('refund_b12c9470e4603paymill');
        } catch (Exception $e) {
            $this->assertInstanceOf('Services_Paymill_Exception', $e);
            $this->assertEquals(404, $e->getCode() );
        }
    }

    /**
     * Tests Services_Paymill_Refunds->update()
     */
    public function testUpdate()
    {
        try {
            $this->_refunds->update();
        } catch (Exception $e) {
            $this->assertInstanceOf('Services_Paymill_Exception', $e);
            $this->assertEquals(404, $e->getCode() );
        }
    }

    /**
     * Tests Services_Paymill_Refunds->delete()
     */
    public function testDelete()
    {
        try {
            $this->_refunds->delete();
        } catch (Exception $e) {
            $this->assertInstanceOf('Services_Paymill_Exception', $e);
            $this->assertEquals(404, $e->getCode() );
        }
    }
}
