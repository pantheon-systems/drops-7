<?php

/**
 * Services_Paymill_Payments test case.
 */
class Services_Paymill_PaymentsTest extends PHPUnit_Framework_TestCase
{
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
        $this->_payments = new Services_Paymill_Payments($this->_apiTestKey,  $this->_apiUrl);
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_payments = null;
    }

    /**
     * Tests Services_Paymill_Payments->create()
     */
    public function testCreateWithoutToken()
    {
        $payment = $this->_payments->create(null);
        $this->assertInternalType("array", $payment);
        $this->assertArrayHasKey("error", $payment);

        $error = $payment["error"];
        $this->assertArrayHasKey("field", $error);
        $this->assertEquals("token", $error["field"]);
    }

    /**
     * Tests Services_Paymill_Payments->create()
     */
    public function testCreateWithoWrongToken()
    {
        $token = 'token_wrongtoken1234';
        $payment = $this->_payments->create(array("token"=>$token));
        $this->assertInternalType("array", $payment);
        $this->assertArrayHasKey("error", $payment);
        $this->assertEquals("Token not Found", $payment["error"]);
    }

    /**
     * Tests Services_Paymill_Payments->create()
     */
    public function testCreateCc()
    {
        $payment = $this->_payments->create(array("token"=> TOKEN));

        $this->assertInternalType('array', $payment);
        $this->assertArrayHasKey("id", $payment);
        $this->assertEquals("creditcard", $payment["type"]);
        $this->assertEquals($payment['last4'],'1111');
        $this->assertEquals($payment['expire_month'],'12');
        $this->assertEquals($payment['expire_year'],'2014');

        $paymentId = $payment['id'];

        return $paymentId;
    }

    /**
     * Tests Services_Paymill_Payments->create()
     */
    public function testCreateDebit()
    {
        // @todo: fix test
        $this->markTestIncomplete(
                'This function is deprecated.'
        );

        $payment = $this->_payments->create(array(
            "type"=>"debit",
            "code"=>"12345678",
            "account"=>"32876487",
            "holder"=>"Max Kunde"
        ));

        $this->assertInternalType('array', $payment);
        $this->assertArrayHasKey("id", $payment);
        $this->assertEquals("debit", $payment["type"]);
        $this->assertEquals($payment['code'],'12345678');
        $this->assertEquals($payment['holder'],'Max Kunde');
        $this->assertEquals($payment['account'],'****6487');

        $paymentId = $payment['id'];

        return $paymentId;
    }

    /**
     * Tests Services_Paymill_Payments->getOne()
     * @depends testCreateCc
     */
    public function testGetOneCc($paymentId)
    {
        $payment = $this->_payments->getOne($paymentId);

        $this->assertInternalType('array', $payment);
        $this->assertEquals($payment['id'],$paymentId);
    }

    /**
     * Tests Services_Paymill_Payments->getOne()
     * @depends testCreateDebit
     */
    public function testGetOneDebit($paymentId)
    {
        $payment = $this->_payments->getOne($paymentId);

        $this->assertInternalType('array', $payment);
        $this->assertEquals($payment['id'],$paymentId);
    }

    /**
     * Tests Services_Paymill_Payments->getOne()
     */
    public function testGetWithWrongId()
    {
        try {
            $payment = $this->_payments->getOne('card_123456789paymill');
        } catch (Exception $e) {
            $this->assertInstanceOf('Services_Paymill_Exception', $e);
            $this->assertEquals(404, $e->getCode() );
        }
    }

    /**
     * Tests Services_Paymill_Payments->get()
     */
    public function testGet()
    {
        $payment = $this->_payments->get();

        $this->assertInternalType('array', $payment);
        $this->assertGreaterThan(1,count($payment));
    }

    /**
     * Tests Services_Paymill_Payments->delete()
     * @depends testCreateCc
     */
    public function testDeleteCc($paymentId)
    {
        $payment = $this->_payments->delete($paymentId);

        $this->assertInternalType('array', $payment);
        $this->assertCount(0,$payment);
    }

    /**
     * Tests Services_Paymill_Payments->delete()
     * @depends testCreateDebit
     */
    public function testDeleteDebit($paymentId)
    {
        $payment = $this->_payments->delete($paymentId);

        $this->assertInternalType('array', $payment);
        $this->assertCount(0,$payment);
    }

    /**
     * Tests Services_Paymill_Payments->update()
     * @expectedException        Services_Paymill_Exception
     * @expectedExceptionMessage Services_Paymill_Payments does not support Services_Paymill_Payments::update
     * @expectedExceptionCode    404
     */
    public function testUpdate()
    {
        $payment = $this->_payments->update();
    }
}
