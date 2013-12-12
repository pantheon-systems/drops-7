<?php

namespace Paymill\Models\Response;

/**
 * Preauthorization Model
 * If you’d like to reserve some money from the client’s credit card but you’d also like to execute the transaction 
 * itself a bit later, then use preauthorizations. This is NOT possible with direct debit.
 * A preauthorization is valid for 7 days.
 */
class Preauthorization extends Base
{
    /**
     * @var integer
     */
    private $_amount;
   
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
     * @return \Paymill\Lib\Models\Response\Preauthorization
     */
    public function setAmount($amount)
    {
        $this->_amount = $amount;
        return $this;
    }
    
    
    /**
     * @var string
     */
    private $_currency;
    
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
     * @return \Paymill\Lib\Models\Response\Preauthorization
     */
    public function setCurrency($currency)
    {
        $this->_currency = $currency;
        return $this;
    }
    
    /**
     * Indicates the current status (open, pending, closed, failed, deleted, preauth)
     * @var string
     */
    private $_status;

    /**
     * Returns the status
     * @return string
     */
    public function getStatus()
    {
        return $this->_status;
    }

    /**
     * Sets the status
     * @param string $status
     * @return \Paymill\Lib\Models\Response\Preauthorization
     */
    public function setStatus($status)
    {
        $this->_status = $status;
        return $this;
    }
    
    /**
     * Whether this preauthorization was issued while being in live mode or not
     * @var boolean
     */
    private $_livemode;
    
    /**
     * Returns the livemode flag of the preAuth object
     * @return boolean
     */
    public function getLivemode()
    {
        return $this->_livemode;
    }

    /**
     * Sets the livemode flag of the preAuth object
     * @param boolean $livemode
     * @return \Paymill\Lib\Models\Response\Preauthorization
     */
    public function setLivemode($livemode)
    {
        $this->_livemode = $livemode;
        return $this;
    }
    
    /**
     * Payment Response Model
     * @var Payment
     */
    private $_payment;
    
    /**
     * Returns the identifier of a payment 
     * @return Payment
     */
    public function getPayment()
    {
        return $this->_payment;
    }

    /**
     * Sets the identifier of a payment 
     * @param Payment $payment
     * @return \Paymill\Lib\Models\Response\Preauthorization
     */
    public function setPayment($payment)
    {
        $this->_payment = $payment;
        return $this;
    }
    
    /**
     * Client Response Model
     * @var Client
     */
    private $_client;

    /**
     * Returns the identifier of a client
     * @return Client
     */
    public function getClient()
    {
        return $this->_client;
    }

    /**
     * Sets the identifier of a client
     * @param Client $client
     * @return \Paymill\Lib\Models\Response\Preauthorization
     */
    public function setClient($client)
    {
        $this->_client = $client;
        return $this;
    }

}