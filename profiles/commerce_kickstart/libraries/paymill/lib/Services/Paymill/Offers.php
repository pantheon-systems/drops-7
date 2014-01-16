<?php

require_once ('Base.php');

/**
 * Paymill API wrapper for offers resource
 */
class Services_Paymill_Offers extends Services_Paymill_Base
{
    /**
     * {@inheritDoc}
     */
    protected $_serviceResource = 'offers/';
}