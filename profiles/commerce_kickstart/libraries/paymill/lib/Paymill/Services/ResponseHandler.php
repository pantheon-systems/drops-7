<?php

namespace Paymill\Services;

use Paymill\Models\Response as Models;
use Paymill\Models\Response\Error;

/**
 * ResponseHandler
 */
class ResponseHandler
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
     * Converts a response to a model
     * @param array $response
     * @param string $serviceResource
     * @return Models\Base|Error
     */
    public function convertResponse($response, $serviceResource)
    {
        $resourceName = substr($serviceResource, 0, -2);
        $resultValue = null;
        if ($this->validateResponse($response)) {
            $resultValue = $this->_convertResponseToModel($response['body']['data'], $resourceName);
        } else {
            $resultValue = $this->_convertErrorToModel($response);
        }
        return $resultValue;
    }

    /**
     * Creates an object from a response array based on the call-context
     * @param array $response Response from any Request
     * @param string $resourceName
     * @return Models\Base
     */
    private function _convertResponseToModel($response, $resourceName)
    {
        if (!is_array($response) || empty($response)) {
            return $response;
        }

        $model = null;
        switch (strtolower($resourceName)) {
            case 'client':
                $model = $this->_createClient($response);
                break;
            case 'payment':
                $model = $this->_createPayment($response);
                break;
            case 'transaction':
                $model = $this->_createTransaction($response);
                break;
            case 'preauthorization':
                if (isset($response['preauthorization'])) {
                    $response = $response['preauthorization'];
                }
                $model = $this->_createPreauthorization($response);
                break;
            case 'refund':
                $model = $this->_createRefund($response);
                break;
            case 'offer':
                $model = $this->_createOffer($response);
                break;
            case 'subscription':
                $model = $this->_createSubscription($response);
                break;
            case 'webhook':
                $model = $this->_createWebhook($response);
                break;
        }

        return $model;
    }

    /**
     * Creates and fills a clientmodel
     *
     * @param array $response
     * @return \Paymill\Lib\Models\Response\Client
     */
    private function _createClient($response)
    {
        $model = new Models\Client();
        $model->setId($response['id']);
        $model->setEmail($response['email']);
        $model->setDescription($response['description']);
        $model->setCreatedAt($response['created_at']);
        $model->setUpdatedAt($response['updated_at']);
        $model->setSubscription($this->_handleRecursive($response['subscription'],'subscription'));
        $model->setAppId($response['app_id']);
        $model->setPayment($this->_handleRecursive($response['payment'], 'payment'));
        return $model;
    }

    /**
     * Creates and fills a paymentmodel
     *
     * @param array $response
     * @return \Paymill\Lib\Models\Response\Payment
     */
    private function _createPayment($response)
    {
        $model = new Models\Payment();
        $model->setId($response['id']);
        $model->setType($response['type']);
        $model->setClient($this->_convertResponseToModel($response['client'], "client"));
        if ($response['type'] === "creditcard") {
            $model->setCardType($response['card_type']);
            $model->setCountry($response['country']);
            $model->setExpireMonth($response['expire_month']);
            $model->setExpireYear($response['expire_year']);
            $model->setCardHolder($response['card_holder']);
            $model->setLastFour($response['last4']);
        } else if ($response['type'] === "debit") {
            $model->setCode($response['code']);
            $model->setHolder($response['holder']);
            $model->setAccount($response['account']);
        }
        $model->setCreatedAt($response['created_at']);
        $model->setUpdatedAt($response['updated_at']);
        $model->setAppId($response['app_id']);
        return $model;
    }

    /**
     * Creates and fills a transactionmodel
     *
     * @param array $response
     * @return \Paymill\Lib\Models\Response\Transaction
     */
    private function _createTransaction($response)
    {
        $model = new Models\Transaction();
        $model->setId($response['id']);
        $model->setAmount($response['amount']);
        $model->setOriginAmount($response['origin_amount']);
        $model->setStatus($response['status']);
        $model->setDescription($response['description']);
        $model->setLivemode($response['livemode']);
        $model->setRefunds($this->_handleRecursive($response['refunds'], 'refund'));
        $model->setCurrency($response['currency']);
        $model->setCreatedAt($response['created_at']);
        $model->setUpdatedAt($response['updated_at']);
        $model->setResponseCode($response['response_code']);
        $model->setShortId($response['short_id']);
        $model->setInvoices($response['invoices']);
        $model->setPayment($this->_convertResponseToModel($response['payment'], "payment"));
        $model->setClient($this->_convertResponseToModel($response['client'], "client"));
        $model->setPreauthorization($this->_convertResponseToModel($response['preauthorization'], "preauthorization"));
        $model->setFees($response['fees']);
        $model->setAppId($response['app_id']);
        return $model;
    }

    /**
     * Creates and fills a preauthorizationmodel
     *
     * @param array $response
     * @return \Paymill\Lib\Models\Response\Preauthorization
     */
    private function _createPreauthorization($response)
    {
        $model = new Models\Preauthorization();
        $model->setId($response['id']);
        $model->setAmount($response['amount']);
        $model->setCurrency($response['currency']);
        $model->setStatus($response['status']);
        $model->setLivemode($response['livemode']);
        $model->setCreatedAt($response['created_at']);
        $model->setUpdatedAt($response['updated_at']);
        $model->setPayment($this->_convertResponseToModel($response['payment'], "payment"));
        $model->setClient($this->_convertResponseToModel($response['client'], "client"));
        $model->setAppId($response['app_id']);
        return $model;
    }

    /**
     * Creates and fills a refundmodel
     *
     * @param array $response
     * @return \Paymill\Lib\Models\Response\Refund
     */
    private function _createRefund($response)
    {
        $model = new Models\Refund();
        $model->setId($response['id']);
        $model->setAmount($response['amount']);
        $model->setStatus($response['status']);
        $model->setDescription($response['description']);
        $model->setLivemode($response['livemode']);
        $model->setCreatedAt($response['created_at']);
        $model->setUpdatedAt($response['updated_at']);
        $model->setResponseCode($response['response_code']);
        //Refund doesn't have the array index 'transaction' when using getOne
        $model->setTransaction(isset($response['transaction']) ? $this->_convertResponseToModel($response['transaction'],'transaction'): null);
        $model->setAppId($response['app_id']);
        return $model;
    }

    /**
     * Creates and fills a offermodel
     *
     * @param array $response
     * @return \Paymill\Lib\Models\Response\Offer
     */
    private function _createOffer($response)
    {
        $model = new Models\Offer();
        $model->setId($response['id']);
        $model->setName($response['name']);
        $model->setAmount($response['amount']);
        $model->setCurrency($response['currency']);
        $model->setInterval($response['interval']);
        $model->setTrialPeriodDays($response['trial_period_days']);
        $model->setCreatedAt($response['created_at']);
        $model->setUpdatedAt($response['updated_at']);
        $model->setSubscriptionCount($response['subscription_count']['active'], $response['subscription_count']['inactive']);
        $model->setAppId($response['app_id']);
        return $model;
    }

    /**
     * Creates and fills a subscriptionmodel
     *
     * @param array $response
     * @return \Paymill\Lib\Models\Response\Subscription
     */
    private function _createSubscription($response)
    {
        $model = new Models\Subscription();
        $model->setId($response['id']);
        $model->setOffer($this->_convertResponseToModel($response['offer'],'offer'));
        $model->setLivemode($response['livemode']);
        $model->setCancelAtPeriodEnd($response['cancel_at_period_end']);
        $model->setTrialStart($response['trial_start']);
        $model->setTrialEnd($response['trial_end']);
        $model->setNextCaptureAt($response['next_capture_at']);
        $model->setCreatedAt($response['created_at']);
        $model->setUpdatedAt($response['updated_at']);
        $model->setCanceledAt($response['canceled_at']);
        $model->setPayment($this->_convertResponseToModel($response['payment'], "payment"));
        $model->setClient($this->_convertResponseToModel($response['client'], "client"));
        $model->setAppId($response['app_id']);
        return $model;
    }

    /**
     * Creates and fills a webhookmodel
     *
     * @param array $response
     * @return \Paymill\Lib\Models\Response\Webhook
     */
    private function _createWebhook($response)
    {
        $model = new Models\Webhook();
        $model->setId($response['id']);
        isset($response['url']) ? $model->setUrl($response['url']) : $model->setEmail($response['email']);
        $model->setLivemode($response['livemode']);
        $model->setEventTypes($response['event_types']);
        $model->setCreatedAt($response['created_at']);
        $model->setUpdatedAt($response['updated_at']);
        $model->setAppId($response['app_id']);
        return $model;
    }

    /**
     * Handles the multidimensional param arrays during model creation
     * @param array $response
     * @param string $resourceName
     * @return array|null|Models\Base
     */
    private function _handleRecursive($response, $resourceName)
    {
        $result = null;
        if (isset($response['id'])) {
            $result = $this->_convertResponseToModel($response, $resourceName);
        } else if (!is_null($response)) {
            $paymentArray = array();
            foreach ($response as $paymentData) {
                array_push($paymentArray, $this->_convertResponseToModel($paymentData, $resourceName));
            }
            $result = $paymentArray;
        }
        return $result;
    }

    /**
     * Generates an error model based on the provided response array
     * @param array $response
     * @return Error
     */
    private function _convertErrorToModel($response)
    {
        $errorModel = new Error();

        $httpStatusCode = isset($response['header']['status']) ? $response['header']['status'] : null;
        $errorModel->setHttpStatusCode($httpStatusCode);

        $responseCode = isset($response['body']['data']['response_code']) ? $response['body']['data']['response_code'] : null;
        $errorModel->setResponseCode($responseCode);

        $errorCode = 'Undefined Error. This should not happen!';
        if (isset($responseCode['error'])) {
            $errorCode = $responseCode['error'];
        } elseif (isset($this->_errorCodes[$responseCode])) {
            $errorCode = $this->_errorCodes[$responseCode];
        } else {
            if (isset($response['body']['error'])) {
                if (is_array($response['body']['error'])) {
                    $errorCode = $this->getErrorMessageFromArray($response['body']['error']);
                } elseif (is_string($response['body']['error'])) {
                    $errorCode = $response['body']['error'];
                }
            }
        }

        $errorModel->setErrorMessage($errorCode);
        return $errorModel;
    }

    /**
     * Validates the data responsed by the API
     *
     * Only Refund, Transaction and Preauthorization return an response_code
     * @param array $response
     * @return boolean
     */
    public function validateResponse($response)
    {
        $returnValue = false;
        if ($response['header']['status'] === 200) {
            if (isset($response['body']['data']['response_code'])) {
                $returnValue = false;
                if ($response['body']['data']['response_code'] === 20000) {
                    $returnValue = true;
                }
            } else {
                $returnValue = true;
            }
        }
        return $returnValue;
    }

    private function getErrorMessageFromArray($errorArray)
    {
        $errorMessage = array_shift($errorArray);
        if (is_array($errorMessage)) {
            return $this->getErrorMessageFromArray($errorMessage);
        } else {
            return $errorMessage;
        }
    }

}
