<?php

namespace Paymill\Models\Request;

/**
 * Subscription Model
 * Subscriptions allow you to charge recurring payments on a client’s credit card / to a client’s direct debit.
 * A subscription connects a client to the offers-object. A client can have several subscriptions to different offers,
 * but only one subscription to the same offer.
 * @tutorial https://paymill.com/de-de/dokumentation/referenz/api-referenz/#document-subscriptions
 */
class Subscription extends Base
{

    /**
     * @var string
     */
    private $_offer;
    
    /**
     * @var boolean
     */
    private $_cancelAtPeriodEnd;
    
    /**
     * @var string
     */
    private $_payment;
    
    /**
     * @var string
     */
    private $_client;
    
    /**
     * @var integer
     */
    private $_startAt;

    /**
     * Creates an instance of the subscription request model
     */
    public function __construct()
    {
        $this->_serviceResource = 'Subscriptions/';
    }

    /**
     * Returns the identifier of the offer the subscription is based on
     * @return string
     */
    public function getOffer()
    {
        return $this->_offer;
    }

    /**
     * Sets the identifier of the offer the subscription is based on
     * @param string $offer
     * @return \Paymill\Lib\Models\Request\Subscription
     */
    public function setOffer($offer)
    {
        $this->_offer = $offer;
        return $this;
    }

    /**
     * Returns the flag determining whether to cancel this subscription immediately or at the end of the current period
     * @return boolean
     */
    public function getCancelAtPeriodEnd()
    {
        return $this->_cancelAtPeriodEnd;
    }

    /**
     * Sets a flag determining whether to cancel this subscription immediately or at the end of the current period
     * @param boolean $cancelAtPeriodEnd
     * @return \Paymill\Lib\Models\Request\Subscription
     */
    public function setCancelAtPeriodEnd($cancelAtPeriodEnd)
    {
        $this->_cancelAtPeriodEnd = $cancelAtPeriodEnd;
        return $this;
    }

    /**
     * Returns the identifier of the payment object registered with this subscription
     * @return string
     */
    public function getPayment()
    {
        return $this->_payment;
    }

    /**
     * Sets the identifier of the payment object registered with this subscription
     * @param string $payment
     * @return \Paymill\Lib\Models\Request\Subscription
     */
    public function setPayment($payment)
    {
        $this->_payment = $payment;
        return $this;
    }

    /**
     * Returns the id of the client associated with this subscription
     * @return string
     */
    public function getClient()
    {
        return $this->_client;
    }

    /**
     * Sets the id of the client associated with this subscription
     * @param string $client
     * @return \Paymill\Lib\Models\Request\Subscription
     */
    public function setClient($client)
    {
        $this->_client = $client;
        return $this;
    }

    /**
     * Returns the Unix-Timestamp for the trial period start
     * @return integer
     */
    public function getStartAt()
    {
        return $this->_startAt;
    }

    /**
     * Sets the Unix-Timestamp for the trial period start
     * @param integer $startAt
     * @return \Paymill\Lib\Models\Request\Subscription
     */
    public function setStartAt($startAt)
    {
        $this->_startAt = $startAt;
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
                $parameterArray['client'] = $this->getClient();
                $parameterArray['offer'] = $this->getOffer();
                $parameterArray['payment'] = $this->getPayment();
                $parameterArray['start_at'] = $this->getStartAt();
                break;
            case 'update':
                $parameterArray['cancel_at_period_end'] = $this->getCancelAtPeriodEnd();
                $parameterArray['offer'] = $this->getOffer();
                $parameterArray['payment'] = $this->getPayment();
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
