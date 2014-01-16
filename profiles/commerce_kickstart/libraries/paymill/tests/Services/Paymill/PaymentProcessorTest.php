<?php

/**
 * Services_Paymill_Payments test case.
 */
class Services_Paymill_PaymentProcessorTest extends PHPUnit_Framework_TestCase implements Services_Paymill_LoggingInterface
{
    /**
     * @var PaymentProcessor
     */
    private $_paymentProcessor;

    /**
     * @var actualLoggingMessage
     */
    private $_actualLoggingMessage;

    /**
     * @var debugMessage
     */
    private $_debugMessage;

    /**
     * @var _client
     */
    private $_clientObject;

    /**
     * @var _payment
     */
    private $_paymentObject;

    /**
     * @var _transactionObject
     */
    private $_transactionObject;

    /**
     * @var _client
     */
    private $_clientId;

    /**
     * @var _payment
     */
    private $_paymentId;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        $this->_actualLoggingMessage = null;
        $this->_apiTestKey = API_TEST_KEY;
        $this->_apiUrl = API_HOST;
        $this->_paymentProcessor = new Services_Paymill_PaymentProcessor($this->_apiTestKey, $this->_apiUrl, null, null, $this);
        $this->_clientObject = new Services_Paymill_Clients($this->_apiTestKey, $this->_apiUrl);
        $this->_paymentObject = new Services_Paymill_Payments($this->_apiTestKey, $this->_apiUrl);
        $this->_transactionObject = new Services_Paymill_Transactions($this->_apiTestKey, $this->_apiUrl);

