<?php

namespace Paymill\Models\Response;

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
     * @var \Paymill\Lib\Models\Response\Offer 
     */
    private $_offer;
    
    /**
     * @var boolean 
     */
    private $_livemode;
    
    /**
     * @var boolean 
     */
    private $_cancelAtPeriodEnd;
    
    /**
     * @var integer 
     */
    private $_trialStart;
    
    /**
     * @var integer
     */
    private $_trialEnd;
    
    /**
     * @var integer
     */
    private $_nextCaptureAt;
    
    /**
     * @var integer
     */
    private $_canceledAt;
    
    /**
     * @var \Paymill\Lib\Models\Response\Payment 
     */
    private $_payment;
    
    /**
     * @var \Paymill\Lib\Models\Response\Client 
     */
    private $_client;
    
    /**
     * @var integer 
     */
    private $_startAt;

    /**
     * Returns the model of the offer the subscription is based on
     * @return \Paymill\Lib\Models\Response\Offer
     */
    public function getOffer()
    {
        return $this->_offer;
    }

    /**
     * Sets the model of the offer the subscription is based on
     * @param \Paymill\Lib\Models\Response\Offer $offer
     * @return \Paymill\Lib\Models\Response\Subscription
     */
    public function setOffer($offer)
    {
        $this->_offer = $offer;
        return $this;
    }

    /**
     * Returns the flag determining whether this subscription was issued while being in live mode or not.
     * @return boolean
     */
    public function getLivemode()
    {
        return $this->_livemode;
    }

    /**
     * Sets the flag determining whether this subscription was issued while being in live mode or not.
     * @param string $livemode
     * @return \Paymill\Lib\Models\Response\Subscription
     */
    public function setLivemode($livemode)
    {
        $this->_livemode = $livemode;
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
     * @return \Paymill\Lib\Models\Response\Subscription
     */
    public function setCancelAtPeriodEnd($cancelAtPeriodEnd)
    {
        $this->_cancelAtPeriodEnd = $cancelAtPeriodEnd;
        return $this;
    }

    /**
     * Returns the Unix-Timestamp for the trial period start
     * @return integer
     */
    public function getTrialStart()
    {
        return $this->_trialStart;
    }

    /**
     * Sets the Unix-Timestamp for the trial period start
     * @param integer $trialStart
     * @return \Paymill\Lib\Models\Response\Subscription
     */
    public function setTrialStart($trialStart)
    {
        $this->_trialStart = $trialStart;
        return $this;
    }

    /**
     * Returns the Unix-Timestamp for the trial period end.
     * @return integer
     */
    public function getTrialEnd()
    {
        return $this->_trialEnd;
    }

    /**
     * Sets the Unix-Timestamp for the trial period end.
     * @param integer $trialEnd
     * @return \Paymill\Lib\Models\Response\Subscription
     */
    public function setTrialEnd($trialEnd)
    {
        $this->_trialEnd = $trialEnd;
        return $this;
    }

    /**
     * Returns the Unix-Timestamp for the next charge.
     * @return integer
     */
    public function getNextCaptureAt()
    {
        return $this->_nextCaptureAt;
    }

    /**
     * Sets the Unix-Timestamp for the next charge.
     * @param integer $nextCaptureAt
     * @return \Paymill\Lib\Models\Response\Subscription
     */
    public function setNextCaptureAt($nextCaptureAt)
    {
        $this->_nextCaptureAt = $nextCaptureAt;
        return $this;
    }

    /**
     * Returns the Unix-Timestamp for the cancel date.
     * @return integer
     */
    public function getCanceledAt()
    {
        return $this->_canceledAt;
    }

    /**
     * Sets the Unix-Timestamp for the cancel date.
     * @param integer $canceledAt
     * @return \Paymill\Lib\Models\Response\Subscription
     */
    public function setCanceledAt($canceledAt)
    {
        $this->_canceledAt = $canceledAt;
        return $this;
    }

    /**
     * Returns the payment object registered with this subscription
     * @return \Paymill\Lib\Models\Response\Payment
     */
    public function getPayment()
    {
        return $this->_payment;
    }

    /**
     * Sets the payment object registered with this subscription
     * @param \Paymill\Lib\Models\Response\Payment $payment
     * @return \Paymill\Lib\Models\Response\Subscription
     */
    public function setPayment($payment)
    {
        $this->_payment = $payment;
        return $this;
    }

    /**
     * Returns the client associated with this subscription
     * @return \Paymill\Lib\Models\Response\Client
     */
    public function getClient()
    {
        return $this->_client;
    }

    /**
     * Sets the client associated with this subscription
     * @param \Paymill\Lib\Models\Response\Client $client
     * @return \Paymill\Lib\Models\Response\Subscription
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
     * @return \Paymill\Lib\Models\Response\Subscription
     */
    public function setStartAt($startAt)
    {
        $this->_startAt = $startAt;
        return $this;
    }

}