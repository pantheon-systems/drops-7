<?php

require_once ('Base.php');

/**
 * Paymill API wrapper for subscriptions resource
 */
class Services_Paymill_Subscriptions extends Services_Paymill_Base
{
    /**
     * {@inheritDoc}
     */
    protected $_serviceResource = 'subscriptions/';
}