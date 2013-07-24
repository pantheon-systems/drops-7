<?php

require_once ('Base.php');

/**
 * Paymill API wrapper for clients resource
 */
class Services_Paymill_Clients extends Services_Paymill_Base
{
    /**
     * {@inheritDoc}
     */
    protected $_serviceResource = 'clients/';
}