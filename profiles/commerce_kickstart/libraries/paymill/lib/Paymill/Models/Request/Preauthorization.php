<?php

namespace Paymill\Models\Request;

/**
 * Preauthorization Model
 * If you’d like to reserve some money from the client’s credit card but you’d also like to execute the transaction
 * itself a bit later, then use preauthorizations. This is NOT possible with direct debit.
 * A preauthorization is valid for 7 days.
 */
class Preauthorization extends Base
{

    /**
     * @var string
     */
    private $_amount;
    
    /**
     * @var string
     */
    private $_currency;
    
    /**
     * @var string
     */
    private $_payment;
    
    /**
     * @var string
     */
    private $_token;

    /**
     * Creates an instance of the preauthorization request model
     */
    function __construct()
    {
        $this->_serviceResource = 'Preauthorizations/';
    }

    /**
     * Returns the amount
     * @return string
     */
    public function getAmount()
    {
        return $this->_amount;
    }

    /**
     * Sets the amount
     * @param string $amount
     * @return \Paymill\Lib\Models\Request\Preauthorization
     */
    public function setAmount($amount)
    {
        $this->_amount = $amount;
        return $this;
    }

    /**
     * Returns the currency
     * @return string
     */
    public function getCurrency()
    {
        return $this->_currency;
    }

    /**
     * Sets the currency
     * @param string $currency
     * @return \Paymill\Lib\Models\Request\Preauthorization
     */
    public function setCurrency($currency)
    {
        $this->_currency = $currency;
        return $this;
    }

    /**
     * Returns the identifier of a payment
     * @return string
     */
    public function getPayment()
    {
        return $this->_payment;
    }

    /**
     * Sets the identifier of a payment
     * @param string $payment
     * @return \Paymill\Lib\Models\Request\Preauthorization
     */
    public function setPayment($payment)
    {
        $this->_payment = $payment;
        return $this;
    }

    /**
     * Returns the token required for the creation of preAuths
     * @return string
     */
    public function getToken()
    {
        return $this->_token;
    }

    /**
     * Sets the token required for the creation of preAuths
     * @param string $token
     * @return \Paymill\Lib\Models\Request\Preauthorization
     */
    public function setToken($token)
    {
        $this->_token = $token;
        return $this;
    }

    /**
     * Returns an array of parameters customized for the argumented methodname
     * @param string $method
     * @return array
     */
    public function parameterize($method)
    {
        $parameterArray = array();
        switch ($method) {
            case 'create':
                if (!is_null($this->getPayment())) {
                    $parameterArray['payment'] = $this->getPayment();
                } else {
                    $parameterArray['token'] = $this->getToken();
                }
                $parameterArray['amount'] = $this->getAmount();
                $parameterArray['currency'] = $this->getCurrency();

                break;
            case 'getOne':
                $parameterArray['count'] = 1;
                $parameterArray['offset'] = 0;
                break;
            case 'getAll':
            $parameterArray = $this->getFilter();
                break;
            case 'delete':
                break;
        }

        return $parameterArray;
    }
}
