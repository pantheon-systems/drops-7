<?php

namespace Paymill\Test\Unit\Services;

use Paymill\Models as Models;
use Paymill\Services\ResponseHandler;
use PHPUnit_Framework_TestCase;

/**
 * Services_Paymill_Customer test case.
 */
class ResponseHandlerTest extends PHPUnit_Framework_TestCase
{

    private $_errorCodes = array(
        10001 => "General undefined response.",
        10002 => "Still waiting on something.",
        20000 => "General success response.",
        40000 => "General problem with data.",
        40001 => "General problem with payment data.",
        40100 => "Problem with credit card data.",
        40101 => "Problem with cvv.",
        40102 => "Card expired or not yet valid.",
        40103 => "Limit exceeded.",
        40104 => "Card invalid.",
        40105 => "Expiry date not valid.",
        40106 => "Credit card brand required.",
        40200 => "Problem with bank account data.",
        40201 => "Bank account data combination mismatch.",
        40202 => "User authentication failed.",
        40300 => "Problem with 3d secure data.",
        40301 => "Currency / amount mismatch",
        40400 => "Problem with input data.",
        40401 => "Amount too low or zero.",
        40402 => "Usage field too long.",
        40403 => "Currency not allowed.",
        50000 => "General problem with backend.",
        50001 => "Country blacklisted.",
        50100 => "Technical error with credit card.",
        50101 => "Error limit exceeded.",
        50102 => "Card declined by authorization system.",
        50103 => "Manipulation or stolen card.",
        50104 => "Card restricted.",
        50105 => "Invalid card configuration data.",
        50200 => "Technical error with bank account.",
        50201 => "Card blacklisted.",
        50300 => "Technical error with 3D secure.",
        50400 => "Decline because of risk issues.",
        50500 => "General timeout.",
        50501 => "Timeout on side of the acquirer.",
        50502 => "Risk management transaction timeout.",
        50600 => "Duplicate transaction.",
    );

