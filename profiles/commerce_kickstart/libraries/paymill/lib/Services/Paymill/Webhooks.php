<?php

require_once ('Base.php');

/**
 * Paymill API wrapper for webhooks resource
 */
class Services_Paymill_Webhooks extends Services_Paymill_Base
{
    /**
     * {@inheritDoc}
     */
    protected $_serviceResource = 'webhooks/';
}