        $this->_paymentProcessor->setAmount(1000);
        $this->_paymentProcessor->setCurrency('EUR');
        $this->_paymentProcessor->setDescription('Deuterium Cartridge');
        $this->_paymentProcessor->setEmail('John@doe.net');
        $this->_paymentProcessor->setName('John Doe');
        $this->_paymentProcessor->setToken(TOKEN);
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        if (isset($this->_paymentId)) {
            $this->_paymentObject->delete($this->_paymentId);
        }
        if (isset($this->_clientId)) {
            $this->_clientObject->delete($this->_clientId);
        }
        $this->_paymentProcessor = null;
        $this->_actualLoggingMessage = null;
        $this->_paymentObject = null;
        $this->_clientObject = null;
    }

    /**
     * @param string $actual
     * @param string $debugmessage
     */
    public function log($actual, $debugmessage)
    {
        $this->_actualLoggingMessage = $actual;
        $this->_debugMessage = $debugmessage;
    }

    /**
     * Processes the Payment
     * @param boolean $preauth
     */
    protected function ProcessPayment($captureNow = true)
    {
        $result = $this->_paymentProcessor->processPayment($captureNow);
        $this->_clientId = $this->_paymentProcessor->getClientId();
        $this->_paymentId = $this->_paymentProcessor->getPaymentId();
        return $result;
    }

    /**
     * Tests the processPayment() without any parameter
     */
    public function testValidationWithMissingParameter()
    {
        $this->_paymentProcessor->setAmount(null);
        $this->assertFalse($this->_paymentProcessor->processPayment());
    }

    /**
     * Tests the processPayment() without any parameter
     */
    public function testValidationWithWrongParameter()
    {
        $this->_paymentProcessor->setAmount('100'); // should be integer
        $this->assertFalse($this->_paymentProcessor->processPayment());
    }

    /**
     * Tests the Log-function
     */
    public function testLogging()
    {
        $this->_paymentProcessor->setName(null);
        $this->assertFalse($this->_paymentProcessor->processPayment());
        $this->assertEquals("The Parameter name is missing.", $this->_actualLoggingMessage);

        $this->_paymentProcessor->setName(12345);
        $this->assertFalse($this->_paymentProcessor->processPayment());
        $this->assertEquals("The Parameter name is not a string.", $this->_actualLoggingMessage);
    }

    /**
     * tests the Paymentprocess
     */
    public function testProcessPayment()
    {
        $this->assertTrue($this->ProcessPayment());
    }

    /**
     * tests the fest checkout
     */
    public function testFastCheckout()
    {
        $this->assertTrue($this->ProcessPayment());

        $this->assertNotEmpty($this->_clientId);
        $this->assertNotEmpty($this->_paymentId);

        $this->_paymentProcessor->setClientId($this->_clientId);
        $this->_paymentProcessor->setPaymentId($this->_paymentId);

        $this->assertTrue($this->_paymentProcessor->processPayment(), $this->_actualLoggingMessage);
        $this->assertEquals($this->_paymentProcessor->getClientId(), $this->_clientId, 'ClientId doesn´t match.');
        $this->assertEquals($this->_paymentProcessor->getPaymentId(), $this->_paymentId, 'PaymentId doesn´t match.');
    }

    /**
     * tests the Paymentprocess
     */
    public function testProcessPaymentWithWrongApiUrl()
    {
        $this->_paymentProcessor = new Services_Paymill_PaymentProcessor($this->_apiTestKey, $this->_apiUrl . '/', null, null, $this);
        $this->_paymentProcessor->setAmount(1000);
        $this->_paymentProcessor->setCurrency('EUR');
        $this->_paymentProcessor->setEmail('John@doe.net');
        $this->_paymentProcessor->setName('John Doe');
        $this->_paymentProcessor->setDescription('Deuterium Cartridge');
        $this->_paymentProcessor->setToken(TOKEN);

        $this->assertFalse($this->ProcessPayment());
        $this->assertEquals('Exception thrown from paymill wrapper.', $this->_actualLoggingMessage);
    }

    /**
     * tests the Paymentprocess when apikey has spaces
     *
     * This testcase can not be reproduced.
     */
    public function testProcessPaymentWithSpaceInApikey()
    {
        $this->_apiTestKey = $this->_apiTestKey . " ";
        $this->_paymentProcessor = new Services_Paymill_PaymentProcessor($this->_apiTestKey, $this->_apiUrl, null, null, $this);
        $this->_paymentProcessor->setAmount(1000);
        $this->_paymentProcessor->setCurrency('EUR');
        $this->_paymentProcessor->setEmail('John@doe.net');
        $this->_paymentProcessor->setName('John Doe');
        $this->_paymentProcessor->setDescription('Deuterium Cartridge');
        $this->_paymentProcessor->setToken(TOKEN);

        $this->markTestIncomplete(
                'This testcase can not be reproduced.'
        );

        $this->assertFalse($this->ProcessPayment());
        $this->assertEquals('Exception thrown from paymill wrapper.', $this->_actualLoggingMessage);
    }

    /**
     * tests the failing of Paymentprocess
     *
     */
    public function testProcessPaymentWithWrongCurrency()
    {
        $this->_paymentProcessor->setCurrency('EURonen');

        $this->assertFalse($this->ProcessPayment());
        $this->assertEquals('Exception thrown from paymill wrapper.', $this->_actualLoggingMessage);
    }

    /**
     * tests the toArray-function
     */
    public function testToArray()
    {

        $toArrayResult = $this->_paymentProcessor->toArray();
        $this->assertEquals($this->_apiTestKey, $toArrayResult['privatekey']);
        $this->assertEquals($this->_apiUrl, $toArrayResult['apiurl']);
        $this->assertInstanceOf('Services_Paymill_PaymentProcessorTest', $toArrayResult['logger']);
        $this->assertEquals(dirname(realpath('../lib/Services/Paymill/PaymentProcessor.php')) . DIRECTORY_SEPARATOR, $toArrayResult['libbase']);
        $this->assertEquals(1000, $toArrayResult['amount']);
        $this->assertEquals(0, $toArrayResult['preauthamount']);
        $this->assertEquals('EUR', $toArrayResult['currency']);
        $this->assertEquals('Deuterium Cartridge', $toArrayResult['description']);
        $this->assertEquals('John@doe.net', $toArrayResult['email']);
        $this->assertEquals('John Doe', $toArrayResult['name']);
        $this->assertEquals(TOKEN, $toArrayResult['token']);
    }

    /**
     * tests the getLastResponse-function
     */
    public function testGetLastResponse()
    {
        $expectedResult = array(
            'error' => 'Token not Found',
            'response_code' => '',
            'http_status_code' => '404'
        );

        $this->_paymentProcessor->setToken('wrongToken');
        $this->assertFalse($this->ProcessPayment());
        $response = $this->_paymentProcessor->getLastResponse();
        $this->assertInternalType('array', $response);
        $this->assertEquals($expectedResult, $response);
    }

    /**
     * tests the getTransactionId-function
     */
    public function testGetTransactionId()
    {
        $this->assertTrue($this->ProcessPayment());
        $transactionId = $this->_paymentProcessor->getTransactionId();
        $this->assertInternalType('string', $transactionId);
        $result = $this->_transactionObject->getOne($transactionId);
        $this->assertInternalType('array', $result);
        $this->assertEquals('20000', $result['response_code']);
        $this->assertEquals($transactionId, $result['id']);
    }

    /**
     * tests the Capture-function
     */
    public function testCapture()
    {
        $this->assertTrue($this->ProcessPayment(false));
        $preauthId = $this->_paymentProcessor->getPreauthId();
        $this->assertInternalType('string', $preauthId);

        $this->_paymentProcessor->capture();
        $transactionId = $this->_paymentProcessor->getTransactionId();
        $result = $this->_transactionObject->getOne($transactionId);
        $this->assertInternalType('array', $result);
        $this->assertEquals('20000', $result['response_code']);
        $this->assertEquals($transactionId, $result['id']);
    }

    /**
     * tests the creation of a preauth and a transaction.
     *
     * The amounts differ from each other so it won't create a standalone transaction
     */
    public function testPreAuthAndCapture()
    {
        $this->_paymentProcessor->setPreAuthAmount(1100);

        $this->assertTrue($this->ProcessPayment());
        $this->assertInternalType('string', $this->_paymentProcessor->getPreauthId());

        $transactionId = $this->_paymentProcessor->getTransactionId();
        $result = $this->_transactionObject->getOne($transactionId);
        $this->assertInternalType('array', $result);
        $this->assertEquals('20000', $result['response_code']);
        $this->assertEquals($transactionId, $result['id']);
        $this->assertArrayHasKey('preauthorization', $result);
        $this->assertInternalType('array', $result['preauthorization']);
        $this->assertArrayHasKey('id', $result['preauthorization']);
        $this->assertEquals($this->_paymentProcessor->getPreauthId(), $result['preauthorization']['id']);
    }

    /**
     * tests the DirectTransaction with same amount
     *
     * should create a transaction instead of preauth & capture
     */
    public function testDirectTransaction()
    {
        $this->assertTrue($this->ProcessPayment());
        $this->assertNull($this->_paymentProcessor->getPreauthId());

        $transactionId = $this->_paymentProcessor->getTransactionId();
        $result = $this->_transactionObject->getOne($transactionId);
        $this->assertInternalType('array', $result);
        $this->assertEquals('20000', $result['response_code']);
        $this->assertEquals($transactionId, $result['id']);
        $this->assertArrayHasKey('preauthorization', $result);
        $this->assertNull($result['preauthorization']);
    }

    /**
     * tests _validateResult with wrong response code
     *
     * @expectedException Exception
     * @expectedExceptionMessage Invalid Result Exception: Invalid ResponseCode
     */
    public function testValidateResultInvalidResponseCode()
    {
        $method = new ReflectionMethod($this->_paymentProcessor, '_validateResult');
        $method->setAccessible(true);

        $transaction['data']['response_code'] = 30000;
        $type = true;
        $output = $method->invoke($this->_paymentProcessor, $transaction, $type);
    }

    /**
     * tests _validateResult for invalid transaction status
     *
     * @expectedException Exception
     * @expectedExceptionMessage Invalid Result Exception: Transaction could not be issued
     */
    public function testValidateResultInvalidStatus()
    {
        $method = new ReflectionMethod($this->_paymentProcessor, '_validateResult');
        $method->setAccessible(true);

        $transaction['id'] = 1;
        $transaction['data']['id'] = 1;
        $transaction['data']['response_code'] = 20000;
        $type = 'Transaction';
        $output = $method->invoke($this->_paymentProcessor, $transaction, $type);
    }

    /**
     * tests _validateResult for invalid order state
     *
     * @expectedException Exception
     * @expectedExceptionMessage Invalid Result Exception: Invalid Orderstate
     */
    public function testValidateResultInvalidOrderstate()
    {
        $method = new ReflectionMethod($this->_paymentProcessor, '_validateResult');
        $method->setAccessible(true);

        $transaction['id'] = 1;
        $transaction['data']['id'] = 1;
        $transaction['data']['response_code'] = 20000;
        $transaction['status'] = 'open';
        $type = 'Transaction';
        $output = $method->invoke($this->_paymentProcessor, $transaction, $type);
    }

    /**
     * tests _validateResult for unknown errors
     *
     * @expectedException Exception
     * @expectedExceptionMessage Invalid Result Exception: Unknown Error 
     */
    public function testValidateResultUnknownError()
    {
        $method = new ReflectionMethod($this->_paymentProcessor, '_validateResult');
        $method->setAccessible(true);

        $transaction['id'] = 1;
        $transaction['data']['id'] = 1;
        $transaction['data']['response_code'] = 20000;
        $transaction['status'] = 'something strange';
        $type = 'Transaction';
        $output = $method->invoke($this->_paymentProcessor, $transaction, $type);
    }

    /**
     * tests _validateResult for unknown errors
     */
    public function testValidateResultCapture()
    {
        $payment = new Services_Paymill_PaymentProcessor($this->_apiTestKey, $this->_apiUrl, null, null, $this);
        $this->assertFalse($payment->capture());
    }
}