    /**
     *
     * @var \Paymill\Services\ResponseHandler
     */
    private $_responseHandler;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        parent::setUp();
        $this->_responseHandler = new ResponseHandler();
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_responseHandler = null;
        parent::tearDown();
    }

    /**
     * Tests the convertResponseModel method with the client model as outcome
     * @test
     */
    public function clientTest()
    {
        $response['header']['status'] = 200;
        $response['body']['data'] = array(
            "id" => "client_88a388d9dd48f86c3136",
            "email" => "lovely-client@example.com",
            "description" => null,
            "created_at" => 1340199740,
            "updated_at" => 1340199760,
            "payment" => array(
                'id' => "pay_be64260ee1b0a368efe597e8",
                'type' => "creditcard",
                'client' => "client_018dcaf0d8d03dde3ff6",
                'card_type' => "visa",
                'country' => null,
                'expire_month' => 12,
                'expire_year' => 2015,
                'card_holder' => null,
                'last4' => 1111,
                'created_at' => 1378472387,
                'updated_at' => 1378472387,
                'app_id' => null
            ),
            "subscription" => null,
            "app_id" => null
        );
        $subject = $this->_responseHandler->convertResponse($response, "clients/");
        $this->assertInstanceOf("\Paymill\Models\Response\Client", $subject, var_export($subject, true));
    }

    /**
     * Tests the convertResponseModel method with the client model as outcome using a client with multiple payment objects
     * @test
     */
    public function clientMultiPaymentTest()
    {
        $response = array();
        $response['header']['status'] = 200;
        $response['body']['data'] = array(
            'id' => "client_018dcaf0d8d03dde3ff6",
            'email' => "Some@Testemail.de",
            'description' => "This is a Testuser.123",
            'created_at' => 1378472311,
            'updated_at' => 1378472311,
            'app_id' => null,
            'payment' => array(
                array(
                    'id' => "pay_be64260ee1b0a368efe597e8",
                    'type' => "creditcard",
                    'client' => "client_018dcaf0d8d03dde3ff6",
                    'card_type' => "visa",
                    'country' => null,
                    'expire_month' => 12,
                    'expire_year' => 2015,
                    'card_holder' => null,
                    'last4' => 1111,
                    'created_at' => 1378472387,
                    'updated_at' => 1378472387,
                    'app_id' => null
                ), array
                    (
                    'id' => "pay_8ff8fb0e864ea55b8b2eb876",
                    'type' => "creditcard",
                    'client' => "client_018dcaf0d8d03dde3ff6",
                    'card_type' => "visa",
                    'country' => null,
                    'expire_month' => 12,
                    'expire_year' => 2015,
                    'card_holder' => null,
                    'last4' => 1111,
                    'created_at' => 1378472406,
                    'updated_at' => 1378472407,
                    'app_id' => null
                )
            ),
            'subscription' => null);

        $subject = $this->_responseHandler->convertResponse($response, "clients/");
        $this->assertInstanceOf("\Paymill\Models\Response\Client", $subject, var_export($subject, true));
        $this->assertInternalType("array", $subject->getPayment(), var_export($subject, true));
    }

    /**
     * Tests the convertResponseModel method with the payment model as outcome
     * @test
     */
    public function paymentTest()
    {
        $response['header']['status'] = 200;
        $response['body']['data'] = array(
            "id" => "pay_3af44644dd6d25c820a8",
            "type" => "creditcard",
            "client" => null,
            "card_type" => "visa",
            "country" => null,
            "expire_month" => 10,
            "expire_year" => 2013,
            "card_holder" => null,
            "last4" => "1111",
            "created_at" => 1349942085,
            "updated_at" => 1349942085,
            "app_id" => null
        );
        $subject = $this->_responseHandler->convertResponse($response, "payments/");
        $this->assertInstanceOf("\Paymill\Models\Response\Payment", $subject, var_export($subject, true));
    }

    /**
     * Tests the convertResponseModel method with the transaction model as outcome
     * @test
     */
    public function transactionTest()
    {
        $response['header']['status'] = 200;
        $response['body']['data'] = array(
            "id" => "tran_54645bcb98ba7acfe204",
            "amount" => "4200",
            "origin_amount" => 4200,
            "status" => "closed",
            "description" => null,
            "livemode" => false,
            "refunds" => null,
            "currency" => "EUR",
            "created_at" => 1349946151,
            "updated_at" => 1349946151,
            "response_code" => 20000,
            "short_id" => '0000.1212.3434',
            "invoices" => array(),
            "payment" => array(
                'id' => "pay_be64260ee1b0a368efe597e8",
                'type' => "creditcard",
                'client' => "client_018dcaf0d8d03dde3ff6",
                'card_type' => "visa",
                'country' => null,
                'expire_month' => 12,
                'expire_year' => 2015,
                'card_holder' => null,
                'last4' => 1111,
                'created_at' => 1378472387,
                'updated_at' => 1378472387,
                'app_id' => null
            ),
            "client" => array(
                "id" => "client_88a388d9dd48f86c3136",
                "email" => "lovely-client@example.com",
                "description" => null,
                "created_at" => 1340199740,
                "updated_at" => 1340199760,
                "subscription" => null,
                'app_id' => null,
                "payment" => array(
                    'id' => "pay_be64260ee1b0a368efe597e8",
                    'type' => "creditcard",
                    'client' => "client_018dcaf0d8d03dde3ff6",
                    'card_type' => "visa",
                    'country' => null,
                    'expire_month' => 12,
                    'expire_year' => 2015,
                    'card_holder' => null,
                    'last4' => 1111,
                    'created_at' => 1378472387,
                    'updated_at' => 1378472387,
                    'app_id' => null
                )),
            "preauthorization" => null,
            "fees" => array(),
            "app_id" => null
        );
        $subject = $this->_responseHandler->convertResponse($response, "transactions/");
        $this->assertInstanceOf("\Paymill\Models\Response\Transaction", $subject, var_export($subject, true));
    }

    /**
     * Tests the convertResponseModel method with the preauthorization model as outcome
     * @test
     */
    public function preauthorizationTest()
    {
        $response['header']['status'] = 200;
        $response['body']['data'] = array(
            "id" => "tran_54645bcb98ba7acfe204",
            "amount" => "4200",
            "origin_amount" => 4200,
            "status" => "closed",
            "description" => null,
            "livemode" => false,
            "refunds" => null,
            "currency" => "EUR",
            "created_at" => 1349946151,
            "updated_at" => 1349946151,
            "response_code" => 20000,
            "short_id" => '0000.1212.3434',
            "invoices" => array(),
            "payment" => array(
                'id' => "pay_be64260ee1b0a368efe597e8",
                'type' => "creditcard",
                'client' => "client_018dcaf0d8d03dde3ff6",
                'card_type' => "visa",
                'country' => null,
                'expire_month' => 12,
                'expire_year' => 2015,
                'card_holder' => null,
                'last4' => 1111,
                'created_at' => 1378472387,
                'updated_at' => 1378472387,
                'app_id' => null
            ),
            "client" => array(
                "id" => "client_88a388d9dd48f86c3136",
                "email" => "lovely-client@example.com",
                "description" => null,
                "created_at" => 1340199740,
                "updated_at" => 1340199760,
                "subscription" => null,
                'app_id' => null,
                "payment" => array(
                    'id' => "pay_be64260ee1b0a368efe597e8",
                    'type' => "creditcard",
                    'client' => "client_018dcaf0d8d03dde3ff6",
                    'card_type' => "visa",
                    'country' => null,
                    'expire_month' => 12,
                    'expire_year' => 2015,
                    'card_holder' => null,
                    'last4' => 1111,
                    'created_at' => 1378472387,
                    'updated_at' => 1378472387,
                    'app_id' => null
                )),
            "preauthorization" => array(
                "id" => "preauth_0b771c503680c341548e",
                "amount" => "4200",
                "currency" => "EUR",
                "status" => "closed",
                "livemode" => false,
                "created_at" => 1349950324,
                "updated_at" => 1349950324,
                "payment" => array(
                    'id' => "pay_be64260ee1b0a368efe597e8",
                    'type' => "creditcard",
                    'client' => "client_018dcaf0d8d03dde3ff6",
                    'card_type' => "visa",
                    'country' => null,
                    'expire_month' => 12,
                    'expire_year' => 2015,
                    'card_holder' => null,
                    'last4' => 1111,
                    'created_at' => 1378472387,
                    'updated_at' => 1378472387,
                    'app_id' => null
                ),
                "client" => array(
                    "id" => "client_88a388d9dd48f86c3136",
                    "email" => "lovely-client@example.com",
                    "description" => null,
                    "created_at" => 1340199740,
                    "updated_at" => 1340199760,
                    "subscription" => null,
                    'app_id' => null,
                    "payment" => array(
                        'id' => "pay_be64260ee1b0a368efe597e8",
                        'type' => "creditcard",
                        'client' => "client_018dcaf0d8d03dde3ff6",
                        'card_type' => "visa",
                        'country' => null,
                        'expire_month' => 12,
                        'expire_year' => 2015,
                        'card_holder' => null,
                        'last4' => 1111,
                        'created_at' => 1378472387,
                        'updated_at' => 1378472387,
                        'app_id' => null
                    )),
                "app_id" => null
            ),
            "fees" => array(),
            "app_id" => null
        );

        $subject = $this->_responseHandler->convertResponse($response, "preauthorizations/");
        $this->assertInstanceOf("\Paymill\Models\Response\Preauthorization", $subject, var_export($subject, true));
    }

    /**
     * Tests the convertResponseModel method with the refund model as outcome
     * @test
     */
    public function refundTest()
    {
        $response['header']['status'] = 200;
        $response['body']['data'] = array(
            "id" => "refund_87bc404a95d5ce616049",
            "amount" => "042",
            "status" => "refunded",
            "description" => null,
            "livemode" => false,
            "created_at" => 1349947042,
            "updated_at" => 1349947042,
            "response_code" => 20000,
            "transaction" => array(
                "id" => "tran_54645bcb98ba7acfe204",
                "amount" => "4200",
                "origin_amount" => 4200,
                "status" => "closed",
                "description" => null,
                "livemode" => false,
                "refunds" => null,
                "currency" => "EUR",
                "created_at" => 1349946151,
                "updated_at" => 1349946151,
                "response_code" => 20000,
                "short_id" => '0000.1212.3434',
                "invoices" => array(),
                "payment" => array(
                    'id' => "pay_be64260ee1b0a368efe597e8",
                    'type' => "creditcard",
                    'client' => "client_018dcaf0d8d03dde3ff6",
                    'card_type' => "visa",
                    'country' => null,
                    'expire_month' => 12,
                    'expire_year' => 2015,
                    'card_holder' => null,
                    'last4' => 1111,
                    'created_at' => 1378472387,
                    'updated_at' => 1378472387,
                    'app_id' => null
                ),
                "client" => array(
                    "id" => "client_88a388d9dd48f86c3136",
                    "email" => "lovely-client@example.com",
                    "description" => null,
                    "created_at" => 1340199740,
                    "updated_at" => 1340199760,
                    "subscription" => null,
                    'app_id' => null,
                    "payment" => array(
                        'id' => "pay_be64260ee1b0a368efe597e8",
                        'type' => "creditcard",
                        'client' => "client_018dcaf0d8d03dde3ff6",
                        'card_type' => "visa",
                        'country' => null,
                        'expire_month' => 12,
                        'expire_year' => 2015,
                        'card_holder' => null,
                        'last4' => 1111,
                        'created_at' => 1378472387,
                        'updated_at' => 1378472387,
                        'app_id' => null
                    )),
                "preauthorization" => null,
                "fees" => array(),
                "app_id" => null
            ),
            "app_id" => null
        );
        $subject = $this->_responseHandler->convertResponse($response, "refunds/");
        $this->assertInstanceOf("\Paymill\Models\Response\Refund", $subject, var_export($subject, true));
    }

    /**
     * Tests the convertResponseModel method with the offer model as outcome
     * @test
     */
    public function offerTest()
    {
        $response['header']['status'] = 200;
        $response['body']['data'] = array(
            "id" => "offer_40237e20a7d5a231d99b",
            "name" => "Nerd Special",
            "amount" => 4200,
            "currency" => "EUR",
            "interval" => "1 WEEK",
            "trial_period_days" => 0,
            "created_at" => 1341935129,
            "updated_at" => 1341935129,
            "subscription_count" => array(
                "active" => "3",
                "inactive" => 0
            ),
            "app_id" => null
        );
        $subject = $this->_responseHandler->convertResponse($response, "offers/");
        $this->assertInstanceOf("\Paymill\Models\Response\Offer", $subject, var_export($subject, true));
    }

    /**
     * Tests the convertResponseModel method with the subscription model as outcome
     * @test
     */
    public function subscriptionTest()
    {
        $response['header']['status'] = 200;
        $response['body']['data'] = array(
            "id" => "sub_012db05186ccfe22d86c",
            "offer" => array(
                "id" => "offer_40237e20a7d5a231d99b",
                "name" => "Nerd Special",
                "amount" => 4200,
                "currency" => "EUR",
                "interval" => "1 WEEK",
                "trial_period_days" => 0,
                "created_at" => 1341935129,
                "updated_at" => 1341935129,
                "subscription_count" => array(
                    "active" => "3",
                    "inactive" => 0
                ),
                "app_id" => null
            ),
            "livemode" => false,
            "cancel_at_period_end" => false,
            "trial_start" => null,
            "trial_end" => null,
            "next_capture_at" => 1369563095,
            "created_at" => 1341935490,
            "updated_at" => 1341935490,
            "canceled_at" => null,
            "payment" => array(
                'id' => "pay_be64260ee1b0a368efe597e8",
                'type' => "creditcard",
                'client' => "client_018dcaf0d8d03dde3ff6",
                'card_type' => "visa",
                'country' => null,
                'expire_month' => 12,
                'expire_year' => 2015,
                'card_holder' => null,
                'last4' => 1111,
                'created_at' => 1378472387,
                'updated_at' => 1378472387,
                'app_id' => null
            ),
            "client" => array(
                "id" => "client_88a388d9dd48f86c3136",
                "email" => "lovely-client@example.com",
                "description" => null,
                "created_at" => 1340199740,
                "updated_at" => 1340199760,
                "subscription" => null,
                'app_id' => null,
                "payment" => array(
                    'id' => "pay_be64260ee1b0a368efe597e8",
                    'type' => "creditcard",
                    'client' => "client_018dcaf0d8d03dde3ff6",
                    'card_type' => "visa",
                    'country' => null,
                    'expire_month' => 12,
                    'expire_year' => 2015,
                    'card_holder' => null,
                    'last4' => 1111,
                    'created_at' => 1378472387,
                    'updated_at' => 1378472387,
                    'app_id' => null
                )),
            "app_id" => null
        );
        $subject = $this->_responseHandler->convertResponse($response, "subscriptions/");
        $this->assertInstanceOf("\Paymill\Models\Response\Subscription", $subject, var_export($subject, true));
    }

    /**
     * Tests the convertResponseModel method with the url version of the Webhook model as outcome
     * @test
     */
    public function urlWebhookTest()
    {
        $response['header']['status'] = 200;
        $response['body']['data'] = array(
            "id" => "hook_40237e20a7d5a231d99b",
            "url" => "your-webhook-url",
            "livemode" => false,
            "event_types" => array(
                "transaction.succeeded",
                "transaction.failed"
            ),
            "created_at" => 1358982000,
            "updated_at" => 1358982000,
            "app_id" => null
        );
        $subject = $this->_responseHandler->convertResponse($response, "webhooks/");
        $this->assertInstanceOf("\Paymill\Models\Response\Webhook", $subject, var_export($subject, true));
    }

    /**
     * Tests the convertResponseModel method with the email version of the Webhook model as outcome
     * @test
     */
    public function emailWebhookTest()
    {
        $response['header']['status'] = 200;
        $response['body']['data'] = array(
            "id" => "hook_40237e20a7d5a231d99b",
            "email" => "your-webhook-email",
            "livemode" => false,
            "event_types" => array(
                "transaction.succeeded",
                "transaction.failed"
            ),
            "created_at" => 1358982000,
            "updated_at" => 1358982000,
            "app_id" => null
        );
        $subject = $this->_responseHandler->convertResponse($response, "webhooks/");
        $this->assertInstanceOf("\Paymill\Models\Response\Webhook", $subject, var_export($subject, true));
    }

    /**
     * Tests the handling of ResponseCodes
     * @test
     */
    public function checkResponseCodes()
    {
        $response['header']['status'] = 200;
        $response['body']['data'] = array(
            "id" => "tran_54645bcb98ba7acfe204",
            "amount" => "4200",
            "origin_amount" => 4200,
            "status" => "closed",
            "description" => null,
            "livemode" => false,
            "refunds" => null,
            "currency" => "EUR",
            "created_at" => 1349946151,
            "updated_at" => 1349946151,
            "short_id" => '0000.1212.3434',
            "invoices" => array(),
            "payment" => new Models\Response\Payment(),
            "client" => new Models\Response\Client(),
            "preauthorization" => null,
            "fees" => array(),
            "app_id" => null
        );

        foreach ($this->_errorCodes as $responseCode => $errorMessage) {
            if ($responseCode === 20000) {
                continue;
            }
            $response['body']['data']['response_code'] = $responseCode;
            $subject = $this->_responseHandler->convertResponse($response, "transactions/");
            $this->assertInstanceOf("\Paymill\Models\Response\Error", $subject);
            $this->assertEquals($responseCode, $subject->getResponseCode(), "ResponseCode:" . $responseCode . "==" . $subject->getResponseCode() . "\n");
            $this->assertEquals($errorMessage, $subject->getErrorMessage(), "ErrorMessage:" . $errorMessage . "==" . $subject->getErrorMessage() . "\n");
            $response['body']['data']['response_code'] = null;
        }
    }

}