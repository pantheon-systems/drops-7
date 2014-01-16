<?php

/**
 * Services_Paymill_Transactions test case.
 */
class Services_Paymill_TransactionsTest extends PHPUnit_Framework_TestCase 
{
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
        $this->_transaction = new Services_Paymill_Transactions($this->_apiTestKey,  $this->_apiUrl);
        $this->_payments = new Services_Paymill_Payments($this->_apiTestKey,  $this->_apiUrl);
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_transaction = null;
    }

    /**
     * Tests Services_Paymill_Transactions->create()
     */
    public function testCreate()
    {
        $params = array('amount' => 999,
                        'currency'=> 'eur',
                        'description' => 'Deuterium Cartridge',
                        'token' => TOKEN
                        );

        $transaction = $this->_transaction->create($params);

        $this->assertInternalType('array', $transaction);
        $this->assertArrayHasKey('id', $transaction);
        $this->assertNotEmpty($transaction['id']);
        $this->assertEquals($transaction['amount'], 999);
        $this->assertEquals($transaction['description'], 'Deuterium Cartridge');

        $transactionId = $transaction['id'];

        return $transactionId;
    }

    public function testCreateDebit()
    {
        // @todo: fix test
        $this->markTestIncomplete(
                'This function is deprecated.'
        );

        $payment = $this->_payments->create(array(
            "code"=>"12345678",
            "account"=>"37465234",
            "holder"=>"Max Kunde"
        ));

        $this->assertInternalType('array', $payment);
        $this->assertArrayHasKey("id", $payment);
        $this->assertEquals($payment['code'],'12345678');
        $this->assertEquals($payment['holder'],'Max Kunde');
        $this->assertEquals($payment['account'],'****5234');

        $paymentId = $payment['id'];

        $params = array('amount' => 999,
            'currency'=> 'eur',
            'description' => 'Deuterium Cartridge',
            'payment' => $paymentId
        );

        $transaction = $this->_transaction->create($params);

        $this->assertInternalType('array', $transaction);
        $this->assertArrayHasKey('id', $transaction);
        $this->assertNotEmpty($transaction['id']);
        $this->assertEquals($transaction['amount'], 999);
        $this->assertEquals($transaction['description'], 'Deuterium Cartridge');

        $transactionId = $transaction['id'];

        return $transactionId;
    }

    /**
     * Tests Services_Paymill_Transactions->get()
     * @depends testCreate
     */
    public function testGet()
    {
        $filters = array('count'=>5,'offset'=>0);
        $transactions = $this->_transaction->get($filters);

        $this->assertInternalType('array', $transactions);
        $this->assertGreaterThanOrEqual(1, count($transactions));
        $this->assertArrayHasKey('id', $transactions[0]);
    }

    /**
     * Tests Services_Paymill_Transactions->getOne()
     * @depends testCreate
     */
    public function testGetOne($transactionId)
    {
        $transaction = $this->_transaction->getOne($transactionId);
        $this->assertEquals($transactionId, $transaction['id']);
    }

    /**
     * Tests Services_Paymill_Transaction->update()
     */
    public function testUpdate()
    {
        try {
            $this->_transaction->update();
        } catch (Exception $e) {
            $this->assertInstanceOf('Services_Paymill_Exception', $e);
            $this->assertEquals(404, $e->getCode() );
        }
    }

    /**
     * Tests Services_Paymill_Transaction->delete()
     */
    public function testDelete()
    {
        try {
            $this->_transaction->delete();
        } catch (Exception $e) {
            $this->assertInstanceOf('Services_Paymill_Exception', $e);
            $this->assertEquals(404, $e->getCode() );
        }
    }
}
